#include "Router.hpp"
#include "Exceptions.hpp"
#include <stdexcept>
#include <algorithm>        

Router::Router(std::string ip, std::unique_ptr<RoutingStrategy> initialStrategy)
    : ipAddress{std::move(ip)},
      strategy{std::move(initialStrategy)} {
    if (this->ipAddress.empty()) {
        throw std::invalid_argument("Router Error: Router IP address cannot be empty.");
    }
}
const std::string& Router::getIP() const noexcept {
    return ipAddress; 
}
    
void Router::addInterface(std::shared_ptr<Interface> iface) {
    if (!iface) {
        throw std::invalid_argument("Router Error: Cannot add null interface to Router " + ipAddress);
    }
    if (iface->getId().empty()) {
        throw std::invalid_argument("Router Error: Interface ID cannot be empty.");
    }
    interfaces.insert_or_assign(iface->getId(), std::move(iface));
}

std::shared_ptr<Interface> Router::getInterface(const std::string& ifaceId) const {
    const auto it = interfaces.find(ifaceId);
    return (it != interfaces.end()) ? it->second : nullptr;
}

const std::unordered_map<std::string, std::shared_ptr<Interface>>& Router::getInterfaces() const noexcept {
    return interfaces;
}

void Router::setInterfaceStatus(const std::string& ifaceId, bool status) {
    auto iface = getInterface(ifaceId);
    if (!iface) {
        throw std::invalid_argument("Router Error: Interface [" + ifaceId + "] does not exist on Router [" + ipAddress + "].");
    }
    iface->setStatus(status);
    if (auto peer = iface->getPeer()) {
        peer->setStatus(status);
    }
}

RoutingTable& Router::getDynamicRoutingTable() noexcept {
    return dynamicTable;
}

const RoutingTable& Router::getDynamicRoutingTable() const noexcept {
    return dynamicTable;
}

RoutingTable& Router::getStaticRoutingTable() noexcept {
    return staticTable;
}

const RoutingTable& Router::getStaticRoutingTable() const noexcept {
    return staticTable;
}

void Router::setStrategy(std::unique_ptr<RoutingStrategy> newStrategy) noexcept {
    strategy = std::move(newStrategy);
}

void Router::updateRoutes(const std::unordered_map<std::string, std::shared_ptr<Router>>& topology) {
    if (strategy) {
        strategy->calculateRoutes(*this, topology);
    }
}

std::string Router::forwardPacket(Packet& packet) const {
    // 1. Kiểm tra nếu gói tin đã đến đích
    if (packet.getDestIP() == ipAddress) {
        return ipAddress;
    }

    // 2. Giảm TTL và kiểm tra thời hạn gói tin
    packet.decrementTTL();
    if (packet.isExpired()) {
        throw std::runtime_error("Router Error: Packet dropped due to expired TTL at Router " + ipAddress);
    }

    // 3. Tra cứu tuyến đường: Ưu tiên Static Table trước, sau đó đến Dynamic Table
    Route selectedRoute;
    if (staticTable.hasRoute(packet.getDestIP())) {
        selectedRoute = staticTable.findRoute(packet.getDestIP());
    } else {
        // Nếu không có trong cả static và dynamic table, hàm findRoute sẽ ném NoRouteException
        selectedRoute = dynamicTable.findRoute(packet.getDestIP());
    }

    // 4. Kiểm tra trạng thái cổng giao tiếp xuất (Outgoing Interface)
    auto outIface = getInterface(selectedRoute.getInterfaceId());
    if (!outIface || !outIface->getStatus()) {
        throw InterfaceDownException(ipAddress, selectedRoute.getInterfaceId());
    }

    // 5. Xác định trạm kế tiếp: nếu nextIP rỗng (Direct Link), trạm kế tiếp chính là IP đích
    if (selectedRoute.getNextIP().empty()) {
        return packet.getDestIP();
    }

    return selectedRoute.getNextIP();
}