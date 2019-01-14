#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>

#include "cJSON.h"
#include "uthash.h"

#include "profile.h"

#define PROFILE_DIRECTORY "./profiles/"
#define PROFILE_EXTENSION ".casap"

#define MAX_PROFILE_CHAR 1024

/// <summary>
/// Determines whether a file bears the Casa profile extension.
/// </summary>
/// <param name="d_name">Directory target.</param>
/// <returns>
///   <c>true</c> if the file ends with the Casa profile extension.
/// </returns>
bool is_casa_profile_file(const char *d_name)
{
	const size_t ext_len = strlen(PROFILE_EXTENSION);
	const size_t str_len = strlen(d_name);

	return strcmp(d_name + (str_len - ext_len), PROFILE_EXTENSION) == 0;
}

/// <summary>
/// Evaluates the subject of a transaction against against a user's permission ruleset.
/// </summary>
/// <param name="profile_identifier">Profile identifier [making the transaction].</param>
/// <param name="room_name">Name of the room.</param>
/// <param name="node_name">Name of the node.</param>
/// <returns>
///   <c>true</c> if the permission exists for the profile; otherwise, <c>false</c>.
/// </returns>
bool is_transaction_permissible(const char *profile_identifier, const char *room_name, const char *node_name)
{
	struct Profile *profile;
	struct RoomPermission *permission;

	HASH_FIND_STR(profiles, profile_identifier, profile);
	HASH_FIND_STR(profile->permissions, room_name, permission);

	if (permission == NULL)
	{
		return false;
	}

	for (uint8_t i = 0; i < MAX_SET_SIZE; i++)
	{ 
		if (permission->nodes[i] == NULL) // reached vacant space
		{
			break;
		}
		else if (strcmp(permission->nodes[i], node_name) == 0)
		{
			return true;
		}
	}

	return false;
}

void bind_client_socket_to_profile(const char *profile_identifier, const int client_socket_identifier)
{
	struct Profile *profile;

	HASH_FIND_STR(profiles, profile_identifier, profile);

	if (profile != NULL && profile->client_socket_identifier == NULL)
	{
		profile->client_socket_identifier = client_socket_identifier;
	}
}

/// <summary>
/// Extracts the profile name from the directory target.
/// </summary>
/// <param name="d_name">Directory target.</param>
/// <returns>Profile name.</returns>
char *profile_identifier_from_directory(const char *d_name)
{
	const size_t ext_len = strlen(PROFILE_EXTENSION);
	size_t str_len = strlen(d_name);
	
	char *file_name_extless = malloc(str_len - 2);

	memcpy(file_name_extless, d_name, str_len - ext_len);
	file_name_extless[str_len - ext_len] = '\0';

	return file_name_extless;
}

/// <summary>
/// Loads permissioning system with profiles from disk.
/// </summary>
int gather_permissions(void)
{
	DIR *directory;

	if (!(directory = opendir(PROFILE_DIRECTORY)))
	{
		return -1;
	}

	struct dirent *dir;
	FILE *profile_file;

	uint8_t profile_count;

	while ((dir = readdir(directory)) != NULL)
	{
		if (!is_casa_profile_file(dir->d_name))
		{
			continue;
		}

		char profile_path[32] = "";

		strcat(profile_path, PROFILE_DIRECTORY);
		strcat(profile_path, dir->d_name);

		profile_file = fopen(profile_path, "r"); // open for read

		if (profile_file == NULL)
		{
			printf("[!] Unable to read profile at %s\n", profile_path);
			continue;
		}

		char *profile_data = calloc(1, 1);
		char *profile_buffer[MAX_PROFILE_CHAR];

		while (fgets(profile_buffer, MAX_PROFILE_CHAR, profile_file) != NULL)
		{
			profile_data = realloc(profile_data, strlen(profile_data) + strlen(profile_buffer) + 1);
			strcat(profile_data, profile_buffer);
		}

		struct Profile *profile = malloc(sizeof(struct Profile));
		const char *profile_identifier = profile_identifier_from_directory(dir->d_name);

		strcpy(profile->identifier, profile_identifier);

		cJSON *profile_json = cJSON_Parse(profile_data);
		cJSON *permissions = cJSON_GetObjectItem(profile_json, "permissions");

		cJSON *permission_json_object = NULL;
		uint8_t permission_count = 0;

		cJSON_ArrayForEach(permission_json_object, permissions)
		{
			struct RoomPermission *permission = malloc(sizeof(struct RoomPermission));

			strcpy(permission->room_name, cJSON_GetObjectItem(permission_json_object,
				"roomName")->valuestring);
			
			cJSON *node_array_json = cJSON_GetObjectItem(permission_json_object, "nodes");

			for (uint8_t i = 0; i < cJSON_GetArraySize(node_array_json); i++)
			{
				permission->nodes[i] = cJSON_GetArrayItem(node_array_json, i)->valuestring;
			}

			HASH_ADD_STR(profile->permissions, room_name, permission);
		};

		HASH_ADD_STR(profiles, identifier, profile);
	}

	closedir(directory);

	return 0;
}