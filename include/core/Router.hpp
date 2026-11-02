#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Interface.hpp"
#include "Packet.hpp"
#include "RoutingTable.hpp"
#include "RoutingStrategy.hpp"

class Router {
public:
    explicit Router(std::string ip, std::unique_ptr<RoutingStrategy> initialStrategy = nullptr);
    const std::string& getIP() const;
    void addInterface(std::shared_ptr<Interface> iface);
    std::shared_ptr<Interface> getInterface(const std::string& ifaceId) const;
    const std::unordered_map<std::string, std::shared_ptr<Interface>>& getInterfaces() const;
    void setInterfaceStatus(const std::string& ifaceId, bool status);
    RoutingTable& getStaticRoutingTable() noexcept;
    const RoutingTable& getStaticRoutingTable() const noexcept;
    RoutingTable& getDynamicRoutingTable() noexcept;
    const RoutingTable& getDynamicRoutingTable() const noexcept;
    void setStrategy(std::unique_ptr<RoutingStrategy> newStrategy);
    void updateRoutes(const std::unordered_map<std::string, std::shared_ptr<Router>>& topology);
    std::string forwardPacket(Packet& packet) const;
private:
    std::string ipAddress;
    std::unordered_map<std::string, std::shared_ptr<Interface>> interfaces;
    RoutingTable staticTable;
    RoutingTable dynamicTable;
    std::unique_ptr<RoutingStrategy> strategy;
};