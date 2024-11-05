#include "sig.h"
#include "logging.h"
#include <errno.h>
#include <string.h>

static struct sigaction sa;
volatile sig_atomic_t   shut_req = 0;

void sigh(int signal)
{
    wlog(INFO, "Shutdown requested after receiving signal %d.", signal);
    shut_req = 1;
    // __sync_synchronize();
    wlog(TRACE, "Shutdown_request is now %d", shut_req);
    // return;
}

// TODO Fit return into standard success / failure
int sigh_startup()
{
    wlog(INFO, "Setting up signal handling...");
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = sigh;
    sa.sa_flags   = SA_RESTART;

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

    wlog(TRACE, "Signal handling startup complete.");
    return 0;
}