#pragma once
#include <stdexcept>
#include <string>

class NoRouteException: public std::runtime_error{
public:
    explicit NoRouteException(const std::string& destIP)
        : std::runtime_error("No route to " + destIP) {};
};

class InterfaceDownException: public std::runtime_error{
public:
    explicit InterfaceDownException(const std::string& routerIP, const std::string& interfaceId)
        : std::runtime_error("Interface " + interfaceId + " of router " + routerIP + " is unavailable") {};
};
