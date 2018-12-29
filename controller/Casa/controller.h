#include <stdio.h>
#include <wiringPi.h>

#include "node.h"

struct Room *rooms; // all rooms in the smart home

void add_room(const char* name, uint8_t thermalGPIO);

void add_node_to_room_handle(const char *room_name, const char *name, NodeType type, uint8_t gpio, int mode, void(change_handler)(void));

struct Node *add_node_to_room(const char *room_name, const char *name, NodeType type, uint8_t gpio);
struct Node *add_node_to_room_rgb(const char *room_name, const char *name, uint8_t rgb_gpio[3]);

struct Node *find_node_from_room(const char *room_name, const char *name);
void apply_value_to_node(const char *room_name, const char *name, int value);

float probe_temperature_from_gpio(int gpio);
void adjust_all_lighting(int brightness);

void emit_room_structure_json(int dispatch_socket);