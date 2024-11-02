#include "logging.h"
#include "net_utils.h"
#include "logging.h"
#include "config.h"

#include <string.h>
#include <stdarg.h>
#include <errno.h>

static const char* log_levels[6]  = {"TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
static int         logging_status = 0;
static FILE*       lfs;  // Local file stream

void wlog_startup()
{
    lfs = fopen(STR(LOG_FILE_NAME), "a");  // Open log_event file stream

    if (lfs == NULL)
    {
        fprintf(stderr, "Error encountered during logging startup: %s\n", strerror(errno));
        fprintf(stderr, "Logging to file is now disabled.\n");
        logging_status = -1;
        return;
    }

    wlog(INFO, "Log file stream opened.");
    logging_status = 1;
};

void wlog_shutdown()
{
    if (lfs)
    {
        wlog(INFO, "Closing log file stream.");
        fclose(lfs);
    }
}

/* Logs date and message to both stderr and local log file.*/
void wlog(LogLevel lvl, char message[], ...)
{
    // Checking if message is empty or begins with newline
    if (message[0] == '\0' || message[0] == '\n')
    {
        wlog(INFO, "Empty log message, what the sigma?");
        return;
    }

    // Create mutable copy of message
    char log_message[256];
    strncpy(log_message, message, sizeof log_message - 1);

    // Create and initialize variable argument list object, then format message copy
    va_list args;
    va_start(args, message);
    vsnprintf(log_message, sizeof log_message, message, args);
    format_log_message(log_message, strlen(log_message));
    va_end(args);

    // Update time

    // Format log time
    char log_time[20];
    get_current_time(log_time, sizeof log_time);

    // Log level part of log message, e.g. "[  FATAL  ]"
    char ll[9];
    center_text(log_levels[lvl], ll, sizeof ll - 1);

    /* ------------------------------------------------------------------------------------------ */

    fprintf(stderr, "%s ", log_time);
    if (lvl >= LOG_LEVEL)
        fprintf(stderr, "[%s] ", ll);
    fprintf(stderr, log_message);

    if (logging_status == -1)
        return;

    if (!lfs)
    {
        fprintf(stderr, "Log file stream broken. Logging to file is now disabled.");
        logging_status = -1;
        return;
    }

    /* ------------------------------------------------------------------------------------------ */

    fprintf(lfs, "%s ", log_time);

    if (lvl >= LOG_LEVEL)
        fprintf(lfs, "[%s] ", ll);

    fprintf(lfs, "%s", log_message);
    fflush(lfs);

    return;
}
