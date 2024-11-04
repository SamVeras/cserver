/* ---------------------------------------------------------------------------------------------- */
/*                                      Server functionality                                      */
/* ---------------------------------------------------------------------------------------------- */

#pragma once

int server_start();
int server_run();
int server_shutdown();

int handle_user_request(int client_socket, char* req);
int send_file(int client_socket, const char path[]);