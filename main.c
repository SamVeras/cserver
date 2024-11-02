#include "config.h"
#include "net_utils.h"
#include "logging.h"
#include "server.h"
#include "server.h"

#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

// ---------------------------------------------------------------------------------------------- //

// TODO use arg for port and buffer size?
// int main(int argc, char const* argv[])
int main()
{
    wlog_startup();
    wlog(INFO, "Server starting up...");

    wlog(INFO, "Setting up signal handling...");
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1)   // Interactive attention signal
        wlog(ERROR, "Sigaction1 failed whatever");
    if (sigaction(SIGTERM, &sa, NULL) == -1)  // Terminal request signal
        wlog(ERROR, "Sigaction 2 failed, %s", strerror(errno));

    // ------------------------------------------------------------------------------------------ //

    int ssfd = 0;                      // Server socket / file descriptor
    int csfd = 0;                      // Client socket / file descriptor
    int err;                           // Return value auxiliary variable

    struct addrinfo  hints;            // Struct with data to guide getaddrinfo()
    struct addrinfo* sai;              // Server address info, gives a linked list with >= 1 results

    struct sockaddr_storage csa;       // Client socket address, can be casted to sockaddr
    socklen_t               csa_size;  // Client sock address length

    // ------------------------------------------------------------------------------------------ //

    csa_size = sizeof csa;
    memset(&hints, 0, sizeof hints);  // Clear structure
    hints.ai_family   = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TDP
    hints.ai_flags    = AI_PASSIVE;   // Local address

    // ------------------------------------------------------------------------------------------ //

    wlog(INFO, "Getting local address info...");
    err = getaddrinfo(NULL, STR(SERVER_PORT), &hints, &sai);

    if (err != 0)
    {
        wlog(FATAL, "Failed to get address info. Error %d %s", err, gai_strerror(err));
        goto shutdown;
    }

    wlog(INFO, "Get address operation successful.");

    /* ------------------------------------------------------------------------------------------ */

    wlog(INFO, "Creating server socket...");
    ssfd = socket(sai->ai_family, sai->ai_socktype, sai->ai_protocol);

    if (ssfd == -1)
    {
        wlog(FATAL, "Failed to create server socket. %d %s", errno, strerror(errno));
        goto shutdown;
    }

    wlog(INFO, "Server socket created.");

    /* ------------------------------------------------------------------------------------------ */

    wlog(INFO, "Setting socket option %d (SO_REUSEADDR)...", SO_REUSEADDR);
    err = setsockopt(ssfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    if (err == -1)
    {
        wlog(ERROR, "Failed to set socket option. %d %s", errno, strerror(errno));
    } else
    {
        wlog(INFO, "Socket option successfully set.");
    }

    /* ------------------------------------------------------------------------------------------ */

    wlog(INFO, "Setting server socket to non-blocking...");
    err = fcntl(ssfd, F_SETFL, fcntl(ssfd, F_GETFL, 0) | O_NONBLOCK);

    if (err == -1)
    {
        wlog(FATAL, "Failed to set server socket to non-blocking. %s", strerror(errno));
        goto shutdown;
    }

    wlog(INFO, "Server socket set successfully set to non-blocking.");

    /* ------------------------------------------------------------------------------------------ */
    wlog(INFO, "Binding server socket with port on local machine...");
    err = bind(ssfd, sai->ai_addr, sai->ai_addrlen);

    if (err == -1)
    {
        wlog(FATAL, "Failed to bind server socket. %d %s", errno, strerror(errno));
        goto shutdown;
    }

    wlog(INFO, "Server socket bound.");

    /* ------------------------------------------------------------------------------------------ */

    wlog(INFO, "Marking server socket as passive (listen)...");
    err = listen(ssfd, BACKLOG);  // Mark server socket as passive, ready to accept connections

    if (err == -1)
    {
        wlog(FATAL, "Failed to listen on server socket. %d %s", errno, strerror(errno));
        goto shutdown;
    }

    wlog(INFO, "Server listening on port %d.", SERVER_PORT);

    /* ------------------------------------------------------------------------------------------ */

    char buff[BUFFER_SIZE];
    int  rb = -1;  // Received bytes

    while (!shut_req)
    {
        struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};
        fd_set         mon;  // Monitored sockets file descriptors
        FD_ZERO(&mon);       // Clear set
        FD_SET(ssfd, &mon);  // Set server socket to be monitored

        int activity = select(ssfd + 1, &mon, NULL, NULL, &timeout);

        if (activity == -1)
        {
            wlog(ERROR, "Select error: %s", strerror(errno));
            goto shutdown;
        }

        if (activity > 0)
        {
            if (FD_ISSET(ssfd, &mon))  // Check if server socket fd not in set for some reason
            {
                csfd = accept(ssfd, (struct sockaddr*) &csa, &csa_size);
                if (csfd == -1)
                {
                    wlog(ERROR, "Failed to accept connection. %d %s", errno, strerror(errno));
                    continue;
                }
            }

            wlog(INFO, "Request accepted. Connected to socket.");
            rb       = recv(csfd, buff, sizeof buff, 0);
            buff[rb] = '\0';  // set null terminator
            wlog(DEBUG, "Received %d bytes.", rb, buff);

            handle_user_request(csfd, buff);

            close(csfd);
        }

        if (shut_req)
            goto shutdown;
    }

shutdown:
    wlog(INFO, "Shutting down with %s value...", err ? "EXIT_FAILURE" : "EXIT_SUCCESS");
    close(ssfd);
    close(csfd);
    freeaddrinfo(sai);
    wlog_shutdown();

    if (err == 0)
        return EXIT_SUCCESS;
    return EXIT_FAILURE;
}
