/* ---------------------------------------------------------------------------------------------- */
/*                                      Server functionality                                      */
/* ---------------------------------------------------------------------------------------------- */

#pragma once
// All functions return EXIT_SUCCESS on successful completion or EXIT_FAILURE if an error occurs.

// Initializes the server, sets up socket and signal handling, and binds to the specified port.
int server_start();

// Runs the main loop, accepting and handling client requests until a shutdown is requested.
int server_run();

// Shuts down the server, closes sockets, cleans up resources.
int server_shutdown();

// Handles client request, parses HTTP method and path.
int handle_user_request(int client_socket, char* req);

// Constructs headers and sends the requested file to the client in chunks.
int send_file(int client_socket, const char path[]);