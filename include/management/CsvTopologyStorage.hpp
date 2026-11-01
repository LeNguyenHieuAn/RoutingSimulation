#pragma once
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include "TextLogger.hpp"
#include "Router.hpp"

class CsvTopologyStorage{
public:
    static void loadTopology(const std::string&, std::unordered_map<std::string,std::shared_ptr<Router>>&, const TextLogger&);
    static void saveTopology(const std::string&, const std::unordered_map<std::string,std::shared_ptr<Router>>&, const TextLogger&);
};
