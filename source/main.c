#include "config.h"
#include "logging.h"
#include "net_utils.h"
#include "server.h"
#include "server.h"
#include "sig.h"

#include <stdlib.h>

int main(int argc, char const* argv[])
{
    if (config_server(argc, argv))
        return EXIT_FAILURE;

    if (server_start() == EXIT_FAILURE)
        return server_shutdown();

    if (server_run() == EXIT_FAILURE)
        fprintf(stderr, "Server exited with error.\n");

    return server_shutdown();
}