/* -------------------------------------------------------------------------- */
/*                              Signals Handling                              */
/* -------------------------------------------------------------------------- */

#pragma once
#include <signal.h>

/**
 * @brief A flag to request the server to shut down.
 * This variable is declared volatile and accessed atomically because it is
 * accessed from both the main thread and the signal handler.
 * @see sigh_startup()
 * @see sigh() */
extern volatile sig_atomic_t shut_req;

/**
 * @brief Signal handler function.
 * This function is triggered when a signal is received. It logs the signal and
 * sets the shutdown request flag to indicate that a shutdown is needed.
 * @param signal The signal number that was received. (An ISO C99 / POSIX signal) */
void sigh(int signal);

/**
 * @brief Signal handling startup function.
 * This function should be called once and only once.  It sets up signal
 * handling by registering the sigh() function to be called when SIGINT or
 * SIGTERM is received.
 * @returns 0 on success, -1 on failure. */
int sigh_startup();
