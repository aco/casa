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
	char identifier[MAX_SET_SIZE];
	uint8_t client_socket_identifier;

	struct RoomPermission *permissions;

	UT_hash_handle hh;
};

/// <summary>
/// Determines whether a file bears the Casa profile extension.
/// </summary>
/// <param name="d_name">Directory target.</param>
/// <returns>
///   <c>true</c> if the file ends with the Casa profile extension.
/// </returns>
bool is_casa_profile_file(const char *d_name);

/// <summary>
/// Evaluates the subject of a transaction against against a user's permission ruleset, identified by associated socket.
/// </summary>
/// <param name="client_socket_identifier">Socket [making the transaction].</param>
/// <param name="room_name">Name of the room.</param>
/// <param name="node_name">Name of the node.</param>
/// <returns>
///   <c>true</c> if the permission exists for the profile; otherwise, <c>false</c> or if no profile was found.
/// </returns>
bool is_transaction_permissible_from_socket(const int client_socket_identifier, const char *room_name, const char *node_name);

/// <summary>
/// Evaluates the subject of a transaction against against a user's permission ruleset.
/// </summary>
/// <param name="profile">Profile [making the transaction].</param>
/// <param name="room_name">Name of the room.</param>
/// <param name="node_name">Name of the node.</param>
/// <returns>
///   <c>true</c> if the permission exists for the profile; otherwise, <c>false</c>.
/// </returns>
bool is_transaction_permissible(struct Profile *profile, const char *room_name, const char *node_name);

/// <summary>
/// Determines whether a room is accessible (a permission set exists for the profile.
/// </summary>
/// <param name="profile">The profile.</param>
/// <param name="room_name">Name of the room.</param>
/// <returns>
///   <c>true</c> if accessible for the specified profile; otherwise, <c>false</c>.
/// </returns>
bool is_room_accessible(struct Profile *profile, const char *room_name);

/// <summary>
/// Associates a socket address with a profile. Can be overwritten.
/// </summary>
/// <param name="profile_identifier">The profile name.</param>
/// <param name="client_socket_identifier">Socket address.</param>
/// <returns>
///   Corresponding <c>struct Profile</c>, else NULL.
/// </returns>
struct Profile *find_profile_from_client_socket(const int client_socket_identifier);

/// <summary>
/// Associates a socket address with a profile. Can be overwritten.
/// </summary>
/// <param name="profile_identifier">The profile name.</param>
/// <param name="client_socket_identifier">Socket address.</param>
void bind_client_socket_to_profile(const char *profile_identifier, const int client_socket_identifier);

/// <summary>
/// Extracts the profile name from the directory target.
/// </summary>
/// <param name="d_name">Directory target.</param>
/// <returns>Profile name.</returns>
char *profile_identifier_from_directory(const char *d_name);

/// <summary>
/// Loads permissioning system with profiles from disk.
/// </summary>
/// <returns>0 for success, -1 for failure.</returns>
int gather_permissions(void);