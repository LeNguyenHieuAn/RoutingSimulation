#include "Interface.hpp"
#include <stdexcept>
#include <utility>

Interface::Interface(std::string id_, std::string linkIP_, std::string linkInterface_, bool isUp_, long long linkCost_)
    : id{std::move(id_)},
      linkIP{std::move(linkIP_)},
      linkInterface{std::move(linkInterface_)},
      isUp{isUp_},
      linkCost{linkCost_} {
    
    // Kiểm tra tính bất biến (Invariant): ID không được rỗng và chi phí không được âm
    if (this->id.empty()) {
        throw std::invalid_argument("Interface Error: Interface ID cannot be empty.");
    }
    if (this->linkCost < 0) {
        throw std::invalid_argument("Interface Error: Link cost cannot be negative.");
    }
}

const std::string& Interface::getId() const noexcept {
    return id;
}

const std::string& Interface::getLinkIP() const noexcept {
    return linkIP;
}

const std::string& Interface::getLinkInterface() const noexcept {
    return linkInterface;
}

bool Interface::getStatus() const noexcept {
    return isUp;
}

void Interface::setStatus(bool status) noexcept {
    isUp = status;
}

long long Interface::getCost() const noexcept {
    return linkCost;
}

void Interface::setCost(long long cost) {
    if (cost < 0) {
        throw std::invalid_argument("Interface Error: Link cost cannot be negative.");
    }
    linkCost = cost;
}

void Interface::connectPeer(std::shared_ptr<Interface> peer) noexcept {
    peerInterface = peer;
}

std::shared_ptr<Interface> Interface::getPeer() const noexcept {
    // peerInterface.lock() trả về shared_ptr nếu đối tượng còn tồn tại, hoặc nullptr nếu đã bị hủy
    return peerInterface.lock();
}