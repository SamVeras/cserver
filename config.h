#pragma once

#include "logging.h"

#define _STR(X) #X
#define STR(X)  _STR(X)

// TODO Get options from file?

// #define LOG_FILE_NAME server.log
// #define SERVER_PORT   9902
// #define BUFFER_SIZE   1024
// #define BACKLOG       5
// #define LOG_LEVEL     TRACE

extern int      BUFFER_SIZE;
extern int      BACKLOG;
extern LogLevel LOG_LEVEL;
extern int      SERVER_PORT;
extern char*    LOG_FILE_NAME;

// Helper function to parse integer arguments.
int parse_arg(const char* arg, const char* value, int* target);
// Main server configuration function.
int config_server(int argc, char const* argv[]);
// Show current server setup configuration.
void server_config_show();
// Show usage information.
void config_help();