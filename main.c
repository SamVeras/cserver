#include "config.h"
#include "logging.h"
#include "net_utils.h"
#include "server.h"
#include "server.h"
#include "sig.h"

// ---------------------------------------------------------------------------------------------- //

// TODO use arg for port and buffer size?
// int main(int argc, char const* argv[])

// TODO fix all logging calls and make them standard and proper

// TODO extract functions from this mess, move most of it to server.h/.c
int main()
{
    server_start();
    server_run();
    return server_shutdown();
}
