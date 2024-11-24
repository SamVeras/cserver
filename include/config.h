/* -------------------------------------------------------------------------- */
/*                                Configuration                               */
/* -------------------------------------------------------------------------- */

#pragma once
#include "logging.h"

/** @brief Convert a macro argument to a string literal. */
#define _STR(X) #X

/** @brief Expand a macro argument and convert to a string literal. */
#define STR(X) _STR(X)

/** @brief Buffer size for network communication. */
extern int BUFFER_SIZE;
/** @brief Maximum length of the client connection queue. */
extern int BACKLOG;
/** @brief Minimum log level for messages. */
extern LogLevel LOG_LEVEL;
/** @brief Port number for the server. */
extern int SERVER_PORT;
/** @brief Maximum number of clients that can be connected at the same time. */
extern int MAX_CLIENTS;
/** @brief Path to the log file. */
extern char* LOG_FILE_NAME;
/** @brief Root directory for serving files. */
extern char* ROOT_DIR;
/** @brief Favicon file name. File to be served when receiving a request for /favicon.ico. */
extern char* FAVICON_FILE;

/**
 * @brief Parses an argument and assigns the value to the target integer.
 *
 * @param[in] arg The name of the argument (for error messages).
 * @param[in] value The value of the argument.
 * @param[out] target The integer to store the parsed value in.
 *
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure. */
int parse_arg(const char* arg, const char* value, int* target);

/**
 * @brief Sets up the server by parsing command line arguments.
 * @param[in] argc The number of command line arguments.
 * @param[in] argv The command line arguments.
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure. */
int config_server(int argc, char const* argv[]);

/**
 * @brief Displays the current server configuration settings.
 * This function prints the current values of the server configuration
 * parameters to the standard error stream. */
void server_config_show();

/**
 * @brief Displays the help message for server configuration options.
 * This function prints out the usage instructions and available command
 * line options for configuring the server. Each option includes a
 * description, the allowed values, and the default value. */
void config_help();