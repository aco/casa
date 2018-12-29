#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <memory.h>
#include <string.h>
#include <stdbool.h>

#include "sha256.h"
#include "cJSON.h"

#include "block.h"

struct Block *lead_block;
uint8_t current_transaction_count;

/// <summary>
/// Computes the block hash using a combination of properties as the seed.
/// </summary>
/// <param name="block">Subject block.</param>
/// <param name="hash_dest">Destination for computed hash.</param>
void compute_block_hash(struct Block *block, uint8_t *hash_dest)
{
	char hash_buffer[SHA256_BYTES * 2];

	int timestamp_sum = 0;

	for (uint8_t i = 0; i < BLOCK_SIZE; i++)
	{
		if (block->transactions[i] != NULL)
		{
			timestamp_sum += block->transactions[i]->timestamp + block->transactions[i]->value;
		}
	}

	snprintf(hash_buffer, sizeof(hash_buffer), "%s.%d.%d.%d", block->data, block->index,
		timestamp_sum / block->timestamp); // concatenate block data, index and transaction cumulitive timestamp / block timestamp

	sha256(hash_buffer, strlen(hash_buffer), hash_dest);
}

/// <summary>
/// Computes the block hash using a combination of properties as the seed.
/// </summary>
/// <param name="block">Subject block.</param>
/// <param name="hash_dest">Destination for computed hash.</param>
struct Block *build_new_block(const char *data)
{
	struct Block *block = malloc(sizeof(struct Block));

	block->timestamp = time(NULL);

	block->index = lead_block == NULL ? 0 : lead_block->index + 1;
	block->prev_block = lead_block;

	strcpy(block->data, data);

	return block;
}

void handle_proposed_block(struct Block *new_block)
{
	if ((new_block->index == 0 && new_block->prev_block == NULL) || // appending genesis block
		(new_block->index == lead_block->index + 1 && new_block->prev_block->hash == lead_block->hash)) // append more recent block
	{
		lead_block = new_block;
	}
	else // discard proposed chain
	{
		struct Block *next;

		for (struct Block *curr = lead_block; curr; curr = next)
		{
			next = curr->prev_block;
			free(curr);
		}

		new_block = NULL;
	}
}

int construct_blockchain(void)
{
	if (lead_block != NULL)
	{
		return -1;
	}

	current_transaction_count = 0;
	handle_proposed_block(build_new_block("casa genesis block"));

	return 0;
}

/// <summary>
/// Evaluates a proposed transaction against ruleset and records in the current block.
/// </summary>
/// <param name="node">Node name.</param>
/// <param name="room">Room name.</param>
/// <param name="value">Proposed node value.</param>
/// <param name="client_identifier">Client identifier.</param>
/// <param name="force_wrap">if set to <c>true</c> [force wrap] will seal the current block regardless of remaining transaction space.</param>
/// <returns></returns>
bool permit_proposed_transaction(const char *node, const char *room, int value, int client_identifier, bool force_wrap)
{
	// TODO: implement ruleset

	if (force_wrap || current_transaction_count == BLOCK_SIZE)
	{
		compute_block_hash(lead_block, lead_block->hash);

		printf("[~] Sealing block #%d at %d%% capacity\n", lead_block->index, (current_transaction_count * 100) / BLOCK_SIZE);

		struct Block *new_block = build_new_block("test");

		handle_proposed_block(new_block);
		current_transaction_count = 0;
	}

	struct Transaction *transaction = malloc(sizeof(struct Transaction));

	transaction->node = node;
	transaction->room = room;
	
	transaction->value = value;
	transaction->client_identifier = client_identifier;

	transaction->timestamp = time(NULL);

	lead_block->transactions[current_transaction_count++] = transaction;

	return true;
}

/// <summary>
/// Builds a JSON representation of the blockchain to a socket.
/// </summary>
/// <param name="successor_block_json">The successor block cJSON.</param>
/// <param name="block">The subject block.</param>
/// <param name="entire_chain">if set to <c>true</c> [entire chain] cascades through all ancestor blocks.</param>
/// <returns></returns>
cJSON *build_block_json(cJSON *successor_block_json, struct Block *block, bool entire_chain)
{
	cJSON *child_block_object = cJSON_CreateObject();
	cJSON *subject_block_object = block == lead_block ? successor_block_json : child_block_object; // lead block will be a blank cJSON object

	cJSON_AddStringToObject(subject_block_object, "data", block->data);
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

/// <summary>
/// Builds and emits a JSON representation of the room structure to a socket.
/// </summary>
/// <param name="entire_chain">if set to <c>true</c> [entire chain] cascades through all ancestor blocks.</param>
/// <param name="dispatch_socket">Client socket to dispatch.</param>
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