#include "log/std_log.h"

#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>

#include <unordered_map>

namespace logging
{

ILog::Level::Value GetLevel(const std::string& v)
{
    const static std::unordered_map<std::string, ILog::Level::Value> levels{
        { "ERROR", ILog::Level::Error },
        { "WARNING", ILog::Level::Warning },
        { "INFO", ILog::Level::Info },
        { "DEBUG", ILog::Level::Debug },
        { "TRACE", ILog::Level::Trace }
    };
    return levels.at(v);
}

Std::Std(ILog::Level::Value level, const char* filename) 
    : m_FileName(filename ? filename : "")
    , m_BackupFileName(!m_FileName.empty() ? m_FileName + ".1" : "")
    , m_Level(level)
{
    if (!m_FileName.empty())
    {
        const auto folder = boost::filesystem::path(m_FileName).branch_path();
        if (!folder.empty() && !boost::filesystem::exists(folder))
            boost::filesystem::create_directories(folder);
        m_Stream = boost::make_shared<std::ofstream>(m_FileName, std::ios::app);
    }
    else
    {
        m_Stream = boost::shared_ptr<std::ostream>(&std::cout, [](const std::ostream*){});
    }

    *m_Stream << "[" << boost::posix_time::microsec_clock::local_time() << "]" << " Started. " << std::endl;
}

bool Std::IsEnabled(const char* /*module*/, Level::Value level) const
{
    return m_Level >= level;
}

boost::filesystem::path Std::GetLogFolder(const char* /*module*/) const
{
    const auto folder = boost::filesystem::path(m_FileName).branch_path();
    if (!boost::filesystem::exists(folder))
        boost::filesystem::create_directories(folder);
    return folder;
}

void Std::Write(const char* module, ILog::Level::Value level, const std::string& text, const char* file, unsigned line, const char* function)
{
    static boost::mutex mutex;
    boost::unique_lock<boost::mutex> lock(mutex);

    *m_Stream
        << "[" << ILog::Level::to_string(level) << "]"
        << "[" << boost::posix_time::microsec_clock::local_time() << "]"
        << "[" << module << "] "
        << "[" << boost::this_thread::get_id() << "] "
        << text
        << "/* " 
        << function
        << " */"
        << std::endl;

    if (!m_FileName.empty() && m_Stream->tellp() > m_FileMaxSize)
    {
        auto fstream = static_cast<std::ofstream*>(m_Stream.get());
        fstream->close();

        // Remove old backup.
        if (boost::filesystem::exists(m_BackupFileName))
            boost::filesystem::remove(m_BackupFileName);
        
        // Backup current log.
        boost::filesystem::rename(m_FileName, m_BackupFileName);
        
        // Start new log.
        m_Stream = boost::make_shared<std::ofstream>(m_FileName);
    }
}

void Std::SetLevel(Level::Value level)
{
    m_Level = level;
}

void Std::SetLevels(const boost::property_tree::ptree& settings)
{
    const auto& lv = settings.get_child("logging").begin();
    m_Level = GetLevel(lv->second.get_value<std::string>());
}

} // namespace logging
