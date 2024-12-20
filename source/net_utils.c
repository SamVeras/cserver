#include "net_utils.h"
#include "logging.h"
#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* -------------------------------------------------------------------------- */

/**
 * @brief Current local time.
 * This variable is used to store the current local time, retrieved using the
 * time() function, as seconds since Epoch (Jan 1st, 1970 00:00 UTC). */
static time_t clt;

/**
 * @brief Current time structure.
 * Store the broken-down current time structure which is retrieved using the
 * localtime() function with the clt variable. */
static struct tm* ct;

/* -------------------------------------------------------------------------- */

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
    if (strcmp(ext, ".json") == 0)
        return "application/json";
    if (strcmp(ext, ".xml") == 0)
        return "application/xml";
    if (strcmp(ext, ".svg") == 0)
        return "image/svg+xml";
    if (strcmp(ext, ".pdf") == 0)
        return "application/pdf";
    if (strcmp(ext, ".mp3") == 0)
        return "audio/mpeg";
    if (strcmp(ext, ".mp4") == 0)
        return "video/mp4";
    if (strcmp(ext, ".woff") == 0 || strcmp(ext, ".woff2") == 0)
        return "font/woff";
    if (strcmp(ext, ".ttf") == 0)
        return "font/ttf";
    if (strcmp(ext, ".ico") == 0)
        return "image/x-icon";
    if (strcmp(ext, ".zip") == 0)
        return "application/zip";
    if (strcmp(ext, ".csv") == 0)
        return "text/csv";

    return "application/octet-stream";  // Default for unknown types
}

/* -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- */

void get_current_time(char buffer[], size_t buff_size)
{
    if (buff_size < 20)
    {
        wlog(ERROR, "Buffer size %zu is too small for current time string.", buff_size);
        return;
    }

    // Get current time and convert it to a tm struct
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

/* -------------------------------------------------------------------------- */

void format_log_message(char str[], size_t str_len)
{
    // Check if message has newline
    char* new_line_pos = strchr(str, '\n');

    // Truncate at first newline
    if (new_line_pos)
    {
        *(new_line_pos) = '\0';
        return;
    }

    if (str_len < 2)
        return;

    str[str_len - 1] = '\n';
    str[str_len]     = '\0';

    return;
}

/* -------------------------------------------------------------------------- */

void human_readable_size(long bytes, char buffer[], size_t buff_len)
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    size_t      unit    = 0;
    double      b       = (double) bytes;

    while (b >= 1024 && unit < (sizeof units / sizeof(char*)))
    {
        b /= 1024;
        unit++;
    }

    snprintf(buffer, buff_len, "%.2f %s", b, units[unit]);

    return;
}

/* -------------------------------------------------------------------------- */

void log_transfer_data(long read, long sent, long unsigned transfers)
{
    char read_str[32], sent_str[32];
    human_readable_size(read, read_str, sizeof read_str);
    human_readable_size(sent, sent_str, sizeof sent_str);
    wlog(INFO, "%lu transfers done. %s sent. Total read: %s.", transfers, sent_str, read_str);

    return;
}

/* -------------------------------------------------------------------------- */

int url_decode(char* dest, size_t dest_size, const char* src, size_t src_size)
{
    if (dest == NULL || src == NULL || dest_size == 0 || src_size == 0)
        return EXIT_FAILURE;

    size_t d = 0, s = 0;

    // Iterate over the source string
    while (s < src_size)
    {
        // Handle percent-encoded characters
        if (src[s] == '%' && s + 2 < src_size && isxdigit(src[s + 1]) && isxdigit(src[s + 2]))
        {
            char hex[3];
            hex[0] = src[s + 1];
            hex[1] = src[s + 2];
            hex[2] = '\0';

            // Convert hex to character and store in destination
            dest[d++] = (char) strtol(hex, NULL, 16);
            s += 3;
            continue;
        }

        // Handle regular characters
        if (d >= dest_size - 1)
        {
            dest[d] = '\0';
            return EXIT_FAILURE;
        }

        dest[d++] = src[s++];
    }

    // Null-terminate the destination string
    if (d < dest_size)
        dest[d] = '\0';
    else
        dest[dest_size - 1] = '\0';

    return EXIT_SUCCESS;
}

/* -------------------------------------------------------------------------- */

// Inútil.

// int parse_ip_port_arg(const char* arg, char* ip, size_t ip_size, char* port, size_t port_size)
// {
//     const char* separator = strchr(arg, ':');  // Get the : separator position
//     if (!separator)                            // There is no separator
//     {
//         fprintf(stderr, "Expected proxy argument to be in format IP:PORT (%s).\n", arg);
//         return EXIT_FAILURE;
//     }
//
//     // Check IP length
//     size_t ip_len = separator - arg;
//     if (ip_len > ip_size)
//     {
//         // Subtracting separator position from arg position gives length
//         fprintf(stderr, "IP too long! (%s).\n", arg);
//         return EXIT_FAILURE;
//     }
//
//     strncpy(ip, arg, ip_len);  // Copy IP to provided IP buffer
//     ip[ip_len] = '\0';         // Add null terminator
//
//     // Check port length
//     if (strlen(separator + 1) > port_size)
//     {
//         fprintf(stderr, "PORT too long! (%s).\n", arg);
//         return EXIT_FAILURE;
//     }
//
//     strncpy(port, separator + 1, port_size);  // Copy PORT to provided port buffer
//     port[strlen(separator + 1)] = '\0';       // Should already be null terminated but just in
//     case
//
//     // Now we need to validate IP and PORT
//     if (inet_pton(AF_INET, ip, NULL) <= 0)
//     {
//         fprintf(stderr, "Invalid IP! (%s).\n", arg);
//         return EXIT_FAILURE;
//     }
//
//     errno        = 0;
//     int port_int = strtol(port, NULL, 10);  // Check if PORT is really numeric
//     if (errno != 0)
//     {
//         fprintf(stderr, "Invalid PORT! (%s) errno: %d %s.\n", arg, errno, strerror(errno));
//         return EXIT_FAILURE;
//     }
//
//     if (port_int < 0 || port_int > 65535)  // Check if PORT is within valid bounds
//     {
//         fprintf(stderr, "Port out of bounds! (%s).\n", arg);
//         return EXIT_FAILURE;
//     }
//
//     return EXIT_SUCCESS;
// }