#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>

#include <wiringPi.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#include "cJSON.h"

#include "socket.h"
#include "controller.h"
#include "system.h"
#include "command.h"
#include "blockchain.h"

#define	DAYLIGHT_PIN 28

bool is_night_time;

int server_socket;
int active_socket;


/// <summary>
/// Interrupt service routine for PIR sensors.
/// </summary>
void did_detect_motion_signal(void)
{
	puts("motion detected");
}

/// <summary>
/// Interrupt service routine for light sensors. Emits a daylight report to client and adjusts home lighting.
/// </summary>
void did_detect_daylight_change(void)
{
	int result = digitalRead(DAYLIGHT_PIN);

	if (result != is_night_time)
	{
		is_night_time = result;

		cJSON *root_object = cJSON_CreateObject();
		cJSON *payload_object = cJSON_CreateObject();

		cJSON_AddNumberToObject(root_object, "type", CMDNOTIFICATION);

		cJSON_AddStringToObject(payload_object, "type", "daylight");
		cJSON_AddBoolToObject(payload_object, "value", is_night_time);

		cJSON_AddItemToObject(root_object, "payload", payload_object);

		handle_write_descriptor(cJSON_PrintUnformatted(root_object), active_socket);
		printf("[>] Daylight shift (%s)\n", is_night_time ? "night" : "day");

		if (is_night_time) // bring exterior lights to 40%
		{
			apply_value_to_node("home", "porch_light", 40);
			apply_value_to_node("garage", "entrance_light", 40);

		}
		else // dawn - douse all lights
		{
			adjust_all_lighting(0);
		}

		emit_room_structure_json(active_socket); // reflect new values in client

		cJSON_Delete(root_object);
	}
}

/// <summary>
/// Registers interrupts to home sensors.
/// </summary>
int arm_sensors(void)
{
	pinMode(DAYLIGHT_PIN, INPUT);

	return wiringPiISR(DAYLIGHT_PIN, INT_EDGE_BOTH, &did_detect_daylight_change);
}

/// <summary>
/// Populates home with rooms and nodes within.
/// </summary>
void populate_rooms(void)
{
	uint8_t rgb_gpio[3] = { 23, 24, 25 };

	add_room("home", 1);
	add_node_to_room_handle("home", "main_alarm", NODEALARM, 26, INT_EDGE_RISING, &did_detect_motion_signal);

	add_node_to_room("home", "porch_light", NODELIGHT, 7);
	add_node_to_room_rgb("home", "side lighting", rgb_gpio);
	add_node_to_room("home", "central_heating", NODETEMPERATURE, 12);

	add_room("lounge", 1);
	add_node_to_room("lounge", "ceiling_light", NODELIGHT, 6);
	add_node_to_room("lounge", "fireplace", NODEFIREPLACE, 4);

	add_room("kitchen", 1);
	add_node_to_room("kitchen", "ceiling_light", NODELIGHT, 2);

	add_room("bedroom", 1);
	add_node_to_room("bedroom", "ceiling_light", NODELIGHT, 5);

	add_room("garage", 1);
	add_node_to_room("garage", "entrance_light", NODELIGHT, 3);
}

/// <summary>
/// Main controller body. Receives and processes messages from socket connections.
/// </summary>
int run_server(void) // credit to: http://beej.us/guide/bgnet/html/single/bgnet.html
{
	int fdmax = -1;
	int i = 0;
	int client_socket = -1;

	fd_set read_fds, active_fds;

	FD_ZERO(&read_fds);
	FD_ZERO(&active_fds);
	FD_SET(server_socket, &active_fds);

	if (server_socket > fdmax)
	{
		fdmax = server_socket;
	}

	while (1)
	{
		memcpy(&read_fds, &active_fds, sizeof(active_fds));
		select(fdmax + 1, &read_fds, NULL, NULL, NULL);

		for (i = 0; i <= fdmax; i++) // iterate connections
		{
			if (FD_ISSET(i, &read_fds))
			{
				if (i == server_socket) // new client connection
				{
					client_socket = accept(server_socket, NULL, NULL);

					if (client_socket == -1)
					{
						perror("[-] accept");
						continue;
					}
					else
					{
						FD_SET(client_socket, &active_fds);

						if (client_socket > fdmax)
						{
							fdmax = client_socket;
						}

						printf("[+] Client %d online\n", fdmax);
					}
				}
				else
				{
					active_socket = i;

					char *message = NULL;
					int n = handle_read_descriptor(&message, active_socket);

					if (n == 0) // nothing received - close socket
					{
						printf("[!] Client %d offline\n", client_socket);
						shutdown_socket(active_socket, &active_fds);

						continue;
					}

					cJSON *root_object = cJSON_Parse(message);
					cJSON *payload_object = cJSON_GetObjectItem(root_object, "payload");

					switch ((Command)(cJSON_GetObjectItem(root_object, "type")->valueint))
					{
					case CMDNODE:
					{
						const char *room = cJSON_GetObjectItem(payload_object, "room")->valuestring;
						const char *node = cJSON_GetObjectItem(payload_object, "node")->valuestring;
						const int value = cJSON_GetObjectItem(payload_object, "value")->valueint;

						if (permit_proposed_transaction(node, room, value, active_socket, false))
						{
							apply_value_to_node(room, node, value);
							printf("[<] Set %s (%s) to %d\n", node, room, value);
						}
						else
						{
							printf("[!] Rejected %s (%s) to %d - insufficient permissions (client %d)\n", node, room, value, active_socket);
						}

						break;
					}
					case CMDREPORT:
					{
						const char *report_type = cJSON_GetObjectItem(payload_object, "type")->valuestring;
						const bool is_batch_request = strcmp(report_type, "batch") == 0;

						if (is_batch_request)
						{
							puts("[!] Processing batch request");
						}

						if (is_batch_request || strcmp(report_type, "structure") == 0)
						{
							emit_room_structure_json(active_socket);
						}

						if (is_batch_request || strcmp(report_type, "system") == 0)
						{
							emit_system_report_json(active_socket);
						}
					}
					default:
						break;
					}
				}
			}
		}
	}

	return 0;
}

/// <summary>
/// Terminates a socket and its descriptor.
/// </summary>
/// <param name="client_socket">The client socket.</param>
/// <param name="active_fds">The active FDS.</param>
void shutdown_socket(int client_socket, fd_set *active_fds)
{
	FD_CLR(client_socket, active_fds);
	close(client_socket);
}

/// <summary>
/// Entry point.
/// </summary>
/// <param name="argc">Passed arg count</param>
/// <param name="argv">Passed arg vector</param>
/// <returns></returns>
int main(int argc, char *argv[])
{
	if (puts("Casa 1.0\n\n[~] Initialising GPIO pins...") && wiringPiSetup() != 0)
	{
		return -1;
	}

	puts("[~] Mapping appliances...");
	populate_rooms();

	if (puts("[~] Arming sensors...") && arm_sensors() != 0)
	{
		return -1;
	}

	if (puts("[~] Building blockchain...") && construct_blockchain() != 0)
	{
		return -1;
	}

	puts("[~] Opening channel...");
	server_socket = start_server(4045); // atoi(argv[1])

	return run_server();
}