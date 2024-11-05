/* ---------------------------------------------------------------------------------------------- */
/*                                        Network utilities                                       */
/* ---------------------------------------------------------------------------------------------- */

#pragma once
#include <stddef.h>
#include <time.h>

// Extract file extension and use it to return an appropriate mime_type.
const char* get_mime_type(const char path[]);

// Write HTML header based on arguments to provided buffer.
void build_html_header(char*       header,
                       size_t      header_size,
                       const char* status,
                       const char* content_type,
                       size_t      content_length);

// Center text in buffer relative to length provided.
void center_text(const char* text, char* buff, size_t len);

// Formats the current time in the format 'DD/MM/YYYY HH:MM:SS' to the buffer.
// (buff_size >= 20)
void get_current_time(char buffer[], size_t buff_size);

// Truncates newlines or appends a newline if it's missing.
void format_log_message(char str[], size_t str_len);

// Convert size in bytes to a human-readable string and store in the buffer.
void human_readable_size(long bytes, char buffer[], size_t buff_len);

// Print to log how many bytes were sent, in human readable format.
void log_transfer_data(long read, long sent, long unsigned transfers);