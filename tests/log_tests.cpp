#include "log/log.h"

#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Return;
using ::testing::Exactly;
using ::testing::AtLeast;
using ::testing::Invoke;
using ::testing::Expectation;

class MockedLog : public ILog
{
public:
    MOCK_CONST_METHOD2(IsEnabled, bool(const char* module, Level::Value level));
    
    MOCK_METHOD6(Write, void(const char* module, ILog::Level::Value level, const std::string& text, const char* file, unsigned line, const char* function));

    virtual boost::filesystem::path GetLogFolder(const char* module) const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

};

SET_LOGGING_MODULE("tests");


TEST(Logging, TextFormatter)
{
    {
        const auto text = TXT("text");
        EXPECT_EQ(text, "text");
    }
    {
        const auto text = TXT("text: %s", std::vector<int>{1, 2, 3});
        EXPECT_EQ(text, "text: 1,2,3");
    }
    {
        const auto text = TXT("text: %s", std::map<std::string, int>{ {"1", 1}, {"2", 2} });
        EXPECT_EQ(text, "text: 1:1,2:2");
    }
}

TEST(Logging, Levels)
{
    auto* log = new MockedLog();

    EXPECT_CALL(*log, IsEnabled(CURRENT_MODULE_ID, ILog::Level::Trace))
            .Times(Exactly(1))
            .WillOnce(Return(true));
    EXPECT_CALL(*log, IsEnabled(CURRENT_MODULE_ID, ILog::Level::Debug))
            .Times(Exactly(1))
            .WillOnce(Return(true));
    EXPECT_CALL(*log, IsEnabled(CURRENT_MODULE_ID, ILog::Level::Warning))
            .Times(Exactly(1))
            .WillOnce(Return(true));
    EXPECT_CALL(*log, IsEnabled(CURRENT_MODULE_ID, ILog::Level::Error))
            .Times(Exactly(1))
            .WillOnce(Return(true));
    EXPECT_CALL(*log, IsEnabled(CURRENT_MODULE_ID, ILog::Level::Info))
            .Times(Exactly(1))
            .WillOnce(Return(true));

    EXPECT_CALL(*log, Write(CURRENT_MODULE_ID, ILog::Level::Trace, "trace", _, _, _))
            .Times(Exactly(1));
    EXPECT_CALL(*log, Write(CURRENT_MODULE_ID, ILog::Level::Debug, "debug", _, _, _))
            .Times(Exactly(1));
    EXPECT_CALL(*log, Write(CURRENT_MODULE_ID, ILog::Level::Warning, "warning", _, _, _))
            .Times(Exactly(1));
    EXPECT_CALL(*log, Write(CURRENT_MODULE_ID, ILog::Level::Error, "error", _, _, _))
            .Times(Exactly(1));
    EXPECT_CALL(*log, Write(CURRENT_MODULE_ID, ILog::Level::Info, "info", _, _, _))
            .Times(Exactly(1));

    logging::CurrentLog::Set(log);

    LOG_TRACE("trace");
    LOG_DEBUG("debug");
    LOG_WARNING("warning");
    LOG_ERROR("error");
    LOG_INFO("info");

    logging::CurrentLog::Set(nullptr);
}

TEST(Logging, WrongFormat)
{
    auto* log = new MockedLog();

    EXPECT_CALL(*log, IsEnabled(CURRENT_MODULE_ID, ILog::Level::Error))
        .Times(Exactly(1))
        .WillOnce(Return(true));
    EXPECT_CALL(*log, Write(CURRENT_MODULE_ID, ILog::Level::Error, "test%1%2", _, _, _))
        .Times(Exactly(1));

    logging::CurrentLog::Set(log);

    LOG_ERROR("test%1%2");

    logging::CurrentLog::Set(nullptr);
}


TEST(Logging, CorrectFormat)
{
    auto* log = new MockedLog();

    EXPECT_CALL(*log, IsEnabled(CURRENT_MODULE_ID, ILog::Level::Info))
        .Times(Exactly(1))
        .WillOnce(Return(true));
    EXPECT_CALL(*log, Write(CURRENT_MODULE_ID, ILog::Level::Info, "text", _, _, _))
        .Times(Exactly(1));

    logging::CurrentLog::Set(log);

    LOG_INFO("%s", "text");
    logging::CurrentLog::Set(nullptr);
}

GTEST_API_ int main(int argc, char **argv) {
    std::cout << "Running main() from gtest_main.cc\n";

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}