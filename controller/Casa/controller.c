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
#include "profile.h"
#include "temperature.h"

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

void add_room(const char* name, uint8_t thermalGPIO)
{
	struct Room *room = malloc(sizeof(struct Room));

	room->thermalGPIO = thermalGPIO; // GPIO of a shared temperature sensor
	room->nodes = NULL;

	strcpy(room->name, name);

	HASH_ADD_STR(rooms, name, room);
}

void add_node_to_room_handle(const char *room_name, const char *name, NodeType type, uint8_t gpio, int mode, void(change_handler)(void))
{
	add_node_to_room(room_name, name, type, gpio);

	pinMode(gpio, INPUT);

	if (wiringPiISR(gpio, mode, change_handler) < 0)
	{
		fprintf(stderr, "[x] ISR failure: %s\n", strerror(errno));
	}
}

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

		for (uint8_t i = 0; i < sizeof(node->gpio); i++)
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
		for (uint8_t i = 0; i < sizeof(node->gpio); i++)
		{
			softPwmWrite(node->gpio[i], new_value >> 16 - (i * 8) & 0xff);
		}

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

float probe_temperature_from_gpio(int gpio)
{
	struct DHT22 *reading = read_temperature_celsius(gpio);

	if (reading != NULL)
	{
		return reading->temperature;
	}

	return 22.5; // retain for demo
}

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

void emit_room_structure_json(int dispatch_socket)
{
	cJSON *root_object = cJSON_CreateObject();
	cJSON *payload_object = cJSON_CreateObject();
	cJSON *rooms_array = cJSON_AddArrayToObject(payload_object, "value");

	cJSON_AddNumberToObject(root_object, "type", CMDREPORT);
	cJSON_AddStringToObject(payload_object, "type", "rooms");

	struct Room *room, *tmpRoom;
	struct Node *node, *tmpNode;

	struct Profile *profile = find_profile_from_client_socket(dispatch_socket);

	if (profile == NULL)
	{
		return;
	}

	HASH_ITER(hh, rooms, room, tmpRoom)
	{
		if (!is_room_accessible(profile, room->name))
		{
			continue;
		}

		cJSON *room_object = cJSON_CreateObject();
		cJSON *node_array = cJSON_AddArrayToObject(room_object, "nodes");

		cJSON_AddStringToObject(room_object, "name", room->name);
		cJSON_AddNumberToObject(room_object, "temperature",
			probe_temperature_from_gpio(room->thermalGPIO));

		HASH_ITER(hh, room->nodes, node, tmpNode)
		{
			if (!is_transaction_permissible(profile, room->name, node->name))
			{
				continue;
			}

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
