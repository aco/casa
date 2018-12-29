#include <stdio.h>
#include <stdint.h>

#include "sha256.h"

#define BLOCK_SIZE 4

/// <summary>
/// struct of a blockchain block
/// </summary>
struct Block
{
	int index;
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

	int value;
	int client_identifier;

	int timestamp;
};