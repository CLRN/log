#pragma once

#include "log.h"

#include <set>
#include <vector>
#include <map>

#include <boost/format.hpp>

//! Logging macro
#define LOG_MACRO(logger, text, level, module)                                                                  \
    logging::MessageFormatter(*logger, module, level, text, __FILE__, __LINE__, __FUNCTION__) 

#define LINFO(logger, module, message)		if (logger && logger->IsEnabled(module, ILog::Level::Info))		    \
    LOG_MACRO(logger, message, ILog::Level::Info, module)
#define LERROR(logger, module, message)		if (logger && logger->IsEnabled(module, ILog::Level::Error))		\
    LOG_MACRO(logger, message, ILog::Level::Error, module)
#define LWARNING(logger, module, message)	if (logger && logger->IsEnabled(module, ILog::Level::Warning))	    \
    LOG_MACRO(logger, message, ILog::Level::Warning, module)
#define LTRACE(logger, module, message)		if (logger && logger->IsEnabled(module, ILog::Level::Trace))		\
    LOG_MACRO(logger, message, ILog::Level::Trace, module)
#define LDEBUG(logger, module, message)		if (logger && logger->IsEnabled(module, ILog::Level::Debug))		\
    LOG_MACRO(logger, message, ILog::Level::Debug, module)

#define LOG_INFO(message) \
    LINFO(logging::CurrentLog::Get(), CURRENT_MODULE_ID, message)
#define LOG_ERROR(message) \
	LERROR(logging::CurrentLog::Get(), CURRENT_MODULE_ID, message)
#define LOG_WARNING(message) \
	LWARNING(logging::CurrentLog::Get(), CURRENT_MODULE_ID, message)
#define LOG_TRACE(message) \
	LTRACE(logging::CurrentLog::Get(), CURRENT_MODULE_ID, message)
#define LOG_DEBUG(message) \
	LDEBUG(logging::CurrentLog::Get(), CURRENT_MODULE_ID, message)

#define SET_LOGGING_MODULE(name) static const char CURRENT_MODULE_ID[] = name;

namespace logging
{
    class MessageFormatter
    {
    public:
        MessageFormatter(ILog& log, const char* module, ILog::Level::Value level, const std::string& text, const char* file, unsigned line, const char* function) 
            : m_Log(log) 
            , m_Module(module)
            , m_Level(level)
            , m_Text(text)
            , m_Format(text)
            , m_File(file)
            , m_Line(line)
            , m_Function(function)
        {}
        ~MessageFormatter()
        {
            m_Log.Write(m_Module, m_Level, m_Format.fed_args() ? m_Format.str() : m_Text, m_File, m_Line, m_Function);
        }
        template<typename T>
        MessageFormatter& operator % (const T& value)
        {
            m_Format % value;
            return *this;
        }
        MessageFormatter& operator % (const std::wstring& value)
        {
            m_Format % conv::cast<std::string>(value);
            return *this;
        }
        template<typename T>
        MessageFormatter& operator % (const std::vector<T>& value)
        {
            WriteSequence(value);
            return *this;
        }
        template<typename T>
        MessageFormatter& operator % (const std::deque<T>& value)
        {
            WriteSequence(value);
            return *this;
        }
        template<typename T>
        MessageFormatter& operator % (const std::set<T>& value)
        {
            WriteSequence(value);
            return *this;
        }
        template<typename K, typename V, typename A>
        MessageFormatter& operator % (const std::map<K, V, A>& arg)
        {
            std::ostringstream oss;
            auto it = arg.begin();
            auto end = arg.end();
            for (; it != end; ++it)
            {
                if (it != arg.begin())
                    oss << ",";
                oss << it->first << ":" << it->second;
            }

            m_Format % oss.str();
            return *this;
        }
        template<typename T>
        void WriteSequence(const T& arg)
        {
            std::ostringstream oss;
            auto it = arg.begin();
            auto end = arg.end();
            for (; it != end; ++it)
            {
                if (it != arg.begin())
                    oss << ",";
                oss << *it;
            }

            m_Format % oss.str();
        }
    private:
        ILog& m_Log;
        const char* m_Module;
        const ILog::Level::Value m_Level;
        const char* const m_File;
        const unsigned m_Line;
        const char* const m_Function;
        boost::format m_Format;
        const std::string& m_Text;
    };
}

