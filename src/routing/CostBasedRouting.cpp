#include "routing/CostBasedRouting.hpp"
#include "core/Router.hpp"
#include "RoutingHelpers.hpp"

#include <functional>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>

void CostBasedRouting::calculateRoutes(Router& self, const Topology& topology) {
    auto& table = self.getDynamicRoutingTable();
    table.clear();
    const std::string source = self.getIP();
    std::unordered_map<std::string, RoutingHelpers::PathInfo> best;
    best.emplace(source, RoutingHelpers::PathInfo{0, 0, "", ""});
    
    using State = std::tuple<long long, int, std::string>;

    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    pq.emplace(0, 0, source);

    while (!pq.empty()) {
        auto [distance, hops, u] = pq.top();
        pq.pop();
        const auto currentIt = best.find(u);
        if (currentIt == best.end() ||
            currentIt->second.cost != distance ||
            currentIt->second.hops != hops) {
            continue;
        }
        const auto routerIt = topology.find(u);
        if (routerIt == topology.end() || !routerIt->second)
            continue;
        const auto current = currentIt->second;
        for (const auto& edge : RoutingHelpers::activeNeighbors(*routerIt->second, topology)) {
            if (edge.cost <= 0) {
                throw std::invalid_argument("Dijkstra requires positive link costs");
            }
            if (current.cost > loo - edge.cost || current.hops == oo) {
                throw std::overflow_error("Dijkstra distance overflow");
            }
            RoutingHelpers::PathInfo candidate{current.cost + edge.cost, current.hops + 1,
                u == source ? edge.neighborIP : current.nextIP,
                u == source ? edge.localInterface : current.interfaceId
            };
            auto old = best.find(edge.neighborIP);
            if (old == best.end() || RoutingHelpers::better(candidate, old->second)) {
                best.insert_or_assign(edge.neighborIP, candidate);
                pq.emplace(candidate.cost, candidate.hops, edge.neighborIP);
            }
        }
    }

    for (const auto& [dest, p] : best) {
        if (dest == source)
            continue;
        table.addRoute(Route(dest, p.nextIP, p.interfaceId, p.hops, p.cost));
    }
}

std::string CostBasedRouting::getName() const {
    return "dijkstra";
}