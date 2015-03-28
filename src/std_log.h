#pragma once

#include "log.h"

namespace logging
{

class Std : public ILog
{
public:


    virtual bool IsEnabled(const char* module, Level::Value level) const;

    virtual boost::filesystem::path GetLogFolder(const char* module) const;

    virtual void Write(const char* module, ILog::Level::Value level, const std::string& text, const char* file, unsigned line, const char* function);

};

} // namespace logging