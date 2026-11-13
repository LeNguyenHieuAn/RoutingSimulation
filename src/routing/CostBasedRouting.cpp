#include "routing/CostBasedRouting.hpp"
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
    bool better(const PathInfo& A, const PathInfo& B){
        return std::tie(A.cost, A.hops, A.interfaceId, A.nextIP)
            < std::tie(B.cost, B.hops, B.interfaceId, B.nextIP);
    }  
}


void CostBasedRouting::calculateRoutes(Router& self, const Topology& topology) {
    auto& table = self.getDynamicRoutingTable();
    table.clear();
    const std::string source = self.getIP();
    std::unordered_map<std::string, PathInfo> best;
    best.emplace(source, PathInfo{0, 0, "", ""});
    
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
        for (const auto& edge : activeNeighbors(*routerIt->second, topology)) {
            if (edge.cost <= 0) {
                throw std::invalid_argument("Dijkstra requires positive link costs");
            }
            if (current.cost > LLONG_MAX - edge.cost || current.hops == INT_MAX) {
                throw std::overflow_error("Dijkstra distance overflow");
            }
            PathInfo candidate{current.cost + edge.cost, current.hops + 1,
                u == source ? edge.neighborIP : current.nextIP,
                u == source ? edge.localInterface : current.interfaceId
            };
            auto old = best.find(edge.neighborIP);
            if (old == best.end() || better(candidate, old->second)) {
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