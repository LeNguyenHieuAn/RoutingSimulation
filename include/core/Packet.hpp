#pragma once
#include <string>

class Packet {
public:
    Packet(std::string srcIP_, std::string destIP_, std::string payload_, int ttl_ = 64);
    const std::string& getSrcIP() const noexcept;
    const std::string& getDestIP() const noexcept;
    const std::string& getPayload() const noexcept;
    int getTTL() const noexcept;
    void decrementTTL() noexcept;
    bool isExpired() const noexcept;
private:
    std::string srcIP{};
    std::string destIP{};
    std::string payload{};
    int ttl{64};
};