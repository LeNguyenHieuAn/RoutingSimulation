#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class Router;
class RoutingStrategy{
public:
    virtual void calculateRoutes(Router& self, const std::unordered_map<std::string, std::shared_ptr<Router>>& topology) = 0;
    virtual std::string getName() const = 0;
    virtual ~RoutingStrategy() = default;
};
