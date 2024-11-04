#include "config.h"
#include "logging.h"
#include "net_utils.h"
#include "server.h"
#include "server.h"
#include "sig.h"

#include <stdlib.h>

// TODO fix all logging calls and make them standard and proper

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
