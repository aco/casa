#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "cJSON.h"

void compute_block_hash(struct Block *block, uint8_t *hash_dest);
struct Block *build_new_block(void);

void handle_proposed_block(struct Block *new_block);

bool record_proposed_transaction_from_client(int client_socket_identifier, const char *node_name, const char *room_name, uint8_t value, bool force_wrap);
bool record_proposed_transaction(const char *profile_identifier, const char *node_name, const char *room_name, uint8_t value, bool force_wrap);

void destroy_blockchain(struct Block *starting_block);
int formulate_blockchain(void);

cJSON *build_block_json(cJSON *successor_block_json, struct Block *block, bool entire_chain);
void emit_block_json(bool entire_chain, int dispatch_socket);
