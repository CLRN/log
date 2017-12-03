#pragma once

#include "log.h"

#include <boost/shared_ptr.hpp>

namespace logging
{

class Std : public ILog
{
public:
    Std(ILog::Level::Value level = ILog::Level::Info, const char* file = nullptr);

    virtual bool IsEnabled(const char* module, Level::Value level) const override;
    virtual boost::filesystem::path GetLogFolder(const char* module) const override;
    virtual void Write(const char* module, ILog::Level::Value level, const std::string& text, const char* file, unsigned line, const char* function) override;
    virtual void SetLevel(Level::Value level) override;
    virtual void SetLevels(const boost::property_tree::ptree& settings) override;

private:
    const size_t m_FileMaxSize{ 100000000 };
    const std::string m_FileName;
    const std::string m_BackupFileName;
    boost::shared_ptr<std::ostream> m_Stream;
    ILog::Level::Value m_Level;
};

} // namespace logging