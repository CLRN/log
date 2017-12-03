#include "log/holder.h"

#include <boost/thread/tss.hpp>

namespace logging
{

boost::thread_specific_ptr<ILog> g_Value;
std::unique_ptr<ILog> g_Default;

ILog* CurrentLog::Get()
{
    return g_Value.get() ? g_Value.get() : g_Default.get();
}

void CurrentLog::Set(ILog* log)
{
    g_Value.reset(log);
}

void CurrentLog::SetDefault(ILog* log)
{
    g_Default.reset(log);
}

} // namespace logging