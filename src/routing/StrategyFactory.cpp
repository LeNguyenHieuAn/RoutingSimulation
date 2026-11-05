#include "routing/StrategyFactory.hpp"
#include "routing/StaticRouting.hpp"
#include "routing/ShortestPathRouting.hpp"
#include "routing/CostBasedRouting.hpp"

#include <stdexcept>
#include <utility>
#include <vector>
std::unique_ptr<RoutingStrategy> StrategyFactory::createStrategy(const std::string& type) {
    std::vector<std::unique_ptr<RoutingStrategy>> strategies;
    strategies.push_back(std::make_unique<StaticRouting>());
    strategies.push_back(std::make_unique<ShortestPathRouting>());
    strategies.push_back(std::make_unique<CostBasedRouting>());

    const std::string name =
        type == "shortest" ? "bfs" :
        type == "cost" ? "dijkstra" : type;

    for (auto& strategy : strategies) 
        if (strategy->getName() == name) {
            return std::move(strategy);
        }
    
    throw std::invalid_argument("Unknown routing strategy: " + type);
}