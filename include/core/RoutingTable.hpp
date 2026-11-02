#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Route.hpp"

class RoutingTable {
public:
    
    void addRoute(const Route& route);

    void removeRoute(const std::string& destIP);

    Route findRoute(const std::string& destIP) const;

    bool hasRoute(const std::string& destIP) const;

    void clear() noexcept;

    std::vector<Route> getSortedRoutes() const;

private:
    std::unordered_map<std::string, Route> routes;
};