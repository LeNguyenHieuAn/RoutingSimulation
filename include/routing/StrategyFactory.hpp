#pragma once
#include "RoutingStrategy.hpp"
#include <memory>
#include <vector>
#include <string>

class StrategyFactory{
public:
    static std::unique_ptr<RoutingStrategy> createStrategy(const std::string& type);
};