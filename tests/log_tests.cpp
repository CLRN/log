#include "log/log.h"

#include <iostream>

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

    LOG_INFO("%s") % "text";
    logging::CurrentLog::Set(nullptr);
}

GTEST_API_ int main(int argc, char **argv) {
    std::cout << "Running main() from gtest_main.cc\n";

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}