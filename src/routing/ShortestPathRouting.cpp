#include "routing/ShortestPathRouting.hpp"
#include "core/Router.hpp"
#include "RoutingHelpers.hpp"

#include <queue>
#include <unordered_map>
#include <stdexcept>
#include <utility>

void ShortestPathRouting::calculateRoutes(Router& self, const Topology& topology){
    auto& table = self.getDynamicRoutingTable();
    table.clear();
    const std::string source = self.getIP();
    std::queue<std::string> q;
    std::unordered_map<std::string, RoutingHelpers::PathInfo> best;
    best.emplace(source, RoutingHelpers::PathInfo{0, 0, "", ""});
    q.push(source);
    while (!q.empty()) {
        const std::string u = q.front();
        q.pop();
        const auto routerIt = topology.find(u);
        if (routerIt == topology.end() || !routerIt->second)
            continue;
        const auto current = best.at(u);
        for (const auto& edge : RoutingHelpers::activeNeighbors(*routerIt->second, topology)){
            if (best.find(edge.neighborIP) != best.end())
                continue;
            if (current.hops == oo) {
                throw std::overflow_error(
                    "BFS hop count overflow"
                );
            }
            RoutingHelpers::PathInfo candidate{current.cost + 1, current.hops + 1,
                u == source ? edge.neighborIP : current.nextIP,
                u == source ? edge.localInterface : current.interfaceId
            };
            best.emplace(edge.neighborIP, std::move(candidate));
            q.push(edge.neighborIP);
        }
    }

    for (const auto& [dest, p] : best) {
        if (dest == source)
            continue;
        table.addRoute(Route(dest, p.nextIP, p.interfaceId, p.hops, p.cost));
    }
}

std::string ShortestPathRouting::getName() const {
    return "bfs";
}