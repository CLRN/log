#include "log4cplus.h"

#include "exception/CheckHelpers.h"

#include <log4cplus/logger.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/hierarchy.h>

#include <boost/filesystem/operations.hpp>
#include <boost/range/algorithm.hpp>

#include <unordered_map>


namespace logging
{
#pragma warning (push)
#pragma warning (disable : 4822) // warning C4822: local class member function does not have a body

class FileNameGetter : public log4cplus::FileAppender
{
    FileNameGetter();
public:
    const log4cplus::tstring GetFileName() const { return filename; }
};
#pragma warning (pop)

log4cplus::LogLevel GetLevel(ILog::Level::Value v)
{
    switch (v)
    {
        case ILog::Level::Info:     return log4cplus::INFO_LOG_LEVEL;
        case ILog::Level::Error:    return log4cplus::ERROR_LOG_LEVEL;
        case ILog::Level::Warning:  return log4cplus::WARN_LOG_LEVEL;
        case ILog::Level::Trace:    return log4cplus::TRACE_LOG_LEVEL;
        case ILog::Level::Debug:    return log4cplus::DEBUG_LOG_LEVEL;
        default: assert(!"wrong log level"); return 0;
    }
}

log4cplus::LogLevel GetLevel(const std::string& v)
{
    const static std::unordered_map<std::string, log4cplus::LogLevel> levels{
        { "INFO", log4cplus::INFO_LOG_LEVEL },
        { "ERROR", log4cplus::ERROR_LOG_LEVEL },
        { "WARNING", log4cplus::WARN_LOG_LEVEL },
        { "TRACE", log4cplus::TRACE_LOG_LEVEL },
        { "DEBUG", log4cplus::DEBUG_LOG_LEVEL }
    };
    return levels.at(v);
}

Log4cplus::Log4cplus(const std::string& src) : m_IsOpened()
{
    namespace fs = boost::filesystem;

    if (fs::is_regular_file(src))
    {
        log4cplus::PropertyConfigurator cfg(conv::cast<log4cplus::tstring>(src));
        cfg.configure();

        // create log folders
        std::set<fs::path> folders;

        {
            log4cplus::LoggerList loggers = log4cplus::Logger::getCurrentLoggers();
            for (auto& logger : loggers)
            {
                for (const auto& appender : logger.getAllAppenders())
                {
                    const log4cplus::FileAppender* fileAppender = dynamic_cast<const log4cplus::FileAppender*>(appender.get());
                    if (fileAppender)
                    {
                        const FileNameGetter& getter = static_cast<const FileNameGetter&>(*fileAppender);
                        const auto path = fs::system_complete(getter.GetFileName()).branch_path();
                        if (!path.empty())
                            folders.insert(path);
                    }
                }
            }
        }

        boost::for_each(folders, boost::bind(fs::create_directories, _1));

        // reconfigure
        cfg.configure();

        m_IsOpened = true;
    }
}

Log4cplus::Log4cplus(Level::Value lvl, const char* file /*= nullptr*/) : m_IsOpened()
{
    log4cplus::BasicConfigurator config;
    config.configure();

    // initialize all loggers
    //         for (const auto& module : Modules())
    //             log4cplus::Logger::getInstance(conv::cast<log4cplus::tstring>(module.str()));

    log4cplus::SharedAppenderPtr appender;
    if (file)
        appender = new log4cplus::FileAppender(file,
        std::ios_base::app,
        true);

    log4cplus::LoggerList loggers = log4cplus::Logger::getCurrentLoggers();
    for (auto& logger : loggers)
    {
        if (file)
            logger.addAppender(appender);
        logger.getRoot().setLogLevel(GetLevel(lvl));
    }

    m_IsOpened = true;
}

bool Log4cplus::IsEnabled(const char* module, Level::Value level) const
{
    if (!m_IsOpened)
        return false;

    const auto logger = log4cplus::detail::macros_get_logger(module);
    return logger.isEnabledFor(GetLevel(level));
}

boost::filesystem::path Log4cplus::GetLogFolder(const char* module) const
{
    namespace fs = boost::filesystem;

    log4cplus::LoggerList loggers = log4cplus::Logger::getCurrentLoggers();
    for (auto& logger : loggers)
    {
        for (const auto& appender : logger.getAllAppenders())
        {
            const log4cplus::FileAppender* fileAppender = dynamic_cast<const log4cplus::FileAppender*>(appender.get());
            if (fileAppender)
            {
                const FileNameGetter& getter = static_cast<const FileNameGetter&>(*fileAppender);
                return fs::system_complete(getter.GetFileName()).branch_path().wstring();
            }
        }
    }
    THROW(cmn::Exception("Unable to get log folder"));
}

void Log4cplus::Write(const char* module, ILog::Level::Value level, const std::string& text, const char* file, unsigned line, const char* function)
{
    const auto logger = log4cplus::detail::macros_get_logger(module);
    const auto logLevel = GetLevel(level);
    if (logger.isEnabledFor(logLevel))
        log4cplus::detail::macro_forced_log(logger, logLevel, text, file, line, function);
}

void Log4cplus::SetLevel(Level::Value level)
{
    log4cplus::LoggerList loggers = log4cplus::Logger::getCurrentLoggers();
    for (auto& logger : loggers)
        logger.getRoot().setLogLevel(GetLevel(level));
}

void Log4cplus::SetLevels(const boost::property_tree::ptree& settings)
{
    if (const auto logging = settings.get_child_optional("logging"))
        for (const auto& lv : logging.get()) {
            auto logger = log4cplus::detail::macros_get_logger(lv.first);
            logger.setLogLevel(GetLevel(lv.second.get_value<std::string>()));
        }
}


} // namespace logging