#include "Administrator.hpp"
#include "CsvTopologyStorage.hpp"
#include "Route.hpp"
#include "StrategyFactory.hpp"
#include "Exceptions.hpp"
#include <string>
#include <stdexcept>
#include <iomanip>
#include <iostream>

namespace {
    void printTable(const std::unordered_map<std::string, Route>& routes) {
        if (routes.empty()) {
            std::cout << "      (Bảng định tuyến rỗng)\n";
            return;
        }
        std::cout << "      " << std::left
                << std::setw(18) << "Destination"
                << std::setw(18) << "Next Hop"
                << std::setw(15) << "Interface"
                << std::setw(10) << "Hops"
                << "Cost\n";

        std::cout << "      ------------------------------------------------------------------\n";

        for (const auto& [destIP, route] : routes) {
            std::cout << "      " << std::left
                    << std::setw(18) << route.getDestIP()
                    << std::setw(18) << route.getNextIP()
                    << std::setw(15) << route.getInterfaceId()
                    << std::setw(10) << route.getHopCount()
                    << route.getCost() << "\n";
        }
    }
}

Administrator::Administrator(std::string logFile) : logger(std::move(logFile)) {}

void Administrator::importNetworkFromCsv(const std::string& csvPath){
    CsvTopologyStorage::loadTopology(csvPath, routers, logger);
    convergeNetwork();
}

void Administrator::exportNetworkToCsv(const std::string& csvPath){
    CsvTopologyStorage::saveTopology(csvPath, routers, logger);
}

void Administrator::addStaticRoute(const std::string& routerIP, const Route& route){
    if (!routers.contains(routerIP))
        throw std::invalid_argument("Router IP is not found");
    auto& staticTable = routers[routerIP]->getStaticRoutingTable();
    staticTable.addRoute(route);
}

void Administrator::removeStaticRoute(const std::string& routerIP, const std::string& destIP){
    if (!routers.contains(routerIP))
        throw std::invalid_argument("Router IP is not found");
    auto& staticTable = routers[routerIP]->getStaticRoutingTable();
    staticTable.removeRoute(destIP);
}

void Administrator::changeGlobalStrategy(const std::string& StrategyType){
    auto strategyType = StrategyFactory::createStrategy(StrategyType);
    const auto& strategyName = strategyType->getName();
    for (auto& [_,router] : routers)
        router->setStrategy(StrategyFactory::createStrategy(StrategyType));
    convergeNetwork();
    logger.logInfo("Global strategy switched to " + strategyName);
}

void Administrator::convergeNetwork(){
    for (const auto& [_,router] : routers)
        router->updateRoutes(routers);
    logger.logInfo("All routers has been updated");
}

void Administrator::setLinkStatus(const std::string& routerIP, const std::string& ifaceId, bool isUP){
    if (!routers.contains(routerIP))
        throw std::invalid_argument("Router IP is not found");
    const auto iface = routers[routerIP]->getInterface(ifaceId);
    iface->setStatus(isUP);
}

void Administrator::sendPacket(const std::string& srcIP, const std::string& destIP, const std::string& payload, int ttl){
    if (!routers.contains(srcIP))
        throw std::invalid_argument("Unknown source router: " + destIP);
    if (!routers.contains(destIP))
        throw std::invalid_argument("Unknown destination: " + destIP);
    int count = 0;
    int maxRetries = 3;
    bool isDeliveried = false;
    while (!isDeliveried && count < maxRetries){
        Packet packet(srcIP, destIP, payload, ttl);
        std::string current = srcIP;
        std::cout << "Packet path: " << current;
        try{
            while (current != destIP){
                if (packet.getTTL() <= 0)
                        throw std::runtime_error("TTL expired in transit");
                std::string nextIP = routers[current]->forwardPacket(packet);
                if (!routers.contains(nextIP))
                    throw NoRouteException(nextIP);
                std::cout << " -> " << nextIP;
                current = nextIP;
            }
            std::cout << "\n DELIVERED | TTL=" << packet.getTTL() << '\n';
            logger.logInfo("Packet delivered " + srcIP + " -> " + destIP + " (remaining TTL=" + std::to_string(packet.getTTL()) + ")");
            isDeliveried = true;
        }
        catch (const NoRouteException& msg){
            std::cout << " | FAILED: " << msg.what() << '\n';
            logger.logError("Packet failed " + srcIP + " -> " + destIP + ": " + msg.what());
        }
        catch (const InterfaceDownException& msg){
            std::cout << " | FAILED: " << msg.what() << '\n';
            std::cout << "Rerouting Topology, please wait!";
            convergeNetwork();
            std::cout << "Rerouting successfully, retrying send packet";
            sendPacket(srcIP, destIP, payload, ttl);
            logger.logError("Packet failed " + srcIP + " -> " + destIP + ": " + msg.what());
        }
        catch (const std::exception &msg){
            std::cout << " | FAILED: " << msg.what() << '\n';
            logger.logError("Packet failed " + srcIP + " -> " + destIP + ": " + msg.what());
        }
        count = count + 1;
    }
    if (!isDeliveried){
        std::cout << "Max retries reached. Packet dropped.\n";

    }
}

void Administrator::printAllRoutingTables() const {
    if (routers.empty()) {
        logger.logInfo("Requested to print routing tables, but network is empty.");
        std::cout << "\n[!] Mạng hiện tại chưa có Router nào.\n";
        return;
    }
    std::cout << "\n========================================================================\n";
    std::cout << "                   BẢNG ĐỊNH TUYẾN TOÀN MẠNG (GLOBAL ROUTING)           \n";
    std::cout << "========================================================================\n";
    for (const auto& [ip, router] : routers) {
        if (!router)
            continue;
        std::cout << "\n[Router IP: " << ip << "]\n";
        std::cout << "  > Static Routing Table:\n";
        const auto& staticRoutes = router->getStaticRoutingTable().getRoutes();
        printTable(staticRoutes);
        std::cout << "  > Dynamic Routing Table (Calculated by Strategy):\n";
        const auto& dynamicRoutes = router->getDynamicRoutingTable().getRoutes();
        printTable(dynamicRoutes);
    }
    std::cout << "========================================================================\n\n";
    logger.logInfo("Printed all routing tables successfully.");
}
