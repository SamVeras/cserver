/* ---------------------------------------------------------------------------------------------- */
/*                                      Server functionality                                      */
/* ---------------------------------------------------------------------------------------------- */

#pragma once
#include <signal.h>

static volatile sig_atomic_t shut_req = 0;  // Shutdown requested

void signal_handler(int signal);
void handle_user_request(int client_socket, char* req);
void send_file(int client_socket, const char path[]);