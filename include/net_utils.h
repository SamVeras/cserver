/* -------------------------------------------------------------------------- */
/*                              Network utilities                             */
/* -------------------------------------------------------------------------- */

#pragma once
#include <stddef.h>
#include <time.h>

/**
 * @brief Extracts the file extension from a given path and returns the corresponding mime type.
 *
 * This function analyzes the file extension of the provided path and returns a string representing
 * the appropriate mime type. If no extension is found or if the extension is unknown,
 * it defaults to "application/octet-stream".
 *
 * @param path The file path to analyze.
 * @return The mime type as a string. */
const char* get_mime_type(const char path[]);

/**
 * @brief Write HTML header based on arguments to provided buffer.
 * This function writes a complete HTTP header to the provided buffer based
 * on the given arguments.
 * @param header The buffer to write the header to.
 * @param header_size The maximum size of the header buffer.
 * @param status The HTTP status code to include in the header.
 * @param content_type The mime type of the content.
 * @param content_length The length of the content to be sent. */
void build_html_header(char*       header,
                       size_t      header_size,
                       const char* status,
                       const char* content_type,
                       size_t      content_length);

/**
 * @brief Center a string in a buffer by padding with spaces.
 * @param[in] text The string to be centered.
 * @param[out] buff The buffer to center the string in.
 * @param[in] len The length of the buffer.
 * If the length of the string is greater than or equal to the length of
 * the buffer, the function does nothing. */
void center_text(const char* text, char* buff, size_t len);

/**
 * @brief Retrieves the current time as a string and stores it in the provided buffer.
 * @details The string is in the format "DD/MM/YYYY HH:MM:SS".
 * @param buffer The character buffer to store the time string in.
 * @param buff_size The size of the buffer. */
void get_current_time(char buffer[], size_t buff_size);

/**
 * @brief Truncates a string at the first newline character and appends a newline
 *        if the string doesn't already end with one.
 * @param str The string to modify.
 * @param str_len The length of the string.
 * The string is modified in place. If the string already has a newline, it's
 * truncated at that point. If the string is too long to append a newline, the
 * string is left unchanged.
 * @note This function does not check if the string is null-terminated. */
void format_log_message(char str[], size_t str_len);

/**
 * @brief Convert size in bytes to a human-readable string and store in the buffer.
 * The buffer must have enough space to store the formatted string.
 * The string is formatted as a double (with 2 decimal places) followed by a unit string.
 * The unit string is selected based on the magnitude of the size: B, KB, MB, GB, TB.
 * The function rounds to the nearest hundredth.
 * @param bytes The size in bytes to be converted.
 * @param buffer The character buffer to store the human-readable string in.
 * @param buff_len The size of the buffer. */
void human_readable_size(long bytes, char buffer[], size_t buff_len);

/**
 * @brief Format a log message with transfer data.
 * @param read     The total number of bytes read from the client.
 * @param sent     The total number of bytes sent to the client.
 * @param transfers The number of transfers done.
 * This function formats a log message with the total number of bytes read and
 * sent, as well as the number of transfers done. It also logs the transfer data
 * in a human-readable format. */
void log_transfer_data(long read, long sent, long unsigned transfers);

/**
 * @brief Decodes a URL-encoded string.
 * This function decodes a URL-encoded input string `src` and stores the result
 * in `dest`. It handles percent-encoded characters and regular characters.
 * @param dest The destination buffer to store the decoded string.
 * @param dest_size The size of the destination buffer.
 * @param src The source URL-encoded string.
 * @param src_size The size of the source string.
 * @return EXIT_SUCCESS on successful decoding, EXIT_FAILURE if an error occurs. */
int url_decode(char* dest, size_t dest_size, const char* src, size_t src_size);