#include "CsvTopologyStorage.hpp"
#include "Router.hpp"
#include "Interface.hpp"
#include "Route.hpp"
#include "TextLogger.hpp"
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stdexcept>
#include <sstream>

namespace{
    struct Router_{
        std::string ipAddress;
    };

    struct Link_{
        std::string ipA,ifaceA;
        std::string ipB,ifaceB;
        long long cost;
        bool isUp;
    };

    struct StaticRoute_{
        std::string routerIP;
        std::string destIP;
        std::string nextIP;
        std::string interfaceId;
        int hopCount;
        long long cost;
    };

    void addRouter(std::unordered_map<std::string,std::shared_ptr<Router>>& routers, const Router_& router){
        if (!routers.contains(router.ipAddress))
            routers[router.ipAddress] = std::make_shared<Router>(router.ipAddress);
        else
            throw std::invalid_argument("Router IP is duplicated");
    }

    void addLink(std::unordered_map<std::string,std::shared_ptr<Router>>& routers, const Link_& link){
        if (!routers.contains(link.ipA) || !routers.contains(link.ipB))
            throw std::invalid_argument("Cannot add link, one of the routers or both of them does not exist");
        std::shared_ptr<Interface>ifaceA = std::make_shared<Interface>(link.ifaceA, link.ipB, link.ifaceB, link.isUp, link.cost);
        std::shared_ptr<Interface>ifaceB = std::make_shared<Interface>(link.ifaceB, link.ipA, link.ifaceA, link.isUp, link.cost);
        ifaceA->connectPeer(ifaceB);
        ifaceB->connectPeer(ifaceA);
        routers[link.ipA]->addInterface(ifaceA);
        routers[link.ipB]->addInterface(ifaceB);
    }

    void addStaticRoute(std::unordered_map<std::string,std::shared_ptr<Router>>& routers, const StaticRoute_& route_){
        if (!routers.contains(route_.routerIP))
            throw std::invalid_argument("Router IP is not found");
        if (!routers.contains(route_.destIP))
            throw std::invalid_argument("Dest IP is not found");
        if (!routers.contains(route_.nextIP))
            throw std::invalid_argument("Next IP is not found");
        const auto& Ifaces = routers[route_.routerIP]->getInterfaces();
        if (!Ifaces.contains(route_.interfaceId))
            throw std::invalid_argument("Interface ID is not found");
        Route route{route_.destIP, route_.nextIP, route_.interfaceId, route_.hopCount, route_.cost};
        auto& StaticRoutingTable = routers[route_.routerIP]->getStaticRoutingTable();
        StaticRoutingTable.addRoute(route);
    }
}

void CsvTopologyStorage::loadTopology(const std::string& csvPath, std::unordered_map<std::string,std::shared_ptr<Router>>& routers,const TextLogger& logger){
    std::ifstream fIn(csvPath,std::ios::in);
    if (!fIn.is_open())
        throw std::runtime_error("Cannot open csv file");
    logger.logInfo("Loading Topology from CSV file");
    std::vector<std::string>row;
    std::vector<Router_>routers_;
    std::vector<Link_>links_;
    std::vector<StaticRoute_>routes_;
    std::string words, line;
    int countLine = 0;
    while (getline(fIn,line)){
        row.clear();
        countLine++;
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (line.empty() || line[0] == '#')
            continue;
        std::stringstream s(line);
        while (getline(s,words,',')){
            row.push_back(words);
        }
        if (row.size() == 0)
            continue;
        try{
            if (row.at(0) == "ROUTER"){
                if (row.size() == 2)
                        routers_.emplace_back(Router_{row.at(1)});
                else
                    throw std::invalid_argument("Has wrong number of fields for ROUTER");
            }
            else
                if (row.at(0) == "LINK"){
                    if (row.size() == 7)
                      links_.emplace_back(Link_{row.at(1),row.at(2),row.at(3),row.at(4),std::stoll(row.at(5)),row.at(6) == "UP"});
                    else
                        throw std::invalid_argument("Has wrong number of fields for LINK");
                }
                else
                    if (row.at(0) == "STATIC"){
                        if (row.size() == 7)
                            routes_.emplace_back(StaticRoute_{row.at(1),row.at(2),row.at(3),row.at(4),std::stoi(row.at(5)),std::stoll(row.at(6))});
                        else
                            throw std::invalid_argument("Has wrong number of fields for STATIC");
                    }
                    else
                        throw std::invalid_argument("Has unknown type: " + row.at(0));
        }
        catch (std::exception &msg){
            logger.logWarning("Line " + std::to_string(countLine) + " format error: " + msg.what());
        }
    }

    for (const auto& router : routers_){
        try{
            addRouter(routers,router);
        }
        catch(std::exception &msg){
            logger.logWarning(msg.what());
        }
    }
    for (const auto& link : links_){
        try{
            addLink(routers,link);
        }
        catch(std::exception &msg){
            logger.logWarning(msg.what());
        }
    }
    for (const auto& route : routes_){
        try{
            addStaticRoute(routers,route);
        }
        catch(std::exception &msg){
            logger.logWarning(msg.what());
        }
    }
}

void CsvTopologyStorage::saveTopology(const std::string& csvPath, const std::unordered_map<std::string,std::shared_ptr<Router>>& routers,const TextLogger& logger){
    std::ofstream fOut(csvPath, std::ios::out | std::ios::trunc);
    if (!fOut.is_open())
        throw std::runtime_error("Cannot open csv file");
    logger.logInfo("Saving Topology to CSV file");
    for (const auto& [IpAddress,router_] : routers)
        fOut << "ROUTER," << IpAddress << '\n';
    std::unordered_set<std::string>recordedIPConnects;
    for (const auto& [ipA, router] : routers){
        const auto& ifaces = router->getInterfaces();
        for (const auto& [ifaceIdA, ifaceA] : ifaces){
            const std::string& ipB = ifaceA->getLinkIP();
            const std::string& ifaceIdB = ifaceA->getLinkInterface();
            std::string linkKey = (ipA < ipB) ?
            (ipA + ":" + ifaceIdA + "<->" + ipB + ":" + ifaceIdB)
            : (ipB + ":" + ifaceIdB + "<->" + ipA + ":" + ifaceIdA);
            if (!recordedIPConnects.contains(linkKey)){
                recordedIPConnects.insert(linkKey);
                fOut << "LINK," << ipA << ',' << ifaceIdA << ',' << ipB << ',' << ifaceIdB << ',' << ifaceA->getCost() << ',' << (ifaceA->getStatus() ? "UP" : "DOWN") << '\n';
            }
        }
    }
    for (const auto& [ipA, router] : routers){
        const auto& table = router->getStaticRoutingTable();
        for (const auto& [ipB,route] : table.getRoutes())
            fOut << "STATIC," << ipA << ',' << route.getDestIP() << ',' << route.getNextIP() << ',' << route.getInterfaceId() << ',' << route.getHopCount() << ',' << route.getCost() << '\n';
    }
}
