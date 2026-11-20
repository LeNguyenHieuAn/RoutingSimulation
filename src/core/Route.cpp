#include "Route.hpp"
#include <stdexcept>
#include <tuple>

Route::Route(std::string destIP_, std::string nextIP_, std::string interfaceId_, int hopCount_, long long cost_)
    : destIP(std::move(destIP_)), 
    nextIP(std::move(nextIP_)), 
    interfaceId(std::move(interfaceId_)), 
    hopCount(hopCount_), 
    cost(cost_) {
    if (this->destIP.empty()) {
        throw std::invalid_argument("Route error: Destination IP cannot be empty.");
    }
    if (this->interfaceId.empty()) {
        throw std::invalid_argument("Route error: Interface ID cannot be empty.");
    }
    if (this->hopCount < 0) {
        throw std::invalid_argument("Route error: Hop count cannot be negative.");
    }
    if (this->cost < 0) {
        throw std::invalid_argument("Route error: Route cost cannot be negative.");
    }
}

const std::string& Route::getDestIP() const noexcept {
    return destIP;
}

const std::string& Route::getNextIP() const noexcept {
    return nextIP;
}

const std::string& Route::getInterfaceId() const noexcept {
    return interfaceId;
}

int Route::getHopCount() const noexcept {
    return hopCount;
}

long long Route::getCost() const noexcept {
    return cost;
}

bool Route::operator==(const Route& other) const noexcept {
    return std::tie(destIP, nextIP, interfaceId, hopCount, cost) ==
           std::tie(other.destIP, other.nextIP, other.interfaceId, other.hopCount, other.cost);
}

bool Route::operator < (const Route& other) const noexcept {
    return std::tie(destIP, nextIP, interfaceId, hopCount, cost) <
           std::tie(other.destIP, other.nextIP, other.interfaceId, other.hopCount, other.cost);
}

std::ostream& operator<<(std::ostream& os, const Route& route) {
    os << "[Dest: " << (route.destIP.empty() ? "None" : route.destIP)
       << " | NextHop: " << (route.nextIP.empty() ? "Direct" : route.nextIP)
       << " | Iface: " << route.interfaceId
       << " | Hops: " << route.hopCount
       << " | Cost: " << route.cost << "]";
    return os;
}