#pragma once

#include <memory>
#include <string>

class Interface {
public:
    Interface(std::string id_, std::string linkIP_, std::string linkInterface_, bool isUp_, long long linkCost_);
    const std::string& getId() const noexcept;
    const std::string& getLinkIP() const noexcept;
    const std::string& getLinkInterface() const noexcept;
    bool getStatus() const noexcept;
    void setStatus(bool status) noexcept;
    long long getCost() const noexcept;
    void setCost(long long cost);
    void connectPeer(std::shared_ptr<Interface> peer) noexcept;
    std::shared_ptr<Interface> getPeer() const noexcept;
private:
    std::string id{};
    std::string linkIP{};
    std::string linkInterface{};
    bool isUp{false};
    long long linkCost{0};
    std::weak_ptr<Interface> peerInterface{};
};