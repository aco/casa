#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <memory.h>
#include <string.h>
#include <stdbool.h>

#include "sha256.h"
#include "cJSON.h"

#include "block.h"
#include "profile.h"

struct Block *lead_block;
uint8_t current_transaction_count;

void compute_block_hash(struct Block *block, uint8_t *hash_dest)
{
	char hash_buffer[SHA256_BYTES * 2];

	int timestamp_sum = 0;
	int transaction_count = 0;

	for (uint8_t i = 0; i < BLOCK_SIZE; i++)
	{
		if (block->transactions[i] != NULL)
		{
			transaction_count++;
			timestamp_sum += block->transactions[i]->timestamp + block->transactions[i]->value;
		}
	}

	snprintf(hash_buffer, sizeof(hash_buffer), "%d.ca.%d.sa.%d", block->index, transaction_count, 
		timestamp_sum / block->timestamp ); // concatenate block data, index and transaction cumulitive timestamp / block timestamp

	sha256(hash_buffer, strlen(hash_buffer), hash_dest);
}

struct Block *build_new_block(void)
{
	struct Block *block = malloc(sizeof(struct Block));

	block->timestamp = time(NULL);

	block->index = lead_block == NULL ? 0 : lead_block->index + 1;
	block->prev_block = lead_block;

	return block;
}

void handle_proposed_block(struct Block *new_block)
{
	if ((lead_block == NULL && new_block->index == 0 && new_block->prev_block == NULL) || // appending genesis block
		(new_block->index == lead_block->index + 1 && new_block->prev_block->hash == lead_block->hash)) // append more recent block
	{
		lead_block = new_block;
	}
	else // discard proposed chain
	{
		destroy_blockchain(new_block);
	}
}

void destroy_blockchain(struct Block *starting_block)
{
	struct Block *next;

	for (struct Block *current = starting_block; current; current = next)
	{
		next = current->prev_block;

		if (current != NULL)
		{
			free(current);
		}
	}

	starting_block = NULL;
}

int formulate_blockchain(void)
{
	if (lead_block != NULL)
	{
		return -1;
	}

	current_transaction_count = 0;
	handle_proposed_block(build_new_block());

	return 0;
}

bool record_proposed_transaction(const char *profile_identifier, const char *node_name, const char *room_name, uint8_t value, bool force_wrap)
{
	if (force_wrap || current_transaction_count == BLOCK_SIZE)
	{
		compute_block_hash(lead_block, lead_block->hash);

		printf("[~] Sealing block #%d at %d%% capacity\n", lead_block->index, (current_transaction_count * 100) / BLOCK_SIZE);

		struct Block *new_block = build_new_block();

		handle_proposed_block(new_block);
		current_transaction_count = 0;
	}

	struct Transaction *transaction = malloc(sizeof(struct Transaction));

	transaction->node = node_name;
	transaction->room = room_name;
	
	transaction->value = value;

	transaction->profile_identifier = profile_identifier;
	transaction->timestamp = time(NULL);
	transaction->authorized = is_transaction_permissible(profile_identifier, room_name, node_name);

	lead_block->transactions[current_transaction_count++] = transaction;

	return transaction->authorized;
}

bool record_proposed_transaction_from_client(int client_socket_identifier, const char *node_name, const char *room_name, uint8_t value, bool force_wrap)
{
	struct Profile *profile, *tmpProfile;

	HASH_ITER(hh, profiles, profile, tmpProfile)
	{
		if (profile->client_socket_identifier == client_socket_identifier)
		{
			return record_proposed_transaction(profile->identifier, node_name, room_name, value, force_wrap);
		}
	}

	return false;
}

cJSON *build_block_json(cJSON *successor_block_json, struct Block *block, bool entire_chain)
{
	cJSON *child_block_object = cJSON_CreateObject();
	cJSON *subject_block_object = block == lead_block ? successor_block_json : child_block_object; // lead block will be a blank cJSON object

	cJSON_AddNumberToObject(subject_block_object, "index", block->index);
	cJSON_AddNumberToObject(subject_block_object, "timestamp", block->timestamp);

	char hash_digest_buffer[SHA256_BYTES * 2];

	for (size_t i = 0; i < SHA256_BYTES - 1; i++) // compute hash string
	{
		snprintf(hash_digest_buffer + (i == 0 ? 0 : strlen(hash_digest_buffer)),
			sizeof(hash_digest_buffer) - strlen(hash_digest_buffer), "%02x", block->hash[i]);
	}

	cJSON_AddStringToObject(subject_block_object, "hash", hash_digest_buffer);
	cJSON_AddItemToObject(successor_block_json, "prevBlock", child_block_object);

	if (entire_chain && block->prev_block != NULL)
	{
		return build_block_json(child_block_object, block->prev_block, entire_chain);
	}
	else
	{
		return successor_block_json;
	}
}

void emit_block_json(bool entire_chain, int dispatch_socket)
{
	cJSON *root_block = cJSON_CreateObject();
	cJSON *root_object = cJSON_CreateObject();
	cJSON *payload_object = cJSON_CreateObject();

	build_block_json(root_block, lead_block, entire_chain);
	cJSON_AddItemToObject(payload_object, "leadBlock", root_block);

	cJSON_AddNumberToObject(root_object, "type", 3);
	cJSON_AddItemToObject(root_object, "payload", payload_object);

	puts(cJSON_Print(root_object));
	cJSON_free(root_object);
}