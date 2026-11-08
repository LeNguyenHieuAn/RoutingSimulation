#include "TextLogger.hpp"
#include <fstream>
#include <chrono>
#include <format>
#include <stdexcept>

void TextLogger::logger(std::string_view logCode, std::string_view msg) const{
    std::ofstream fOut(logFilePath,std::ios::app);
    if (fOut.is_open() == false)
        throw std::runtime_error("Cannot open or create log file");
    const auto now = std::chrono::system_clock::now();
    const auto now_sec = std::chrono::floor<std::chrono::seconds>(now);
    const auto local_time = std::chrono::current_zone()->to_local(now_sec);
    std::string formatted = std::format("{:%d/%m/%Y %H:%M:%S}", local_time);
    fOut << "[" << formatted << "] (" << logCode << ") " << msg <<'\n';
}

TextLogger::TextLogger (std::string filePath_)
    : logFilePath(std::move(filePath_)) {}

void TextLogger::logInfo(std::string_view msg) const{
    logger("INFO",msg);
}

void TextLogger::logWarning(std::string_view msg) const{
    logger("WARNING",msg);
}

void TextLogger::logError(std::string_view msg) const{
    logger("ERROR",msg);
}
