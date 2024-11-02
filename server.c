#include "server.h"
#include "logging.h"
#include "net_utils.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

void signal_handler(int signal)
{
    wlog(INFO, "Shutdown requested after receiving signal %d.", signal);
    shut_req = 1;
}

void handle_user_request(int client_socket, char* req)
{
    // TODO maybe sanitize path as only what's between / and end of string?

    char method[8], path[256];  // e.g. GET /index.html -> method = "GET", path = "/index.html"
    sscanf(req, "%s %s", method, path);

    wlog(INFO, "Request with method \"%s\" and path \"%s\"...", method, path);

    if (strstr(path, "..") || strstr(path, "//"))
    {
        wlog(ERROR, "Path traversal attempt detected: %s", path);
        return;
    }

    if (path[0] == '/')
    {
        memmove(path, path + 1, strlen(path));
    }

    wlog(DEBUG, "Changed path to \"%s\".", path);

    send_file(client_socket, path);

    return;
}

void send_file(int client_socket, const char path[])
{
    const char* body;
    const char* content_type = get_mime_type(path);
    wlog(TRACE, "Determined content-type to be %s.", content_type);

    wlog(INFO, "Opening file at %s and creating stream...", path);
    FILE* file = fopen(path, "rb");

    char header[512];
    if (!file)
    {
        wlog(ERROR, "Failed to open file. Sending 404 page to user.");

        body = "<html><body><h1>404 Not Found</h1><p>Sorry, not found.</p></body></html>";
        build_html_header(header, sizeof header, "404 Not Found", "text/html", strlen(body));

        send(client_socket, header, strlen(header), 0);
        send(client_socket, body, strlen(body), 0);
        return;
    }

    wlog(TRACE, "Seeking size of file...");
    fseek(file, 0, SEEK_END);          // Move file pointer to end of file
    long int file_size = ftell(file);  // Count # of number of bytes
    fseek(file, 0, SEEK_SET);          // Move file pointer back to beginning
    wlog(TRACE, "Size of file is %ld.", file_size);

    build_html_header(header, sizeof(header), "200 OK", content_type, file_size);

    wlog(INFO, "Sending header to user...");
    ssize_t sent = send(client_socket, header, strlen(header), 0);
    wlog(INFO, "%d bytes sent.", sent);

    char   buffer[1024];
    size_t bytes_read;

    wlog(INFO, "Reading file...");
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        wlog(INFO, "%lu bytes read.", bytes_read);
        sent = send(client_socket, buffer, bytes_read, 0);
        wlog(INFO, "%d bytes sent.", sent);
    }
    wlog(INFO, "Done reading file.");

    wlog(INFO, "Closing file.");
    fclose(file);
    return;
}
