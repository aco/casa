#pragma once

/// <summary>
/// Handles the reading of a socket's file descriptor
/// </summary>
/// <param name="message">Output eference to a message buffer.</param>
/// <param name="readfd">File descriptor.</param>
/// <returns>Buffer size of message.</returns>
int handle_read_descriptor(char **message, int readfd);

/// <summary>
/// Writes a given message to a socket file descriptor
/// </summary>
/// <param name="message">Message to write.</param>
/// <param name="client_socket">Destination socket.</param>
void handle_write_descriptor(const char *message, int target_socket);

/// <summary>
/// Opens a TCP socket through a given port.
/// </summary>
/// <param name="server_port">Port.</param>
/// <returns>Server socket identifier.</returns>
int start_server(int server_port);