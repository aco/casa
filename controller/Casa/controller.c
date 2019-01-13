#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <wiringPi.h>
#include <softPwm.h>

#include "controller.h"

#include "uthash.h"
#include "cJSON.h"

#include "command.h"
#include "socket.h"
#include "room.h"

struct Room *rooms = NULL;

/// <summary>
/// String represetnations of the NodeType enum.
/// </summary>
const char *node_type_ref[NODETEMPERATURE + 1] =
{
	"alarm",
	"door",
	"fireplace",
	"light",
	"rgb_light",
	"temperature"
};

/// <summary>
/// Adds a room to the home controller.
/// </summary>
/// <param name="name">Name of the room.</param>
/// <param name="thermalGPIO">GPIO pin # registered to a temperature sensor.</param>
void add_room(const char* name, uint8_t thermalGPIO)
{
	struct Room *room = malloc(sizeof(struct Room));

	room->thermalGPIO = thermalGPIO; // GPIO of a shared temperature sensor
	room->nodes = NULL;

	strcpy(room->name, name);

	HASH_ADD_STR(rooms, name, room);
}

/// <summary>
/// Adds a node to a room with a handler routine fired on change.
/// </summary>
/// <param name="room_name">Room name.</param>
/// <param name="name">Node name.</param>
/// <param name="type">Node type.</param>
/// <param name="gpio">Node GPIO pin (wiringPi).</param>
/// <param name="mode">Interrupt mode.</param>
/// <param name="change_handler">Handler routine.</param>
void add_node_to_room_handle(const char *room_name, const char *name, NodeType type, uint8_t gpio, int mode, void(change_handler)(void))
{
	add_node_to_room(room_name, name, type, gpio);

	pinMode(gpio, INPUT);

	if (wiringPiISR(gpio, mode, change_handler) < 0)
	{
		fprintf(stderr, "[!] ISR failure: %s\n", strerror(errno));
	}
}

/// <summary>
/// Adds a node supporting RGB channels to a room.
/// </summary>
/// <param name="room_name">Room name.</param>
/// <param name="name">Node name.</param>
/// <param name="r_gpio">The red GPIO pin.</param>
/// <param name="g_gpio">The green GPIO pin.</param>
/// <param name="b_gpio">The blue GPIO pin.</param>
/// <returns>Pointer to the new struct node - discardable.</returns>
struct Node *add_node_to_room_rgb(const char *room_name, const char *name, uint8_t rgb_gpio[3])
{
	struct Room *room;
	struct Node *node = malloc(sizeof(struct Node));

	node->value = 0;
	node->type = NODERGB;

	for (int i = 0; i < sizeof node->gpio; i++)
	{
		pinMode(rgb_gpio[i], SOFT_PWM_OUTPUT);
		softPwmCreate(rgb_gpio[i], node->value, 100);

		node->gpio[i] = rgb_gpio[i];
	}

	strcpy(node->name, name);

	HASH_FIND_STR(rooms, room_name, room);
	HASH_ADD_STR(room->nodes, name, node);

	return node;
}

/// <summary>
/// Adds a node to a room.
/// </summary>
/// <param name="room_name">Room name.</param>
/// <param name="name">Node name.</param>
/// <param name="type">Node type.</param>
/// <param name="gpio">Node GPIO pin (wiringPi).</param>
/// <returns>Pointer to the new struct node - discardable.</returns>
struct Node *add_node_to_room(const char *room_name, const char *name, NodeType type, uint8_t gpio)
{
	struct Room *room;
	struct Node *node = malloc(sizeof(struct Node));

	node->gpio[0] = gpio;
	node->value = 0;
	node->type = type;

	strcpy(node->name, name);

	HASH_FIND_STR(rooms, room_name, room);
	HASH_ADD_STR(room->nodes, name, node);

	switch (type)
	{
	case NODELIGHT:
	case NODEFIREPLACE:
		pinMode(gpio, SOFT_PWM_OUTPUT);
		softPwmCreate(gpio, node->value, 100);

		break;
	case NODETEMPERATURE:
		break;

	default:
		break;
	}

	return node;
}

/// <summary>
/// Finds and returns a node from a room within the house.
/// </summary>
/// <param name="room_name">Room name.</param>
/// <param name="name">Node name.</param>
/// <returns>Pointer to the new struct node - discardable and NULL if not found.</returns>
struct Node *find_node_from_room(const char *room_name, const char *name)
{
	struct Room *room;
	struct Node *node;

	if (room_name == NULL || name == NULL)
	{
		return NULL;
	}

	HASH_FIND_STR(rooms, room_name, room);
	HASH_FIND_STR(room->nodes, name, node);

	return node;
}

/// <summary>
/// Applies a given value to a node within a room.
/// </summary>
/// <param name="room_name">Room name.</param>
/// <param name="name">Node name.</param>
/// <param name="new_value">New value to apply.</param>
void apply_value_to_node(const char *room_name, const char *name, int new_value)
{
	struct Node *node = find_node_from_room(room_name, name);

	if (node == NULL)
	{
		return;
	}

	node->value = new_value;

	switch (node->type)
	{
	case NODELIGHT:
	case NODEFIREPLACE:
	{
		node->value = new_value;

		for (int i = 0; i < sizeof(node->gpio); i++)
		{
			if (node->gpio[0] != NULL)
			{
				softPwmWrite(node->gpio[i], node->value);
			}
		}

		break;
	}

	case NODERGB:
	{
		if (new_value < 1)
		{
			for (uint8_t i = 0; i < sizeof node->gpio; i++)
			{
				softPwmWrite(node->gpio[i], 0);
			}
		}

		softPwmWrite(node->gpio[0], (new_value >> 16) & 0xff);
		softPwmWrite(node->gpio[1], (new_value >> 8) & 0xff);
		softPwmWrite(node->gpio[2], (new_value) & 0xff);

		break;
	}

	case NODEALARM:
		break;

	case NODETEMPERATURE:
		break;

	default:
		break;
	}
}

/// <summary>
/// Probes the temperature from gpio.
/// </summary>
/// <param name="gpio">GPIO pin no.</param>
/// <returns>Temperature reading as float</returns>
float probe_temperature_from_gpio(int gpio)
{
	return 21.5;
}

/// <summary>
/// Adjusts all lighting within the house to a given brightness.
/// </summary>
/// <param name="brightness">Brightness as a percentage.</param>
void adjust_all_lighting(int brightness)
{
	struct Room *room, *tmpRoom;
	struct Node *node, *tmpNode;

	HASH_ITER(hh, rooms, room, tmpRoom)
	{
		HASH_ITER(hh, room->nodes, node, tmpNode)
		{
			if (node->type == NODELIGHT)
			{
				node->value = brightness;
				softPwmWrite(node->gpio[0], node->value);
			}
		}
	}

	printf("[!] Lighting set to %d%%\n", brightness);
}

/// <summary>
/// Builds and emits a JSON representation of the room structure to a socket.
/// </summary>
/// <param name="dispatch_socket">Client socket to dispatch.</param>
void emit_room_structure_json(int dispatch_socket)
{
	cJSON *root_object = cJSON_CreateObject();
	cJSON *payload_object = cJSON_CreateObject();
	cJSON *rooms_array = cJSON_AddArrayToObject(payload_object, "value");

	cJSON_AddNumberToObject(root_object, "type", CMDREPORT);
	cJSON_AddStringToObject(payload_object, "type", "rooms");

	struct Room *room, *tmpRoom;
	struct Node *node, *tmpNode;

	HASH_ITER(hh, rooms, room, tmpRoom)
	{
		cJSON *room_object = cJSON_CreateObject();
		cJSON *node_array = cJSON_AddArrayToObject(room_object, "nodes");

		cJSON_AddStringToObject(room_object, "name", room->name);
		cJSON_AddNumberToObject(room_object, "temperature",
			probe_temperature_from_gpio(room->thermalGPIO));

		HASH_ITER(hh, room->nodes, node, tmpNode)
		{
			cJSON *node_object = cJSON_CreateObject();

			cJSON_AddStringToObject(node_object, "name", node->name);
			cJSON_AddStringToObject(node_object, "type", node_type_ref[node->type]);
			cJSON_AddNumberToObject(node_object, "value", node->value);

			cJSON_AddItemToArray(node_array, node_object);
		}

		cJSON_AddItemToArray(rooms_array, room_object);
	}

	cJSON_AddItemToObject(root_object, "payload", payload_object);

	handle_write_descriptor(cJSON_PrintUnformatted(root_object), dispatch_socket);
	puts("[>] Room structure");
}
