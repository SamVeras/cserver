#include "server.h"
#include "logging.h"
#include "net_utils.h"
#include "config.h"
#include "sig.h"

#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
/* -------------------------------------------------------------------------- */

/**
 * @brief The current status of the server.
 * Tracks the server's current state to ensure proper operation.  It prevents
 * the server from being started if it is already running, and prevents shutdown
 * attempts if the server has not been initialized. */
static ServerStatus sst = SST_UNINITIALIZED;

/**
 * @brief Global error code.
 * Stores the last error code when a function returns an error. */
static int err = 0;

/**
 * @brief Server socket.
 * File descriptor of the server socket. */
static int ssfd = 0;

/**
 * @brief Client socket.
 * File descriptor of the client socket. */
static int csfd = 0;
// TODO Why am I using a global csfd if I pass the client socket to functions?
/**
 * @brief Server address info.
 * Linked list with >= 1 results from the getaddrinfo() function. */
static struct addrinfo* sai;

/**
 * @brief Client socket address.
 * Client socket address, which can be casted to a sockaddr struct. */
static struct sockaddr_storage csa;

/**
 * @brief Client socket address length.
 * Length of the client socket address. */
static socklen_t csa_size = sizeof csa;

/**
 * @brief Favicon file name.
 * Name of the favicon file to be served when receiving a request for
 * /favicon.ico. */
static const char* favicon = "favicon32.png";

/**
 * @brief Landing page file name.
 * Name of the landing page file to be served when the user requests
 * the root directory. */
static const char* landing = "index.html";

/* -------------------------------------------------------------------------- */

int server_start()
{
    wlog_startup();  // Start logging

    wlog(DEBUG, "Checking server status during initialization attempt. (%d)", (int) sst);

    if (sst != SST_UNINITIALIZED)
    {
        wlog(FATAL, "Out of order server start call detected.");
        sst = SST_OUTOFORDERCALL;
        return EXIT_FAILURE;
    }

    wlog(INFO, "Server starting up...");

    if (sigh_startup() == -1)  // Start signal handling
    {
        wlog(FATAL, "Signal handling setup incomplete.");
        sst = SST_FAILURE;
        return EXIT_FAILURE;
    };

    struct addrinfo hints;            // Struct with data to guide getaddrinfo()
    memset(&hints, 0, sizeof hints);  // Clear structure
    hints.ai_family   = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TDP
    hints.ai_flags    = AI_PASSIVE;   // Local address

    wlog(INFO, "Getting local address info...");

    char port_string[6];  // Getaddrinfo requires port as a string.
    snprintf(port_string, sizeof port_string, "%d", SERVER_PORT);
    err = getaddrinfo(NULL, port_string, &hints, &sai);

    if (err != 0)
    {
        wlog(FATAL, "Failed to get address info. Error %d %s.", err, gai_strerror(err));
        sst = SST_FAILURE;
        return EXIT_FAILURE;
    }

    wlog(DEBUG, "Get address operation successful.");

    wlog(INFO, "Creating server socket...");
    ssfd = socket(sai->ai_family, sai->ai_socktype, sai->ai_protocol);

    if (ssfd == -1)
    {
        wlog(FATAL, "Failed to create server socket. %d %s.", errno, strerror(errno));
        sst = SST_FAILURE;
        return EXIT_FAILURE;
    }

    wlog(DEBUG, "Server socket created.");

    wlog(INFO, "Setting socket option %d (SO_REUSEADDR)...", SO_REUSEADDR);
    err = setsockopt(ssfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    if (err == -1)
        wlog(ERROR, "Failed to set socket option. %d %s.", errno, strerror(errno));

    wlog(DEBUG, "Socket option successfully set.");

    wlog(INFO, "Setting server socket to non-blocking...");
    err = fcntl(ssfd, F_SETFL, fcntl(ssfd, F_GETFL, 0) | O_NONBLOCK);

    if (err == -1)
    {
        wlog(FATAL, "Failed to set server socket to non-blocking. %s.", strerror(errno));
        sst = SST_FAILURE;
        return EXIT_FAILURE;
    }

    wlog(DEBUG, "Server socket successfully set to non-blocking.");

    wlog(INFO, "Binding server socket with port on local machine...");
    err = bind(ssfd, sai->ai_addr, sai->ai_addrlen);

    if (err == -1)
    {
        wlog(FATAL, "Failed to bind server socket. %d %s.", errno, strerror(errno));
        sst = SST_FAILURE;
        return EXIT_FAILURE;
    }

    wlog(DEBUG, "Server socket bound.");

    // Mark server socket as passive, ready to accept connections
    wlog(INFO, "Marking server socket as passive (listen)...");
    err = listen(ssfd, BACKLOG);

    if (err == -1)
    {
        wlog(FATAL, "Failed to listen on server socket. %d %s.", errno, strerror(errno));
        sst = SST_FAILURE;
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr;
    socklen_t          addr_len = sizeof addr;

    if (getsockname(ssfd, (struct sockaddr*) &addr, &addr_len) == -1)
    {
        wlog(FATAL, "Failed to get socket name. %d %s.", errno, strerror(errno));
        sst = SST_FAILURE;
        return EXIT_FAILURE;
    }

    sst = SST_RUNNING;
    wlog(INFO, "Server listening on port %d.", ntohs(addr.sin_port));

    return EXIT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

int server_run()
{
    if (sst == SST_UNINITIALIZED)
    {
        wlog(FATAL,
             "Server has not been initialized. "
             "This message will only be displayed once.");
        sst = SST_NONINITFAILURE;
        return EXIT_FAILURE;
    }

    if (sst == SST_NONINITFAILURE)
        return EXIT_FAILURE;

    struct pollfd polled;  // We will only monitor 1 socket
    int           event_count = 0;

    polled.fd     = ssfd;    // Server
    polled.events = POLLIN;  // Poll incoming connections

    wlog(TRACE, "Entering main loop...");
    while (!shut_req)
    {
        wlog(TRACE, "Polling with 1.5s timeout...");
        event_count = poll(&polled, 1, 1500);  // 1.5 second timeout
        if (event_count < 0)
        {
            if (errno == EINTR)  // Interrupted by a signal
            {
                if (shut_req)    // Do we need to exit?
                {
                    wlog(INFO, "Shutdown requested during polling.");
                    break;
                }

                wlog(FATAL, "EINTR detected, but shutdown request = %d.", shut_req);
                wlog(INFO, "This should not happen. Manually requesting shutdown.");
                shut_req = 1;
                break;  // This should NOT happen
            }

            wlog(ERROR, "Polling failed: (%d) %s.", errno, strerror(errno));
            continue;
        }

        if (event_count == 0)
        {
            wlog(TRACE, "Continue polling...");
            continue;
        }

        if (polled.revents & POLLIN)
        {
            wlog(TRACE, "POLLIN event received.");

            csfd = accept(ssfd, (struct sockaddr*) &csa, &csa_size);
            if (csfd == -1)
            {
                wlog(ERROR, "Failed to accept connection. (%d) %s.", errno, strerror(errno));
                continue;
            }

            wlog(INFO, "Request accepted. Connected to socket.");

            char        ipstr[INET6_ADDRSTRLEN];
            int         port     = 0;
            const char* inet_err = NULL;

            if (csa.ss_family == AF_INET)
            {
                struct sockaddr_in* sin = (struct sockaddr_in*) &csa;
                port                    = ntohs(sin->sin_port);
                inet_err = inet_ntop(csa.ss_family, &sin->sin_addr, ipstr, sizeof ipstr);
            }
            else if (csa.ss_family == AF_INET6)
            {
                struct sockaddr_in6* sin = (struct sockaddr_in6*) &csa;
                port                     = ntohs(sin->sin6_port);
                inet_err = inet_ntop(csa.ss_family, &sin->sin6_addr, ipstr, sizeof ipstr);
            }

            if (inet_err == NULL)
            {
                wlog(ERROR, "Failed to determine peer's IP address.");
                close(csfd);
                continue;
            }

            if (port == 0)
            {
                wlog(ERROR, "Failed to determine peer's port number.");
                close(csfd);
                continue;
            }

            wlog(INFO, "Accepted connection from %s:%d", ipstr, port);

            wlog(TRACE, "Forking...");

            // Fork to handle client in a separate process
            // fork() returns PID of child process to parent and 0 to the child itself.
            pid_t pid = fork();

            if (pid == -1)
            {
                wlog(ERROR, "Failed to fork proccess: (%d) %s.", errno, strerror(errno));
                close(csfd);
                continue;
            }

            if (pid == 0)         // We are in a child process
            {
                if (close(ssfd))  // Close unused server socket
                    wlog(WARNING, "[%d] Failed to close server socket.", getpid());

                if (server_client_handler(csfd))
                    wlog(WARNING, "[%d] Failure during client handling.", getpid());

                if (close(csfd))  // Done
                    wlog(WARNING, "[%d] Failed to close client socket.", getpid());

                exit(EXIT_SUCCESS);  // Kill child
            }

            close(csfd);  // Parent process closes the client socket
        }

        if (shut_req)
        {
            wlog(INFO, "Shutdown requested after handling events.");
            break;  // Exit the loop if shutdown is requested
        }
    }

    wlog(INFO, "Server no longer running.");
    return EXIT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

int server_client_handler(int client_socket)
{
    char buff[BUFFER_SIZE];                                      // Buffer
    int  rec_bytes = recv(client_socket, buff, sizeof buff, 0);  // Bytes received

    if (rec_bytes < 0)
    {
        wlog(ERROR, "Failed to receive data: (%d) %s.", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    buff[rec_bytes] = '\0';
    char rec_str[32];  // TODO why is this not an array? a pointer? what?
    human_readable_size(rec_bytes, rec_str, sizeof rec_str);
    wlog(INFO, "Received %s.", rec_str);

    if (handle_user_request(client_socket, buff))
    {
        wlog(ERROR, "Failure during request handling.");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
    // Client socked is closed in server_run() after forking.
}

/* -------------------------------------------------------------------------- */

int server_shutdown()
{
    if (sst == SST_UNINITIALIZED)
    {
        fprintf(stderr, "Server shutdown attempt on uninitialized server. exit(1)\n");
        exit(EXIT_FAILURE);
    }

    wlog(INFO, "Shutting down with '%s' value...", sst != SST_RUNNING ? "FAILURE" : "SUCCESS");
    if (ssfd && close(ssfd) == -1)
        wlog(WARNING, "Failed to close server socket: %d %s.", errno, strerror(errno));

    if (csfd && close(csfd) == -1)
        wlog(WARNING, "Failed to close client socket: %d %s.", errno, strerror(errno));

    // Note: && is a short-circuiting AND, it means that the second condition will not be checked
    // (and that there will be no attempt to close the sockets) if the first one fails.

    freeaddrinfo(sai);  // Can this fail? It has no return value

    if (wlog_shutdown())
        fprintf(stderr, "Error during logging shutdown.\n");

    if (sst != SST_RUNNING)
    {
        fprintf(stderr, "Error during shutdown attempt on unitialized or failed server.\n");
        sst = SST_UNINITIALIZED;
        return EXIT_FAILURE;
    }

    sst = SST_UNINITIALIZED;
    return EXIT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

int handle_user_request(int client_socket, char* req)
{
    // ex.: GET /index.html -> method = "GET", path = "/index.html"
    char method[8], path[256];
    sscanf(req, "%s %s", method, path);

    wlog(INFO, "Request with method \"%s\" and path \"%s\"...", method, path);

    if (strstr(path, "..") || strstr(path, "//"))
    {
        wlog(WARNING, "Path traversal attempt detected: %s.", path);
        wlog(INFO, "Attempting to send 403 Forbidden page to user...");
        send_error_page(csfd, "403 Forbidden", "FORBIDDEN", "GET OUT &#x1F5E3;");
        return EXIT_FAILURE;
    }

    wlog(TRACE, "path: %s, len: %d, sizeof: %d.", path, strlen(path), sizeof path);

    if (strlen(path) == 1)
    {
        wlog(DEBUG, "Root request.");
        snprintf(path, sizeof path, "data/%s", landing);  // Página "padrão"
    }

    if (strcmp(path, "/favicon.ico") == 0)
    {
        wlog(DEBUG, "Favicon request.");
        snprintf(path, sizeof path, "data/%s", favicon);  // Favicon
    }

    if (path[0] == '/')  // This is probably be the case regardless, but we should check
    {
        memmove(path + 4, path, strlen(path) + 1);
        memcpy(path, "data", 4);
    }

    wlog(DEBUG, "Changed path to \"%s\".", path);

    if (strcmp(path, "data/index.html") == 0)
    {
        return serve_data_tree(client_socket);
    }

    return send_file(client_socket, path);
}

/* -------------------------------------------------------------------------- */

int send_file(int client_socket, const char path[])
{
    const char* content_type = get_mime_type(path);
    wlog(DEBUG, "Determined content-type to be %s.", content_type);

    wlog(INFO, "Opening file at %s and creating stream...", path);
    FILE* file = fopen(path, "rb");

    char header[512];
    if (!file)
    {
        wlog(WARNING, "Failed to open file. Sending 404 page to user.");
        send_error_page(csfd, "404 Not Found", "404", "Sorry, not found!");
        return EXIT_FAILURE;
    }

    wlog(TRACE, "Seeking size of file...");
    fseek(file, 0, SEEK_END);          // Move file pointer to end of file
    long int file_size = ftell(file);  // Count # of number of bytes
    fseek(file, 0, SEEK_SET);          // Move file pointer back to beginning
    wlog(TRACE, "Size of file is %ld.", file_size);

    build_html_header(header, sizeof(header), "200 OK", content_type, file_size);

    wlog(INFO, "Sending header to user...");
    ssize_t sent = send(client_socket, header, strlen(header), 0);
    ssize_t read;
    wlog(INFO, "%d bytes sent.", sent);

    char   buffer[BUFFER_SIZE];
    size_t bytes_read;

    wlog(INFO, "Reading file with buffer of length %d...", BUFFER_SIZE);

    unsigned long transfers = 0;
    sent                    = 0;
    read                    = 0;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        sent += send(client_socket, buffer, bytes_read, 0);
        read += bytes_read;
        transfers++;
    }

    log_transfer_data(read, sent, transfers);

    wlog(INFO, "Done reading file.");

    if (fclose(file) != 0)
    {
        wlog(WARNING, "Failed to close file: %d %s.", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    wlog(INFO, "File closed.");
    return EXIT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

int send_error_page(int client_socket, const char* code, const char* title, const char* message)
{
    char    header[512], body[512];
    ssize_t sent;

    // Build the error page body
    snprintf(body, sizeof(body), "<html><body><h1>%s</h1><p>%s</p></body></html>", title, message);

    // Build the error page header
    build_html_header(header, sizeof(header), code, "text/html", strlen(body));

    wlog(TRACE, "Sending error %s header to user...", code);
    sent = send(client_socket, header, strlen(header), 0);
    if (sent == -1)
        wlog(ERROR, "Failed to send %s error header.", code);
    wlog(TRACE, "%d bytes sent.", sent);

    wlog(TRACE, "Sending error %s body to user...", code);
    sent = send(client_socket, body, strlen(body), 0);
    if (sent == -1)
        wlog(ERROR, "Failed to send %s error body.", code);
    wlog(TRACE, "%d bytes sent.", sent);

    return EXIT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

int serve_data_tree(int client_socket)
{
    char command[80];
    // Use tree for now, until/if we make our own tree view
    snprintf(command,
             sizeof(command),
             "cd data && tree -H . --noreport --charset utf-8 . > %s",
             landing);

    if (system(command) != 0)
    {
        wlog(ERROR, "Failed to execute tree command in data/%s.", landing);
        send_error_page(client_socket,
                        "500 Internal Server Error",
                        "Internal Server Error",
                        "Failed to execute tree command.");
        return EXIT_FAILURE;
    }
    char path[16];
    snprintf(path, 16, "data/%s", landing);
    return send_file(client_socket, path);
}
