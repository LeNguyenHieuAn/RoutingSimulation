#pragma once
#include "RoutingStrategy.hpp"
class StaticRouting final : public RoutingStrategy{
public:
    void calculateRoutes(Router& self, const std::unordered_map<std::string, std::shared_ptr<Router>>& topology) override;
    std::string getName() const override;

};
