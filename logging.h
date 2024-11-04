/* -------------------------------------------------------------------------- */
/*                                   Logging                                  */
/* -------------------------------------------------------------------------- */

#pragma once
#include <stdio.h>

typedef enum LogLevelEnum
{
    TRACE,    // Very detailed information, for tracing code execution.
    DEBUG,    // Debugging info, more verbose than INFO.
    INFO,     // General information about the program's execution.
    WARNING,  // Potential issue, not currently impacting functionality.
    ERROR,    // Serious issue, something failed but the program can still run.
    FATAL     // Critical error, program cannot recover.
} LogLevel;

typedef enum FileLogStatusEnum
{
    LS_STREAMBROKEN   = -3,  // Stream to log file broken during runtime.
    LS_NONINITFAILURE = -2,  // User attempted to use logging before initializing.
    LS_FAILURE        = -1,  // Logging initialization failed.
    LS_UNINITIALIZED  = 0,   // Logging functionality has not been initialized yet. Default state.
    LS_SUCCESSFUL     = 1    // Logging to file successfully started.
} LogStatus;

/* -------------------------------------------------------------------------- */

// Attempt to set up logging to file
int wlog_startup();

// Logging shutdown procedures, such as closing log file stream
int wlog_shutdown();

// Main logging function, logs date and message to both stderr and local log file
int wlog(LogLevel lvl, char message[], ...);
