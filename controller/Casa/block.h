#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "sha256.h"

#define BLOCK_SIZE 4

/// <summary>
/// struct of a blockchain block.
/// </summary>
struct Block
{
	uint8_t index;
	uint8_t occupied_capacity;

	int timestamp;

	struct Block *prev_block;
	struct Transaction *transactions[BLOCK_SIZE];

	uint8_t hash[SHA256_BYTES];
};

/// <summary>
/// struct of a block transaction.
/// </summary>
struct Transaction
{
	char *node[32];
	char *room[32];
	char *profile_identifier[32];

	uint8_t value;
	bool authorized;

	int timestamp;
};