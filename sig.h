/* ---------------------------------------------------------------------------------------------- */
/*                                        Signals Handling                                        */
/* ---------------------------------------------------------------------------------------------- */

#pragma once
#include <signal.h>

static volatile sig_atomic_t shut_req;  // Shutdown requested

// (Sig)nal (h)andler
void sigh(int signal);

// Signal handling startup procedure
int sigh_startup();
