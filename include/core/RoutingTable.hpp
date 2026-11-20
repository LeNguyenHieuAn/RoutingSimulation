#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Route.hpp"

class RoutingTable {
public:
    RoutingTable() = default;
    ~RoutingTable() = default;
    const std::unordered_map<std::string, Route>& getRoutes() const noexcept;
    void addRoute(const Route& route);
    void removeRoute(const std::string& destIP);
    Route findRoute(const std::string& destIP) const;
    bool hasRoute(const std::string& destIP) const noexcept;
    void clear() noexcept;
private:
    std::unordered_map<std::string, Route> routes{};
};