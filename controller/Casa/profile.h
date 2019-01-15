#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "uthash.h"

#define MAX_SET_SIZE 16

struct Profile *profiles;

/// <summary>
/// struct of a permission rule, its presence indivactes permission granted
/// </summary>
struct RoomPermission
{
	char room_name[32];
	char *nodes[MAX_SET_SIZE];

	UT_hash_handle hh;
};

/// <summary>
/// struct of a user-specific permission profile
/// </summary>
struct Profile
{
	char identifier[16];
	uint8_t client_socket_identifier;

	struct RoomPermission *permissions;

	UT_hash_handle hh;
};

bool is_casa_profile_file(const char *d_name);

bool is_transaction_permissible_from_socket(const int client_socket_identifier, const char *room_name, const char *node_name);
bool is_transaction_permissible(struct Profile *profile, const char *room_name, const char *node_name);

bool is_room_accessible(struct Profile *profile, const char *room_name);

struct Profile *find_profile_from_client_socket(const int client_socket_identifier);
void bind_client_socket_to_profile(const char *profile_identifier, const int client_socket_identifier);

char *profile_identifier_from_directory(const char *d_name);
int gather_permissions(void);