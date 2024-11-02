#include "sig.h"
#include "logging.h"
#include <errno.h>
#include <string.h>

static struct sigaction sa;

void sigh(int signal)
{
    wlog(INFO, "Shutdown requested after receiving signal %d.", signal);
    shut_req = 1;
    return;
}

int sigh_startup()
{
    wlog(INFO, "Setting up signal handling...");
    sa.sa_handler = sigh;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1)  // Interactive attention signal
    {
        wlog(FATAL, "Failed to set SIGINT: (%d) %s", errno, strerror(errno));
        return -1;
    }

    if (sigaction(SIGTERM, &sa, NULL) == -1)  // Terminal request signal
    {
        wlog(FATAL, "Failed to set SIGTERM: (%d) %s", errno, strerror(errno));
        return -1;
    }

    wlog(INFO, "Signal handling startup complete.");
    return 0;
}