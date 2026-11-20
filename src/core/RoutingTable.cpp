#include "RoutingTable.hpp"
#include "Exceptions.hpp"
#include <algorithm>
#include <stdexcept>

const std::unordered_map<std::string, Route>& RoutingTable::getRoutes() const noexcept {
    return routes;
}

void RoutingTable::addRoute(const Route& route) {
    if (route.getDestIP().empty() || route.getInterfaceId().empty()) {
        throw std::invalid_argument("RoutingTable Error: Destination IP and Interface ID cannot be empty.");
    }
    routes.insert_or_assign(route.getDestIP(), route);
}

void RoutingTable::removeRoute(const std::string& destIP) {
    if (routes.erase(destIP) == 0) throw NoRouteException{destIP};
}

Route RoutingTable::findRoute(const std::string& destIP) const {
    const auto it = routes.find(destIP);
    if (it == routes.end()) {
        throw NoRouteException{destIP};
    }
    return it->second;
}

bool RoutingTable::hasRoute(const std::string& destIP) const noexcept {
    return routes.find(destIP) != routes.end();
}

void RoutingTable::clear() noexcept {
    routes.clear(); 
}
