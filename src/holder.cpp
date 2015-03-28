#include "holder.h"

#include <boost/thread/tss.hpp>

namespace logging
{

boost::thread_specific_ptr<ILog> g_Value;

ILog* CurrentLog::Get()
{
    return g_Value.get();
}

void CurrentLog::Set(ILog* log)
{
    g_Value.reset(log);
}

} // namespace logging