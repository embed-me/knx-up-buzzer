#include <gtest/gtest.h>
#include "src/drivers/logger/Logger.hpp"
#include "src/drivers/logger/DummyLogger.hpp"
#include "test/mocks/mock_logger.hpp"
#include "src/drivers/logger/Logger.cpp"

using namespace drivers::logger;

class LoggerTest : public ::testing::Test {
protected:
    void TearDown() override {
        Logger::setLogger(nullptr);
    }
};

TEST_F(LoggerTest, GetLoggerWithoutSetLoggerReturnsDummyLogger) {
    auto logger = Logger::getLogger();
    EXPECT_NE(logger, nullptr);
}

TEST_F(LoggerTest, SetLoggerThenGetLoggerReturnsCustomLogger) {
    auto custom = std::make_shared<MockILogger>();
    Logger::setLogger(custom);
    EXPECT_EQ(Logger::getLogger(), custom);
}

TEST(DummyLoggerTest, InitNoCrash) {
    DummyLogger dl;
    EXPECT_NO_THROW(dl.init(LOGLEVEL_INFO));
}

TEST(DummyLoggerTest, FatalNoCrash) {
    DummyLogger dl;
    EXPECT_NO_THROW(dl.fatal("test"));
}

TEST(DummyLoggerTest, InfoNoCrash) {
    DummyLogger dl;
    EXPECT_NO_THROW(dl.info("test"));
}

TEST(DummyLoggerTest, AllLevelsNoCrash) {
    DummyLogger dl;
    EXPECT_NO_THROW(dl.fatal("fatal"));
    EXPECT_NO_THROW(dl.error("error"));
    EXPECT_NO_THROW(dl.warning("warning"));
    EXPECT_NO_THROW(dl.info("info"));
    EXPECT_NO_THROW(dl.trace("trace"));
}

TEST_F(LoggerTest, LogTraceDispatchesToString) {
    auto mock = std::make_shared<MockILogger>();
    Logger::setLogger(mock);
    logTrace(std::string("hello"));
    EXPECT_EQ(mock->lastLevel, LOGLEVEL_TRACE);
    EXPECT_EQ(mock->lastMsg, "hello");
}

TEST_F(LoggerTest, LogInfoDispatchesFormatString) {
    auto mock = std::make_shared<MockILogger>();
    Logger::setLogger(mock);
    logInfo("value=%d", 42);
    EXPECT_EQ(mock->lastLevel, LOGLEVEL_INFO);
}

TEST_F(LoggerTest, LogWarningErrorFatalDispatch) {
    auto mock = std::make_shared<MockILogger>();
    Logger::setLogger(mock);

    logWarning(std::string("warn"));
    EXPECT_EQ(mock->lastLevel, LOGLEVEL_WARNING);
    EXPECT_EQ(mock->lastMsg, "warn");

    logError(std::string("err"));
    EXPECT_EQ(mock->lastLevel, LOGLEVEL_ERROR);
    EXPECT_EQ(mock->lastMsg, "err");

    logFatal(std::string("fat"));
    EXPECT_EQ(mock->lastLevel, LOGLEVEL_FATAL);
    EXPECT_EQ(mock->lastMsg, "fat");
}
