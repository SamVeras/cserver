#include "config.h"
#include "logging.h"
#include "net_utils.h"
#include "server.h"
#include "server.h"
#include "sig.h"

#include <stdlib.h>

// ---------------------------------------------------------------------------------------------- //

// TODO fix all logging calls and make them standard and proper

// int main()
// TODO use arg for port and buffer size?
// server -p <port> -b <buffer_size> -l <log_level> -c <backlog> -f <log_file>
int main(int argc, char const* argv[])
{
    if (config_server(argc, argv))
        return EXIT_FAILURE;

    server_config_show();

    if (server_start() == EXIT_FAILURE)
        return server_shutdown();

    server_run();

    return server_shutdown();
}
