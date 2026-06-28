#pragma once
#include "src/drivers/logger/ILogger.hpp"
#include <string>

struct MockILogger : public drivers::logger::ILogger
{
    drivers::logger::LOGLEVEL lastLevel = drivers::logger::LOGLEVEL_OFF;
    std::string lastMsg;
    int callCount = 0;

    void init(drivers::logger::LOGLEVEL level, bool blockTillSerialConnected = false) override {
        lastLevel = level; callCount++;
    }
    void setLoglevel(drivers::logger::LOGLEVEL level) override { lastLevel = level; }

    void fatal(const char* msg, ...) override { recordCall(drivers::logger::LOGLEVEL_FATAL, msg); }
    void fatal(const char* msg, va_list args) override { recordCall(drivers::logger::LOGLEVEL_FATAL, msg); }
    void fatal(std::string msg) override { lastLevel = drivers::logger::LOGLEVEL_FATAL; lastMsg = msg; callCount++; }
    void error(const char* msg, ...) override { recordCall(drivers::logger::LOGLEVEL_ERROR, msg); }
    void error(const char* msg, va_list args) override { recordCall(drivers::logger::LOGLEVEL_ERROR, msg); }
    void error(std::string msg) override { lastLevel = drivers::logger::LOGLEVEL_ERROR; lastMsg = msg; callCount++; }
    void warning(const char* msg, ...) override { recordCall(drivers::logger::LOGLEVEL_WARNING, msg); }
    void warning(const char* msg, va_list args) override { recordCall(drivers::logger::LOGLEVEL_WARNING, msg); }
    void warning(std::string msg) override { lastLevel = drivers::logger::LOGLEVEL_WARNING; lastMsg = msg; callCount++; }
    void info(const char* msg, ...) override { recordCall(drivers::logger::LOGLEVEL_INFO, msg); }
    void info(const char* msg, va_list args) override { recordCall(drivers::logger::LOGLEVEL_INFO, msg); }
    void info(std::string msg) override { lastLevel = drivers::logger::LOGLEVEL_INFO; lastMsg = msg; callCount++; }
    void trace(const char* msg, ...) override { recordCall(drivers::logger::LOGLEVEL_TRACE, msg); }
    void trace(const char* msg, va_list args) override { recordCall(drivers::logger::LOGLEVEL_TRACE, msg); }
    void trace(std::string msg) override { lastLevel = drivers::logger::LOGLEVEL_TRACE; lastMsg = msg; callCount++; }

private:
    void recordCall(drivers::logger::LOGLEVEL level, const char* msg) {
        lastLevel = level;
        lastMsg = msg ? msg : "";
        callCount++;
    }
};
