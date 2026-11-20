#pragma once
#include <iostream>
#include <string>

class Route {
public:
    Route() = default;
    Route(std::string destIP_, std::string nextIP_, std::string interfaceId_, int hopCount_, long long cost_);
    const std::string& getDestIP() const noexcept;
    const std::string& getNextIP() const noexcept;
    const std::string& getInterfaceId() const noexcept;
    int getHopCount() const noexcept;
    long long getCost() const noexcept;
    bool operator==(const Route& other) const noexcept;
    bool operator<(const Route& other) const noexcept;
    friend std::ostream& operator<<(std::ostream& os, const Route& route);
private:
    std::string destIP{};
    std::string nextIP{};
    std::string interfaceId{};
    int hopCount{0};
    long long cost{0};
};