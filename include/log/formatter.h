#pragma once

#include "conversion/cast.hpp"

#include "log.h"

#include <set>
#include <vector>
#include <map>

#include <boost/format.hpp>
#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/comparison/greater.hpp>
#include <boost/preprocessor/variadic/size.hpp>

// Helpers
#define FORMAT_FOR_ZERO_ARGS(text) logging::MessageFormatter(text).GetText()
#define FORMAT_FOR_NONZERO_ARGS(...) logging::MessageFormatter(__VA_ARGS__).GetText()
#define FORMAT_ARGS(...) BOOST_PP_IIF( \
    BOOST_PP_GREATER(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 1), \
        FORMAT_FOR_NONZERO_ARGS, FORMAT_FOR_ZERO_ARGS)(__VA_ARGS__) \

//! Text formatting
#define TXT(...)                                                                                                \
    FORMAT_ARGS(__VA_ARGS__)

//! Logging macro
#define LOG_MACRO(logger, level, module, ...)                                                                   \
    logger->Write(module, level, TXT(__VA_ARGS__), __FILE__, __LINE__, __FUNCTION__)

#define LINFO(logger, module, ...)                                                                              \
    ((logger && logger->IsEnabled(module, ILog::Level::Info)) ?		                                            \
        LOG_MACRO(logger, ILog::Level::Info, module, __VA_ARGS__) : void())
#define LERROR(logger, module, ...)		                                                                        \
    ((logger && logger->IsEnabled(module, ILog::Level::Error)) ?		                                        \
        LOG_MACRO(logger, ILog::Level::Error, module, __VA_ARGS__) : void())
#define LWARNING(logger, module, ...)	                                                                        \
    ((logger && logger->IsEnabled(module, ILog::Level::Warning)) ?	                                            \
        LOG_MACRO(logger, ILog::Level::Warning, module, __VA_ARGS__) : void())
#define LTRACE(logger, module, ...)	                                                                            \
    ((logger && logger->IsEnabled(module, ILog::Level::Trace)) ?		                                        \
        LOG_MACRO(logger, ILog::Level::Trace, module, __VA_ARGS__) : void())
#define LDEBUG(logger, module, ...)                                                                             \
    ((logger && logger->IsEnabled(module, ILog::Level::Debug)) ?		                                        \
        LOG_MACRO(logger, ILog::Level::Debug, module, __VA_ARGS__) : void())
#define LLEVELED(logger, module, lvl, ...)                                                                      \
    ((logger && logger->IsEnabled(module, lvl)) ?		                                                        \
        LOG_MACRO(logger, lvl, module, __VA_ARGS__) : void())

#define LOG_INFO(...) \
    LINFO(logging::CurrentLog::Get(), CURRENT_MODULE_ID, __VA_ARGS__)
#define LOG_ERROR(...) \
	LERROR(logging::CurrentLog::Get(), CURRENT_MODULE_ID, __VA_ARGS__)
#define LOG_WARNING(...) \
	LWARNING(logging::CurrentLog::Get(), CURRENT_MODULE_ID, __VA_ARGS__)
#define LOG_TRACE(...) \
	LTRACE(logging::CurrentLog::Get(), CURRENT_MODULE_ID, __VA_ARGS__)
#define LOG_DEBUG(...) \
	LDEBUG(logging::CurrentLog::Get(), CURRENT_MODULE_ID, __VA_ARGS__)
#define LOG_LEVELED(lvl, ...) \
	LLEVELED(logging::CurrentLog::Get(), CURRENT_MODULE_ID, static_cast<ILog::Level::Value>(lvl), __VA_ARGS__)

#define SET_LOGGING_MODULE(name) static const char CURRENT_MODULE_ID[] = name;

namespace logging
{
    class MessageFormatter
    {
    public:

        template <typename ... T>
        MessageFormatter(const std::string& text, const T&... args)
            : m_Text(text)
            , m_Format(text)
        {
            Print(args...);
        }

        void Print()
        {
        }

        template <typename T, typename ... Args>
        void Print(const T& arg, const Args&... args)
        {
            m_Format % arg;
            Print(args...);
        }

        template <typename ... Args>
        void Print(const std::wstring& value, const Args&... args)
        {
            m_Format % conv::cast<std::string>(value);
            Print(args...);
        }

        template <typename T, typename ... Args>
        void Print(const std::vector<T>& arg, const Args&... args)
        {
            WriteSequence(arg);
            Print(args...);
        }

        template <typename T, typename ... Args>
        void Print(const std::set<T>& arg, const Args&... args)
        {
            WriteSequence(arg);
            Print(args...);
        }

        template <typename T, typename ... Args>
        void Print(const std::list<T>& arg, const Args&... args)
        {
            WriteSequence(arg);
            Print(args...);
        }

        template <typename T, typename ... Args>
        void Print(const std::deque<T>& arg, const Args&... args)
        {
            WriteSequence(arg);
            Print(args...);
        }

        template<typename K, typename V, typename A, typename ... Args>
        void Print(const std::map<K, V, A>& arg, const Args&... args)
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
            Print(args...);
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

        std::string GetText() const
        {
            return m_Format.remaining_args() ? m_Text : m_Format.str();
        }

    protected:
        boost::format m_Format;
        const std::string& m_Text;
    };

}

