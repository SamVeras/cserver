#include "logging.h"
#include "net_utils.h"
#include "logging.h"
#include "config.h"

#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>

/* -------------------------------------------------------------------------- */

/** @brief Log strings for log levels.*/
static const char* log_strings[6] = {
    "[TRACE]", "[DEBUG]", "[INFO]", "[WARN]", "[ERROR]", "[FATAL]"};

/**
 * @brief Local file stream where log messages are written.
 * This is the file stream that is opened when wlog_startup() is called.
 * It is used to store log messages in a file.
 */
static FILE* lfs;

/**
 * @brief Log status.
 * This variable stores the current status of the logging system.
 * It is used to prevent the logging system from being used before it is
 * initialized, and to prevent it from being initialized multiple times.
 */
static LogStatus ls = LS_UNINITIALIZED;

/* -------------------------------------------------------------------------- */

int wlog_startup()
{
    if (ls == LS_NONINITFAILURE)  // Ensure that logging is not already set up
    {
        wlog(ERROR, "Out of order logging initialization. Start up before use.");
        return EXIT_FAILURE;
    }

    lfs = fopen(LOG_FILE_NAME, "a");  // Open log_event file stream

    if (lfs == NULL)
    {
        ls = LS_FAILURE;
        wlog(ERROR, "Error encountered during logging startup: %s\n", strerror(errno));
        wlog(INFO, "Logging to file is now disabled.\n");
        return EXIT_FAILURE;
    }

    ls = LS_SUCCESSFUL;
    wlog(INFO, "Log file stream opened.");
    return EXIT_SUCCESS;
};

/* -------------------------------------------------------------------------- */

int wlog_shutdown()
{
    if (lfs)
    {
        wlog(INFO, "Closing log file stream.");
        fclose(lfs);
        return EXIT_SUCCESS;
    }

    wlog(WARNING, "Closing log file failed: file stream is not open.");
    return EXIT_FAILURE;
}

/* -------------------------------------------------------------------------- */

int wlog(LogLevel lvl, char message[], ...)
{
    if (ls == LS_UNINITIALIZED)  // User has forgotten to call wlog_startup()
    {
        ls = LS_NONINITFAILURE;
        wlog(ERROR,
             "Logging has not been initialized properly. "
             "This message will only be displayed once.");
        return EXIT_FAILURE;
    }

    if (lvl < LOG_LEVEL)  // Should this message even be printed?
        return EXIT_SUCCESS;

    if (message[0] == '\0' || message[0] == '\n')  // Check if message is empty.
    {
        wlog(INFO, "Empty log message, what the sigma?");
        return EXIT_FAILURE;
    }

    char log_message[256];  // Create mutable copy of message
    strncpy(log_message, message, sizeof log_message - 1);

    // Create and initialize variable argument list object, then format message copy
    va_list args;
    va_start(args, message);
    vsnprintf(log_message, sizeof log_message, message, args);
    format_log_message(log_message, strlen(log_message));
    va_end(args);

    char log_time[20];
    get_current_time(log_time, sizeof log_time);  // Get current time (formatted)

    // char ll[8];  // Log level part of log message, e.g. "[  FATAL  ]"
    // center_text(log_strings[lvl], ll, sizeof ll - 1);
    const char* ll = log_strings[lvl];

    /* ---------------------------------------------------------------------- */

    fprintf(stderr, "%s %s %s", log_time, ll, log_message);

    if (ls <= LS_FAILURE)  // Logging to file has failed or has not been initialized
    {
        return EXIT_FAILURE;
    }

    if (!lfs)
    {
        fprintf(stderr, "Log file stream broken. Logging to file is now disabled.");
        ls = LS_STREAMBROKEN;
        return EXIT_FAILURE;
    }

    /* ---------------------------------------------------------------------- */

    fprintf(lfs, "%s %s %s", log_time, ll, log_message);
    fflush(lfs);

    return EXIT_SUCCESS;
}
