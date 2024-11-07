/* -------------------------------------------------------------------------- */
/*                            Server functionality                            */
/* -------------------------------------------------------------------------- */

#pragma once

/** @brief The current status of the server. */
typedef enum ServerStatusEnum
{
    /** @brief The user attempted to start up server while it's running. **/
    SST_OUTOFORDERCALL = -3,
    /** @brief The user attempted to run server before initializing it. */
    SST_NONINITFAILURE = -2,
    /** @brief Error encountered during server initialization. */
    SST_FAILURE = -1,
    /** @brief Server start has not been initialized yet. */
    SST_UNINITIALIZED = 0,
    /** @brief Server is currently running. */
    SST_RUNNING = 1
} ServerStatus;

/* -- All functions return EXIT_SUCCESS or EXIT_FAILURE if an error occurs. - */

/**
 * @brief Initialize the server.
 * This function sets up the server by initializing logging,
 * setting up signal handling, getting the local address info,
 * creating the server socket, setting options, binding the
 * socket, and marking it as passive.
 * @return 0 on success, -1 on failure.
 */
int server_start();

/**
 * @brief Runs the server's main loop, accepting and handling client requests.
 * This function monitors incoming connections using poll and forks a child
 * process to handle each client request. It continues running until a shutdown
 * is requested.
 * @return EXIT_SUCCESS on successful shutdown, EXIT_FAILURE on error.
 */
int server_run();

/**
 * @brief Shutdown the server.
 * Shuts down the server by closing the server and client sockets and
 * freeing the address info.
 * @return EXIT_SUCCESS on successful shutdown, EXIT_FAILURE on error.
 */
int server_shutdown();

/**
 * @brief Handles an HTTP request from a client.
 * This function parses the HTTP request to extract the method and path,
 * logs the request details, checks for path traversal attempts, and
 * serves the requested file or sends an error page if necessary.
 * @param client_socket The socket associated with the client.
 * @param req The HTTP request received from the client.
 * @return EXIT_SUCCESS on successful file serving, EXIT_FAILURE on error.
 */
int handle_user_request(int client_socket, char* req);

/**
 * @brief Sends a file to a client.
 * @param client_socket The socket where the file should be sent.
 * @param path The path to the file to be sent.
 * @return 0 on success, -1 on failure.
 */
int send_file(int client_socket, const char path[]);

/**
 * @brief Send an error page to the user.
 * @param client_socket The socket where the error page should be sent.
 * @param code The HTTP status code for the error.
 * @param title The title of the error page.
 * @param message The message to be displayed on the error page.
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int send_error_page(int client_socket, const char* code, const char* title, const char* message);

/**
 * @brief Handle a single client request.
 * @param[in] client_socket File descriptor of the socket to read from.
 * @return 0 on success, -1 on failure.
 */
int server_client_handler(int client_socket);

/**
 * @brief Serves a directory listing as a web page to the client.
 * This function utilizes the 'tree' command to generate a directory listing
 * of the 'data' directory to a .HTML file and then send it to the client.
 * @param client_socket The socket associated with the client.
 * @return EXIT_SUCCESS on successfully sending the directory listing, EXIT_FAILURE on error.*/
int serve_data_tree(int client_socket);
