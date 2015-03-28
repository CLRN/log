#include "std_log.h"

#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace logging
{

bool Std::IsEnabled(const char* /*module*/, Level::Value /*level*/) const
{
    return true;
}

boost::filesystem::path Std::GetLogFolder(const char* module) const
{
    const auto path = boost::filesystem::current_path() / "log" / module;
    if (!boost::filesystem::exists(path))
        boost::filesystem::create_directories(path);
    return path;
}

void Std::Write(const char* module, ILog::Level::Value level, const std::string& text, const char* file, unsigned line, const char* function)
{
    std::clog
        << "[" << ILog::Level::to_string(level) << "]"
        << "[" << boost::posix_time::microsec_clock::local_time() << "]"
        << "[" << module << "]"
        << "[" << text << "]"
        << std::endl;
}

} // namespace logging
