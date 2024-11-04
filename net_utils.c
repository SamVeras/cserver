#include "net_utils.h"
#include "logging.h"
#include "config.h"
#include <string.h>

static time_t     clt;  // Current local time
static struct tm* ct;   // Current time structure (broken-down)

const char* get_mime_type(const char path[])
{
    wlog(TRACE, "Getting mime-type of path %s...", path);

    const char* ext = strrchr(path, '.');
    wlog(TRACE, "Determined extension to be %s...", ext);

    if (!ext)
        return "application/octet-stream";  // Default binary type
    if (strcmp(ext, ".html") == 0)
        return "text/html";
    if (strcmp(ext, ".css") == 0)
        return "text/css";
    if (strcmp(ext, ".js") == 0)
        return "application/javascript";
    if (strcmp(ext, ".png") == 0)
        return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(ext, ".gif") == 0)
        return "image/gif";
    if (strcmp(ext, ".txt") == 0)
        return "text/plain";

    return "application/octet-stream";  // Default for unknown types
}

void build_html_header(char*       header,
                       size_t      header_size,
                       const char* status,
                       const char* content_type,
                       size_t      content_length)
{
    wlog(TRACE, "Building HTML header. (%s, %s, %lu)", status, content_type, content_length);
    snprintf(header,
             header_size,
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n",
             status,
             content_type,
             content_length);
}

void center_text(const char* text, char* buff, size_t len)
{
    if (strlen(text) >= len)
        return;

    int pad = len - strlen(text);
    int rp  = pad / 2;
    int lp  = pad - rp;

    snprintf(buff, len + 1, "%*s%s%*s", lp, "", text, rp, "");

    return;
}

void get_current_time(char buffer[], size_t buff_size)
{
    if (buff_size < 20)
        return;

    clt = time(NULL);
    ct  = localtime(&clt);

    snprintf(buffer,
             buff_size,
             "%02d/%02d/%04d %02d:%02d:%02d",
             ct->tm_mday,
             ct->tm_mon + 1,
             ct->tm_year + 1900,
             ct->tm_hour,
             ct->tm_min,
             ct->tm_sec);
}

void format_log_message(char str[], size_t str_len)
{
    // Check if message has newline
    char* new_line_pos = strchr(str, '\n');

    // TODO maybe dont truncate at first newline? replace them with whitespace?
    // Truncate at first newline
    if (new_line_pos)
    {
        *(strchr(str, '\n') + 1) = '\0';
        return;
    }

    // If no newline and there's room, append newline
    if (str_len < (size_t) (BUFFER_SIZE - 1))
    {
        str[str_len]     = '\n';
        str[str_len + 1] = '\0';
    }
}