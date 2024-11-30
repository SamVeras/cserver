#include "config.h"
#include "logging.h"
#include "net_utils.h"
#include "server.h"
#include "server.h"
#include "sig.h"

#include <stdlib.h>

// TODO use magic numbers to determine mime type
// TODO make my own tree implementation or include one
// TODO optimize snprintf calls by storing actual paths
// TODO remove necessity of having a favicon file if none is found (maybe have a default one?)
int main(int argc, char const* argv[])
{
    if (config_server(argc, argv) == EXIT_FAILURE)
        return EXIT_FAILURE;

    if (server_start() == EXIT_FAILURE)
        return server_shutdown();

    if (server_run() == EXIT_FAILURE)
        fprintf(stderr, "Server exited with error.\n");

    return server_shutdown();
}