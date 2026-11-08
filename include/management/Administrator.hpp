#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "Router.hpp"
#include "Route.hpp"
#include "CsvTopologyStorage.hpp"
#include "TextLogger.hpp"

class Administrator{
private:
    std::unordered_map<std::string,std::shared_ptr<Router>>routers;
    TextLogger logger;
public:
    Administrator(std::string = "./data/network_events.txt");
    void importNetworkFromCsv(const std::string&);
    void exportNetworkToCsv(const std::string&);
    void addStaticRoute(const std::string&, const Route&);
    void removeStaticRoute(const std::string&, const std::string&);
    void changeGlobalStrategy(const std::string&);
    void convergeNetwork();
    void setLinkStatus(const std::string&, const std::string&, bool);
    void sendPacket(const std::string&, const std::string&, const std::string&, int);
    void printAllRoutingTables() const;
};
