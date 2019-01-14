#pragma once

#include <stdio.h>
#include <stdint.h>

#include "sha256.h"

#define BLOCK_SIZE 4

/// <summary>
/// struct of a blockchain block
/// </summary>
struct Block
{
	uint8_t index;
	int timestamp;

	struct Block *prev_block;

	char data[256];
	struct Transaction *transactions[BLOCK_SIZE];

	uint8_t hash[SHA256_BYTES];
};

/// <summary>
/// struct of an individual transaction
/// </summary>
struct Transaction
{
	char *node;
	char *room;

	uint8_t value;
	int profile_identifier;

	int timestamp;
};