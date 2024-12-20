#include <iostream>
#include <memory>
#include <string>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FAULT
};

class Logger {
public:
    void log(LogLevel level, const std::string& message) {
        switch (level) {
            case LogLevel::DEBUG:
                std::cout << "\033[34m[DEBUG] " << message << "\033[0m" << std::endl;
                break;
            case LogLevel::INFO:
                std::cout << "\033[32m[INFO] " << message << "\033[0m" << std::endl;
                break;
            case LogLevel::WARNING:
                std::cout << "\033[33m[WARNING] " << message << "\033[0m" << std::endl;
                break;
            case LogLevel::ERROR:
                std::cout << "\033[31m[ERROR] " << message << "\033[0m" << std::endl;
                break;
            case LogLevel::FAULT:
                std::cout << "\033[41m[FAULT] " << message << "\033[0m" << std::endl;
                break;
        }
    }
};

class LoggerFactory {
public:
    static std::shared_ptr<Logger> createLogger() {
        return std::make_shared<Logger>();
    }
};

auto logger = LoggerFactory::createLogger();
