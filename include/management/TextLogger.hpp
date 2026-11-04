#pragma once
#include <string>
#include <string_view>

class TextLogger {
private:
    std::string logFilePath;
    void logger(std::string_view logCode, std::string_view msg) const;
public:
    TextLogger (std::string = "./data/network_events.txt");
    void logInfo(std::string_view) const;
    void logWarning(std::string_view) const;
    void logError(std::string_view) const;
};
