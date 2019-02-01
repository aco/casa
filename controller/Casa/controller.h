#pragma once

#include <stdio.h>
#include <wiringPi.h>

#include "node.h"

struct Room *rooms; // all rooms in the smart home

/// <summary>
/// Adds a room to the home controller.
/// </summary>
/// <param name="name">Name of the room.</param>
/// <param name="thermalGPIO">GPIO pin # registered to a temperature sensor.</param>
void add_room(const char* name, uint8_t thermalGPIO);

/// <summary>
/// Adds a node to a room with a handler routine fired on change.
/// </summary>
/// <param name="room_name">Room name.</param>
/// <param name="name">Node name.</param>
/// <param name="type">Node type.</param>
/// <param name="gpio">Node GPIO pin (wiringPi).</param>
/// <param name="mode">Interrupt mode.</param>
/// <param name="change_handler">Handler routine.</param>
void add_node_to_room_handle(const char *room_name, const char *name, NodeType type, uint8_t gpio, int mode, void(change_handler)(void));

/// <summary>
/// Adds a node to a room.
/// </summary>
/// <param name="room_name">Name of the room.</param>
/// <param name="name">Node name.</param>
/// <param name="type">Node type.</param>
/// <param name="gpio">Node GPIO pin (wiringPi).</param>
/// <returns>Pointer to the new struct node - discardable.</returns>
struct Node *add_node_to_room(const char *room_name, const char *name, NodeType type, uint8_t gpio);

/// <summary>
/// Adds an RGB node to a room.
/// </summary>
/// <param name="room_name">Name of the room.</param>
/// <param name="name">The name.</param>
/// <param name="rgb_gpio">The RGB gpios.</param>
/// <returns></returns>
struct Node *add_node_to_room_rgb(const char *room_name, const char *name, uint8_t rgb_gpio[3]);

/// <summary>
/// Finds and returns a node from a room within the house.
/// </summary>
/// <param name="room_name">Room name.</param>
/// <param name="name">Node name.</param>
/// <returns>Pointer to the new struct node - discardable and NULL if not found.</returns>
struct Node *find_node_from_room(const char *room_name, const char *name);

/// <summary>
/// Applies a given value to a node within a room.
/// </summary>
/// <param name="room_name">Room name.</param>
/// <param name="name">Node name.</param>
/// <param name="new_value">New value to apply.</param>
void apply_value_to_node(const char *room_name, const char *name, int value);

/// <summary>
/// Probes the temperature from gpio.
/// </summary>
/// <param name="gpio">GPIO pin no.</param>
/// <returns>Temperature reading as float</returns>
float probe_temperature_from_gpio(int gpio);

/// <summary>
/// Adjusts all lighting within the house to a given brightness.
/// </summary>
/// <param name="brightness">Brightness as a percentage.</param>
void adjust_all_lighting(int brightness);

/// <summary>
/// Builds and emits a JSON representation of the room structure to a socket.
/// </summary>
/// <param name="dispatch_socket">Client socket to dispatch.</param>
void emit_room_structure_json(int dispatch_socket);