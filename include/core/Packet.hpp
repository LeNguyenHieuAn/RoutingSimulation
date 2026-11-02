#pragma once
#include <string>

class Packet {
public:
    Packet(std::string srcIP, std::string destIP, std::string payload, int ttl = 64);
    const std::string& getSrcIP() const;
    const std::string& getDestIP() const;
    const std::string& getPayload() const;
    int getTTL() const noexcept;
    void decrementTTL() noexcept;
    bool isExpired() const noexcept;
private:
    std::string srcIP;
    std::string destIP;
    std::string payload;
    int ttl{64};
};