#include "core/Router.hpp"
#include "StaticRouting.hpp"
using Topology = std::unordered_map<std::string, std::shared_ptr<Router>>;

void StaticRouting::calculateRoutes(Router& self, const Topology& /*topology*/){
    self.getDynamicRoutingTable().clear();
}

std::string StaticRouting::getName() const {
    return "static";
}