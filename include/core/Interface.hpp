#pragma once

#include <memory>
#include <string>

class Interface {
public:
   
    Interface(std::string id, std::string linkIP, std::string linkInterface, bool isUp, long long linkCost);

    const std::string& getId() const;
    const std::string& getLinkIP() const;
    const std::string& getLinkInterface() const;

    bool getStatus() const noexcept;
    void setStatus(bool status) noexcept;

    long long getCost() const noexcept;
    void setCost(long long cost);

    void connectPeer(std::shared_ptr<Interface> peer);

private:
    std::string id;
    std::string linkIP;
    std::string linkInterface;
    bool isUp;
    long long linkCost;
    std::weak_ptr<Interface> peerInterface;
};