#pragma once

#include "log.h"

namespace logging
{

class Log4cplus : public ILog
{
public:
    Log4cplus(const std::string& src);
    Log4cplus(Level::Value lvl, const char* file = nullptr);

    virtual bool IsEnabled(const char* module, Level::Value level) const override;
    virtual boost::filesystem::path GetLogFolder(const char* module) const override;
    virtual void Write(const char* module, ILog::Level::Value level, const std::string& text, const char* file, unsigned line, const char* function) override;
    virtual void SetLevel(Level::Value level) override;
    virtual void SetLevels(const boost::property_tree::ptree& settings) override;

private:
    bool m_IsOpened;
};

} // namespace logging