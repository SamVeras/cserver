/* -------------------------------------------------------------------------- */
/*                            Server functionality                            */
/* -------------------------------------------------------------------------- */

#pragma once

typedef enum ServerStatusEnum
{
    SST_OUTOFORDERCALL = -3,  // User attempted to start up server while it's running.
    SST_NONINITFAILURE = -2,  // User attempted to run server before initializing it.
    SST_FAILURE        = -1,  // Error encountered during server initialization.
    SST_UNINITIALIZED  = 0,   // Server start has not been called yet.
    SST_RUNNING        = 1    // Server is currently running.
} ServerStatus;

/* -------------------------------------------------------------------------- */
// All functions return EXIT_SUCCESS or EXIT_FAILURE if an error occurs.

// Initializes the server, sets up socket, signal handling, and binds to port.
int server_start();

// Runs main loop, accepts and handles client requests until a shutdown request.
int server_run();

// Shuts down the server, closes sockets and cleans up resources.
int server_shutdown();

// Handles client request, parses HTTP method and path.
int handle_user_request(int client_socket, char* req);

// Constructs headers and sends the requested file to the client in chunks.
int send_file(int client_socket, const char path[]);

// Send formatted error page to user.
int send_error_page(int client_socket, const char* code, const char* title, const char* message);

// Handle specific clients in forks.
int server_client_handler(int client_socket);
