#include "logging.h"
#include "net_utils.h"
#include "logging.h"
#include "config.h"

#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>

// Log level strings
static const char* log_levels[6] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

// Local log file stream
static FILE* lfs;

// 0 uninitialized, 1 successful setup, -1 failure, -2 non-init failure
static int file_logging_status = 0;

int wlog_startup()
{
    if (file_logging_status == -2)
    {
        wlog(ERROR, "Out of order logging initialization. Start up before use.");
        return EXIT_FAILURE;
    }

    lfs = fopen(STR(LOG_FILE_NAME), "a");  // Open log_event file stream

    if (lfs == NULL)
    {
        file_logging_status = -1;
        wlog(ERROR, "Error encountered during logging startup: %s\n", strerror(errno));
        wlog(INFO, "Logging to file is now disabled.\n");
        return EXIT_FAILURE;
    }

    file_logging_status = 1;
    wlog(INFO, "Log file stream opened.");
    return EXIT_SUCCESS;
};

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

int wlog(LogLevel lvl, char message[], ...)
{
    if (file_logging_status == 0)
    {  // User has forgotten to call wlog_startup()
        // TODO test this scenario
        file_logging_status = -2;
        wlog(ERROR,
             "Logging has not been initialized properly. "
             "This message will only be displayed once.");
        return EXIT_FAILURE;
    }

    if (message[0] == '\0' || message[0] == '\n')
    {  // Checking if message is empty or begins with newline
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

    char ll[7];  // Log level part of log message, e.g. "[  FATAL  ]"
    center_text(log_levels[lvl], ll, sizeof ll - 1);

    /* ------------------------------------------------------------------------------------------ */

    fprintf(stderr, "%s ", log_time);
    if (lvl >= LOG_LEVEL)
        fprintf(stderr, "[%s] ", ll);
    fprintf(stderr, log_message);

    if (file_logging_status < 0)
    {  // Logging to file has failed or has not been initialized
        return EXIT_FAILURE;
    }

    if (!lfs)
    {
        fprintf(stderr, "Log file stream broken. Logging to file is now disabled.");
        file_logging_status = -1;
        return EXIT_FAILURE;
    }

    /* ------------------------------------------------------------------------------------------ */

    fprintf(lfs, "%s ", log_time);

    if (lvl >= LOG_LEVEL)
        fprintf(lfs, "[%s] ", ll);

    fprintf(lfs, "%s", log_message);
    fflush(lfs);

    return EXIT_SUCCESS;
}
