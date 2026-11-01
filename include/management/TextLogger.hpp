#pragma once
#include <string>
#include <string_view>

class TextLogger {
private:
    std::string logFilePath;
public:
    TextLogger (std::string = "network_events.txt");
    void logInfo(std::string_view) const;
    void logWarning(std::string_view) const;
    void logError(std::string_view) const;
};
