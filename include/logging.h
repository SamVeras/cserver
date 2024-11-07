/* -------------------------------------------------------------------------- */
/*                                   Logging                                  */
/* -------------------------------------------------------------------------- */

#pragma once
#include <stdio.h>

/**
 * @brief Log levels.
 * These are the log levels that can be used when writing to the log.
 * The levels are ordered from most detailed to least detailed. */
typedef enum LogLevelEnum
{
    /** @brief Very detailed information, for tracing code execution. */
    TRACE,
    /** @brief Debugging info, more verbose than INFO. */
    DEBUG,
    /** @brief General information about the program's execution.*/
    INFO,
    /** @brief Potential issue, not currently impacting functionality. */
    WARNING,
    /** @brief Serious issue, something failed but the program can still run. */
    ERROR,
    /** @brief Critical error, program cannot recover. */
    FATAL
} LogLevel;

/**
 * @brief File logging status.
 * Describes the current status of the local log file stream. */
typedef enum FileLogStatusEnum
{
    /** @brief Stream to log file broken during runtime. */
    LS_STREAMBROKEN = -3,
    /** @brief User attempted to use logging before initializing. */
    LS_NONINITFAILURE = -2,
    /** @brief Logging initialization failed. */
    LS_FAILURE = -1,
    /** @brief Logging functionality has not been initialized yet. */
    LS_UNINITIALIZED = 0,
    /** @brief Logging to file successfully started. */
    LS_SUCCESSFUL = 1
} LogStatus;

/* -------------------------------------------------------------------------- */

/**
 * @brief Set up logging to a file.
 * This function is used to start logging to a file. It opens the file specified
 * by the LOG_FILE_NAME variable and stores a pointer to the file stream in the
 * lfs variable.
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error. */
int wlog_startup();

/**
 * @brief Closes the log file stream if it is open.
 *
 * This function should be called when logging is no longer needed.
 * It ensures that the log file stream is properly closed and logs the
 * appropriate message based on the success or failure of the operation.
 *
 * @return EXIT_SUCCESS on successfully closing the file stream,
 *         EXIT_FAILURE if the file stream was not open. */
int wlog_shutdown();

/**
 * @brief Prints a log message to the console and log file stream.
 *
 * @param[in] lvl The log level to check against the LOG_LEVEL constant.
 * @param[in] message The format string for the log message.
 * @param[in] ... The arguments to be formatted into the log message.
 *
 * @return EXIT_SUCCESS on successful log, EXIT_FAILURE on failure. */
int wlog(LogLevel lvl, char message[], ...);
