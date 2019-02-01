#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024

int handle_read_descriptor(char **message, int readfd)
{
	*message = (char *)malloc(BUFFER_SIZE * sizeof(char));
	memset(*message, 0, BUFFER_SIZE);

	int bytes = 0;
	bytes = recv(readfd, *message, BUFFER_SIZE, 0);

	if (bytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
	{
		puts("[-] Reading without data\n");
	}

	return bytes;
}

void handle_write_descriptor(const char *message, int client_socket)
{
	int buffer_length = strlen(message);
	char buffer[buffer_length];
	buffer[0] = '\0';

	memset(buffer, 0, sizeof(buffer));

	sprintf(buffer, "%s", message);
	send(client_socket, &buffer, buffer_length, 0);
}

int start_server(int server_port)
{
	struct sockaddr_in server_addrin;

	const char *socket_optval = "yes"; 
	int server_socket;

	memset(&server_addrin, 0, sizeof(server_addrin));

	server_addrin.sin_family = AF_INET;
	server_addrin.sin_port = htons(server_port);
	server_addrin.sin_addr.s_addr = htonl(INADDR_ANY);

	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&socket_optval, sizeof(socket_optval));
	fcntl(server_socket, F_SETFL, O_NONBLOCK);

	if (bind(server_socket, (struct sockaddr *)&server_addrin, sizeof(server_addrin)) == -1)
	{
		perror("bind() failed");
	}

	listen(server_socket, 1);

	printf("[!] Listening on port %d\n", ntohs(server_addrin.sin_port));

	return server_socket;
}