#pragma once
#include <stdio.h>

typedef enum LogLevelEnum
{
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
} LogLevel;

void wlog_startup();
void wlog_shutdown();
void wlog(LogLevel lvl, char message[], ...);
