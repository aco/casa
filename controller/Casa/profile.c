#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>

#include "cJSON.h"
#include "uthash.h"

#include "profile.h"

#define PROFILE_DIRECTORY "./profiles/"
#define PROFILE_EXTENSION ".casap"

#define MAX_PROFILE_CHAR 1024

bool is_casa_profile_file(const char *d_name)
{
	const size_t ext_len = strlen(PROFILE_EXTENSION);
	const size_t str_len = strlen(d_name);

	return strcmp(d_name + (str_len - ext_len), PROFILE_EXTENSION) == 0;
}

bool is_transaction_permissible_from_socket(const int client_socket_identifier, const char *room_name, const char *node_name)
{
	struct Profile *profile = find_profile_from_client_socket(client_socket_identifier);

	return is_transaction_permissible(profile, room_name, node_name); 
}

bool is_transaction_permissible(struct Profile *profile, const char *room_name, const char *node_name)
{
	if (profile == NULL)
	{
		return false;
	}

	struct RoomPermission *permission;

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

bool is_room_accessible(struct Profile *profile, const char *room_name)
{
	struct RoomPermission *permission;
	HASH_FIND_STR(profile->permissions, room_name, permission);

	return permission != NULL && permission->nodes[0] != NULL;
}

struct Profile *find_profile_from_client_socket(const int client_socket_identifier)
{
	struct Profile *profile, *tmpProfile;

	HASH_ITER(hh, profiles, profile, tmpProfile)
	{
		if (profile->client_socket_identifier == client_socket_identifier)
		{
			return profile;
		}
	}

	return NULL;
}

void bind_client_socket_to_profile(const char *profile_identifier, const int client_socket_identifier)
{
	struct Profile *profile;

	HASH_FIND_STR(profiles, profile_identifier, profile);

	if (profile != NULL)
	{
		profile->client_socket_identifier = client_socket_identifier;
	}
}

char *profile_identifier_from_directory(const char *d_name)
{
	const size_t ext_len = strlen(PROFILE_EXTENSION);
	size_t str_len = strlen(d_name);
	
	char *file_name_extless = malloc(str_len - 2);

	memcpy(file_name_extless, d_name, str_len - ext_len);
	file_name_extless[str_len - ext_len] = '\0';

	return file_name_extless;
}

int gather_permissions(void)
{
	DIR *directory;

	if (!(directory = opendir(PROFILE_DIRECTORY)))
	{
		return -1;
	}

	struct dirent *dir;
	FILE *profile_file;

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
			printf("[x] Unable to read profile at %s\n", profile_path);
			continue;
		}
		else // set permissions to read-only for all tiers
		{
			chmod(profile_path, S_IRUSR);
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

		cJSON_ArrayForEach(permission_json_object, permissions)
		{
			struct RoomPermission *permission = malloc(sizeof(struct RoomPermission));

			strcpy(permission->room_name, cJSON_GetObjectItem(permission_json_object,
				"roomName")->valuestring);
			
			cJSON *node_array_json = cJSON_GetObjectItem(permission_json_object, "nodes");

			for (uint8_t i = 0; i < cJSON_GetArraySize(node_array_json); i++)
			{
				const char *node_name = cJSON_GetArrayItem(node_array_json, i)->valuestring;

				for (uint8_t j = 0; j < i; j++) // ensure node identifier is distinct
				{
					if (strcmp(permission->nodes[j], node_name) == 0)
					{
						return -1;
					}
				}

				permission->nodes[i] = cJSON_GetArrayItem(node_array_json, i)->valuestring;
			}

			HASH_ADD_STR(profile->permissions, room_name, permission);
		};

		HASH_ADD_STR(profiles, identifier, profile);
		printf("[+] Loaded policy for profile '%s'\n", profile->identifier);
	}

	closedir(directory);

	return 0;
}