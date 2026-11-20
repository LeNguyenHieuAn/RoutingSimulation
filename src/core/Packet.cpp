#include "Packet.hpp"
#include <utility>
#include <stdexcept>

Packet::Packet(std::string srcIP_, std::string destIP_, std::string payload_, int ttl_)
    : srcIP{std::move(srcIP_)},
      destIP{std::move(destIP_)},
      payload{std::move(payload_)},
      ttl{ttl_} {
    if (this->srcIP.empty()) {
        throw std::invalid_argument("Packet Error: Source IP cannot be empty.");
    }
    if (this->destIP.empty()) {
        throw std::invalid_argument("Packet Error: Destination IP cannot be empty.");
    }
    if (this->ttl <= 0) {
        throw std::invalid_argument("Packet Error: Initial TTL must be strictly positive.");
    }
}

const std::string& Packet::getSrcIP() const noexcept {
    return srcIP; 
}

const std::string& Packet::getDestIP() const noexcept {
    return destIP; 
}

const std::string& Packet::getPayload() const noexcept { 
    return payload; 
}

int Packet::getTTL() const noexcept { 
    return ttl; 
}

void Packet::decrementTTL() noexcept {
    if (ttl > 0){
        --ttl;
    } 
}

bool Packet::isExpired() const noexcept {
    return ttl <= 0;
}