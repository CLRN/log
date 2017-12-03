#pragma once

#include "log.h"


namespace logging
{

class CurrentLog
{
public:

    static ILog* Get();
    static void Set(ILog* log);
    static void SetDefault(ILog* log);
};

} // namespace logging