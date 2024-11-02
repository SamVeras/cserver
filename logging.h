/* ---------------------------------------------------------------------------------------------- */
/*                                             Logging                                            */
/* ---------------------------------------------------------------------------------------------- */

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

// Attempt to set up logging to file
void wlog_startup();

// Logging shutdown procedures, such as closing log file stream
void wlog_shutdown();

// Main logging function
void wlog(LogLevel lvl, char message[], ...);
