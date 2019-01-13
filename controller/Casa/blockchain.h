#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "cJSON.h"

void compute_block_hash(struct Block *block, uint8_t *hash_dest);
struct Block *build_new_block(const char *data);

void handle_proposed_block(struct Block *new_block);
bool permit_proposed_transaction(const char *node, const char *room, int value, int client_identifier, bool force_wrap);

int construct_blockchain(void);

cJSON *build_block_json(cJSON *successor_block_json, struct Block *block, bool entire_chain);
void emit_block_json(bool entire_chain, int dispatch_socket);
