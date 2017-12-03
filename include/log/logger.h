#pragma once

#include "conversion/cast.hpp"

#include <string>
#include <vector>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>

//! Logger interface
//!
//! \class ILog
//!
class ILog
{
public:
    
    //! Log levels
    struct Level
    {
        enum Value
        {
            Error   = 0,
            Warning = 1,
            Info    = 2,
            Debug   = 3,
            Trace   = 4,
        };

        static std::string to_string(Value v)
        {
            switch (v)
            {
            case ILog::Level::Info:
                return "INFO";
            case ILog::Level::Error:
                return "ERROR";
            case ILog::Level::Warning:
                return "WARN";
            case ILog::Level::Trace:
                return "TRACE";
            case ILog::Level::Debug:
                return "DEBUG";
            default:
                return "UNKNOWN";
            }
        }
    };

    //! Is logging enabled
    virtual bool IsEnabled(const char* module, Level::Value level) const = 0;

    //! Get module log folder
    virtual boost::filesystem::path GetLogFolder(const char* module) const = 0;

    //! Write text
    virtual void Write(const char* module, ILog::Level::Value level, const std::string& text, const char* file, unsigned line, const char* function) = 0;

    //! Set logging level
    virtual void SetLevel(Level::Value level) = 0;
    virtual void SetLevels(const boost::property_tree::ptree& settings) = 0;

    //! Destructor
    virtual ~ILog() {}
};
