#pragma once
#include <iostream>
#include <string>

class Route {
public:
    Route() = default;
    Route(std::string destIP, std::string nextIP, std::string interfaceId, int hopCount, long long cost);
    const std::string& getDestIP() const;
    const std::string& getNextIP() const;
    const std::string& getInterfaceId() const;
    int getHopCount() const;
    long long getCost() const;
    bool operator==(const Route& other) const;
    bool operator<(const Route& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Route& route);
private:
    std::string destIP;
    std::string nextIP;
    std::string interfaceId;
    int hopCount;
    long long cost;
};