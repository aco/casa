#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "cJSON.h"

/// <summary>
/// Computes the block hash using a combination of properties as the seed.
/// </summary>
/// <param name="block">Subject block.</param>
/// <param name="hash_dest">Destination for computed hash.</param>
void compute_block_hash(struct Block *block, uint8_t *hash_dest);


/// <summary>
/// Builds a new block from the lead block
/// </summary>
struct Block *build_new_block(void);

/// <summary>
/// Evaluates a candidate block to succeed the current lead block.
/// </summary>
/// <param name="new_block">The candidate block.</param>
void handle_proposed_block(struct Block *new_block);

/// <summary>
/// Builds a JSON representation of the blockchain to a socket.
/// </summary>
/// <param name="successor_block_json">The successor block cJSON.</param>
/// <param name="block">The subject block.</param>
/// <param name="entire_chain">if set to <c>true</c> [entire chain] cascades through all ancestor blocks.</param>
/// <returns>JSON represenatation (cJSON) of the chain beginning from the passed block.</returns>
bool record_proposed_transaction_from_client(int client_socket_identifier, const char *node_name, const char *room_name, uint8_t value, bool force_wrap);

/// <summary>
/// Evaluates a proposed transaction against ruleset and records in the current block.
/// </summary>
/// <param name="profile_identifier">The profile identifier.</param>
/// <param name="node_name">Name of the node.</param>
/// <param name="room_name">Name of the room.</param>
/// <param name="force_wrap">if set to <c>true</c> [force wrap] will seal the current block regardless of remaining transaction space.</param>
/// <returns>
///		<c>true</c> if successful.
/// </returns>
bool record_proposed_transaction(const char *profile_identifier, const char *node_name, const char *room_name, uint8_t value, bool force_wrap);

/// <summary>
/// Destroys a block and its ancestores.
/// </summary>
/// <param name="starting_block">The starting block.</param>
void destroy_blockchain(struct Block *starting_block);

/// <summary>
/// Spawns the genesis block and initialises shared variables.
/// </summary>
/// <returns>0 if successful, -1 if chain already existent</returns>
int formulate_blockchain(void);

/// <summary>
/// Builds a JSON representation of the chain beginning from the specified block.
/// </summary>
/// <param name="successor_block_json">The successor block json.</param>
/// <param name="block">The next block to serialize.</param>
/// <param name="entire_chain">if set to <c>true</c>, continues down to the genesis block.</param>
/// <returns></returns>
cJSON *build_block_json(cJSON *successor_block_json, struct Block *block, bool entire_chain);

/// <summary>
/// Builds and emits a JSON representation of the room structure to a socket.
/// </summary>
/// <param name="entire_chain">if set to <c>true</c> [entire chain] cascades through all ancestor blocks.</param>
/// <param name="dispatch_socket">Client socket to dispatch.</param>
void emit_block_json(bool entire_chain, int dispatch_socket);
