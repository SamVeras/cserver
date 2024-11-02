/* ---------------------------------------------------------------------------------------------- */
/*                                      Server functionality                                      */
/* ---------------------------------------------------------------------------------------------- */

#pragma once

void handle_user_request(int client_socket, char* req);
void send_file(int client_socket, const char path[]);