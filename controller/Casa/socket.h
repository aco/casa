#pragma once

int start_server(int server_port);

int handle_read_descriptor(char **message, int readfd);
void handle_write_descriptor(const char *message, int target_socket);
