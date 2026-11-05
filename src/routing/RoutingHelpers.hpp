#pragma once
#include "core/Router.hpp"
#include <string>
#include <vector>
using Topology = std::unordered_map<std::string, std::shared_ptr<Router>>;
const long long loo = 1e18;
const int oo = 1e9;
namespace RoutingHelpers {
struct Edge {
    std::string neighborIP;
    std::string localInterface;
    long long cost;
};
struct PathInfo {
    long long cost;
    int hops;
    std::string nextIP;
    std::string interfaceId;
};
std::vector<Edge> activeNeighbors(const Router& router, const Topology& topology);
bool better(const PathInfo& a, const PathInfo& b);
}
