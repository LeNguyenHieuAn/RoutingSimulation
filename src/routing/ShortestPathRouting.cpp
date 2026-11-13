#include "routing/ShortestPathRouting.hpp"
#include "core/Router.hpp"
#include "core/Interface.hpp"
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <stdexcept>
#include <utility>
#include <vector>
#include <tuple>
#include <climits>
using Topology = std::unordered_map<std::string, std::shared_ptr<Router>>;
namespace{
    struct Edge{
        std::string neighborIP;
        std::string localInterface;
        long long cost;
    };

    struct PathInfo{
        long long cost;
        int hops;
        std::string nextIP;
        std::string interfaceId;
    };

    std::vector<Edge> activeNeighbors(const Router& router, const Topology& topology){
        std::vector<Edge> result;
        for(const auto& [id, iface] : router.getInterfaces()){
            if(!iface || !iface -> getStatus()) 
                continue;
            const auto routerIt = topology.find(iface -> getLinkIP());
            if(routerIt == topology.end() || !routerIt -> second)
                continue;
            const auto& peerInterfaces = routerIt -> second -> getInterfaces();
            const auto peerIt = peerInterfaces.find(iface -> getLinkInterface());
            if(peerIt == peerInterfaces.end())
                continue;
            const auto& peer = peerIt -> second;
            if(!peer || !peer -> getStatus()) continue;
            if(peer -> getId() != iface -> getLinkInterface() ||
                peer -> getLinkIP() != router.getIP() ||
                peer -> getLinkInterface() != iface -> getId()){
                    continue;
            }
            result.push_back({iface -> getLinkIP(), id, iface -> getCost()});
        }    

        std::sort(result.begin(), result.end(), [](const Edge& A, const Edge& B){
            return std::tie(A.neighborIP, A.localInterface, A.cost) 
                < std::tie(B.neighborIP, B.localInterface, B.cost);
        });

        return result;
    }
    
}

void ShortestPathRouting::calculateRoutes(Router& self, const Topology& topology){
    auto& table = self.getDynamicRoutingTable();
    table.clear();
    const std::string source = self.getIP();
    std::queue<std::string> q;
    std::unordered_map<std::string, PathInfo> best;
    best.emplace(source, PathInfo{0, 0, "", ""});
    q.push(source);
    while (!q.empty()) {
        const std::string u = q.front();
        q.pop();
        const auto routerIt = topology.find(u);
        if (routerIt == topology.end() || !routerIt->second)
            continue;
        const auto current = best.at(u);
        for (const auto& edge : activeNeighbors(*routerIt->second, topology)){
            if (best.find(edge.neighborIP) != best.end())
                continue;
            if (current.hops == INT_MAX) {
                throw std::overflow_error("BFS hop count overflow");
            }
            PathInfo candidate{current.cost + 1, current.hops + 1,
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