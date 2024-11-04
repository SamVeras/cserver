#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int      BUFFER_SIZE   = -1;
int      BACKLOG       = -1;
LogLevel LOG_LEVEL     = -1;
int      SERVER_PORT   = -1;
char*    LOG_FILE_NAME = "";

int parse_arg(const char* arg, const char* value, int* target)
{
    if (!value)
    {
        fprintf(stderr, "Expected value after %s.\n", arg);
        return EXIT_FAILURE;
    }

    errno   = 0;  // strtol doesn't set errno to 0 on success because it sucks
    *target = strtol(value, NULL, 10);

    if (errno != 0)
    {
        fprintf(stderr, "Invalid number! (%s) errno: %d %s.\n", value, errno, strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int config_server(int argc, char const* argv[])
{
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        config_help();
        return EXIT_FAILURE;
    }

    // Default values:
    SERVER_PORT       = 0;     // Passing port 0 to socket() gives a random port
    BUFFER_SIZE       = 1024;  // In bytes
    LOG_LEVEL         = INFO;  // Messages of this level and above will be shown
    int log_level_int = 2;
    BACKLOG           = 5;     // Connection queue size
    LOG_FILE_NAME     = "server.log";

    if (argc == 1)
    {
        fprintf(stderr, "Using default arguments.\n");
        return EXIT_SUCCESS;
    }

    for (int i = 1; i < argc; i++)
    {
        if (i + 1 >= argc)  // Check if there's a next argument
        {
            fprintf(stderr, "Expected value after %s.\n", argv[i]);
            return EXIT_FAILURE;
        }

        if (strcmp("-p", argv[i]) == 0)
        {
            i++;
            if (parse_arg(argv[i - 1], argv[i], &SERVER_PORT))
            {
                return EXIT_FAILURE;
            }
        }
        else if (strcmp("-b", argv[i]) == 0)
        {
            i++;
            if (parse_arg(argv[i - 1], argv[i], &BUFFER_SIZE))
            {
                return EXIT_FAILURE;
            }
        }
        else if (strcmp("-l", argv[i]) == 0)
        {
            i++;
            if (parse_arg(argv[i - 1], argv[i], &log_level_int))
            {
                return EXIT_FAILURE;
            }
        }
        else if (strcmp("-c", argv[i]) == 0)
        {
            i++;
            if (parse_arg(argv[i - 1], argv[i], &BACKLOG))
            {
                return EXIT_FAILURE;
            }
        }
        else if (strcmp("-f", argv[i]) == 0)
        {
            LOG_FILE_NAME = strdup(argv[++i]);
        }
        else
        {
            fprintf(stderr, "Unknown option: %s.\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    // Check validity of user args

    if (BUFFER_SIZE <= 0)
    {
        fprintf(stderr, "Buffer size must be greater than 0.\n");
        return EXIT_FAILURE;
    }

    if (SERVER_PORT != 0 && (SERVER_PORT < 1024 || SERVER_PORT > 65535))
    {
        fprintf(stderr,
                "Server port not allowed: %d. "
                "Port must be in range [1024, 65535]\n",
                SERVER_PORT);
        return EXIT_FAILURE;
    }

    if (log_level_int < 0 || log_level_int > 5)
    {
        fprintf(stderr,
                "Unknown / invalid log level: %d. "
                "Known log levels in range [0, 5] (0=TRACE, 5=FATAL)\n",
                log_level_int);
        return EXIT_FAILURE;
    }

    if (BACKLOG < 1 || BACKLOG > 1024)
    {
        fprintf(stderr,
                "Backlog size out of allowed range: %d."
                "Valid values in range [1, 1024]\n",
                BACKLOG);
        return EXIT_FAILURE;
    }

    if (strcmp(LOG_FILE_NAME, "") == 0)
    {
        fprintf(stderr, "Log file name cannot be empty.\n");
        return EXIT_FAILURE;
    }

    LOG_LEVEL = (LogLevel) log_level_int;

    fprintf(stderr, "Arguments successfully set.\n");

    return EXIT_SUCCESS;
}

void server_config_show()
{
    fprintf(stderr,
            "port = %d, buffer_size = %d, log_level = %d, "
            "backlog = %d, log_file = %s\n",
            SERVER_PORT,
            BUFFER_SIZE,
            LOG_LEVEL,
            BACKLOG,
            LOG_FILE_NAME);
    return;
}

void config_help()
{
    fprintf(stderr,
            "Usage: server [OPTIONS]...\n"
            "-p, --port PORT\t\t\tChoose specific port to bind to. "
            "Must be in range [1024, 65535] or 0, for a random port. "
            "Defaults to 0.\n"

            "-b, --buffer BUFF_SIZE\t\tBuffer size for file transfer, in bytes. "
            "Must be a positive value. "
            "Defaults to 1024.\n"

            "-l, --log-level LOGLEVEL\tLog level in range [0, 5]. "
            "TRACE, DEBUG, INFO, WARN, ERROR, FATAL. "
            "Defaults to INFO (2).\n"

            "-c, --backlog MAXCONNECT\tMaximum number of connections in queue. "
            "Must be a positive value. "
            "Defaults to 5.\n"

            "-f, --log-file LOGFILE\t\tName of file to write log to. "
            "Will be created if it does not exist. "
            "If specified, file name must not be empty. "
            "Defaults to server.log.\n");
}