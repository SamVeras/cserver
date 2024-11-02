#pragma once

#define _STR(X) #X
#define STR(X)  _STR(X)

#define LOG_FILE_NAME server.log
#define SERVER_PORT   9902
#define BUFFER_SIZE   1024
#define BACKLOG       5
#define LOG_LEVEL     TRACE