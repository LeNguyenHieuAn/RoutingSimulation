#include "core/Interface.hpp"
#include "core/Router.hpp"
#include "RoutingHelpers.hpp"
#include <algorithm>
#include <tuple>
#include <vector>

namespace RoutingHelpers{
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
            if(peerIt == peerInterfaces.end()) continue;
            const auto& peer = peerIt -> second;
            if(!peer || !peer -> getStatus())
                continue;
            if(peer -> getId() != iface -> getLinkInterface() ||
                peer -> getLinkIP() != router.getIP() ||
                peer -> getLinkInterface() != iface -> getId()){
                    continue;
                }
            result.push_back({iface -> getLinkIP(), id, iface -> getCost()});
        }
        std::sort(result.begin(), result.end(), [](const Edge& a, const Edge& b){
            return std::tie(a.neighborIP, a.localInterface, a.cost) 
                < std::tie(b.neighborIP, b.localInterface, b.cost);
        });
        return result;
    }
    bool better(const PathInfo& A, const PathInfo& B){
        return std::tie(A.cost, A.hops, A.interfaceId, A.nextIP)
            < std::tie(B.cost, B.hops, B.interfaceId, B.nextIP);
    }
}