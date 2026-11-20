#include "management/Administrator.hpp"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <limits>

namespace Ask{
    int askInt(const std::string& prompt, int min, int max) {
        std::string value;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, value);
            try {
                int intValue = std::stoi(value);
                if (intValue >= min && intValue <= max) {
                    return intValue;
                }
                else {
                    std::cout << "Invalid input. Please enter a valid number between " << min << " and " << max << ".\n";
                }
            }
            catch (const std::exception& msg) {
                std::cout << "Invalid input. Please enter a valid number between " << min << " and " << max << ".\n";
            }
        }
    }

    long long askLongLong(const std::string& prompt, long long min, long long max) {
        std::string value;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, value);
            try {
                long long longValue = std::stoll(value);
                if (longValue >= min && longValue <= max) {
                    return longValue;
                }
                else {
                    std::cout << "Invalid input. Please enter a valid number between " << min << " and " << max << ".\n";
                }
            }
            catch (const std::exception& msg) {
                std::cout << "Invalid input. Please enter a valid number between " << min << " and " << max << ".\n";
            }
        }
    }

    std::string askString(const std::string& prompt) {
        std::string value;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, value);
            if (!value.empty()) {
                return value;
            }
            else {
                std::cout << "Invalid input. Please enter a non-empty string.\n";
            }
        }
    }
}

void menu(){
    std::cout << "\n==================== ROUTING SIMULATION ====================\n"
                 "1. Print all routing tables\n"
                 "2. Send packet\n"
                 "3. Turn on/off interface\n"
                 "4. Add static route\n"
                 "5. Remove static route\n"
                 "6. Import network topology from CSV\n"
                 "7. Export network topology to CSV\n"
                 "8. Converge network\n"
                 "9. Change global routing strategy\n"
                 "0. Exit\n";
}

int main() {
    Administrator admin;
    try{
        admin.importNetworkFromCsv("./data/network_topology.csv");
        admin.changeGlobalStrategy("cost");
        std::cout << "Network topology imported successfully.\n";
    } catch (const std::exception& msg) {
        std::cerr << "Error importing network topology: " << msg.what() << std::endl;
        return 1;
    }

    while (true){
        try{
            menu();
            int choice = Ask::askInt("Enter your choice: ", 0, 9);
            if (choice == 0) {
                std::cout << "Exiting the program.\n";
                break;
            }
            switch (choice){
                case 1:
                    {
                        admin.printAllRoutingTables();
                        break;
                    }
                case 2:
                    {
                        auto src = Ask::askString("Enter source router: ");
                        auto dest = Ask::askString("Enter destination router: ");
                        auto payload = Ask::askString("Enter packet payload: ");
                        auto ttl = Ask::askInt("Enter packet TTL: ", 1, std::numeric_limits<int>::max());
                        admin.sendPacket(src, dest, payload, ttl);
                        break;
                    }
                case 3:
                    {
                        auto routerName = Ask::askString("Enter router name: ");
                        auto interfaceName = Ask::askString("Enter interface name: ");
                        bool status = Ask::askInt("Enter status (1 for UP, 0 for DOWN): ", 0, 1);
                        admin.setLinkStatus(routerName, interfaceName, status);
                        break;
                    }
                case 4:
                    {
                        auto routerName = Ask::askString("Enter router name: ");
                        auto destIP = Ask::askString("Enter destination network: ");
                        auto nextIP = Ask::askString("Enter next hop: ");
                        auto interfaceId = Ask::askString("Enter interface ID: ");
                        int hopCount = Ask::askInt("Enter hop count: ", 0, std::numeric_limits<int>::max());
                        long long cost = Ask::askLongLong("Enter cost: ", 1, std::numeric_limits<long long>::max());
                        Route route(destIP, nextIP, interfaceId, hopCount, cost);
                        admin.addStaticRoute(routerName, route);
                        break;
                    }
                case 5:
                    {
                        auto routerName = Ask::askString("Enter router name: ");
                        auto destIP = Ask::askString("Enter destination network to remove: ");
                        admin.removeStaticRoute(routerName, destIP);
                        break;
                    }
                case 6:
                    {
                        auto filePath = Ask::askString("Enter CSV file path to import: ");
                        admin.importNetworkFromCsv("./data/" + filePath);
                        break;
                    }
                case 7:
                    {
                        auto filePath = Ask::askString("Enter CSV file path to export: ");
                        admin.exportNetworkToCsv("./data/" + filePath);
                        break;
                    }
                case 8:
                    {
                        admin.convergeNetwork();
                        break;
                    }
                case 9:
                    {
                        auto strategy = Ask::askString("Enter new global routing strategy (bfs/dijkstra/static): ");
                        admin.changeGlobalStrategy(strategy);
                        break;
                    }
                default:
                    {
                        std::cout << "Invalid choice. Please try again.\n";
                        break;
                    }
            }
        }
        catch (const std::exception& msg) {
            std::cerr << "Error occurred: " << msg.what() << std::endl;
        }
    }
    return 0;
}
