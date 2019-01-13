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

<<<<<<< HEAD
=======
/// <summary>
/// struct of an individual transaction
/// </summary>
>>>>>>> 74d36b798d227c747c4e05ccce04a0f342b36110
struct Transaction
{
	char *node;
	char *room;

	int value;
	int client_identifier;

	int timestamp;
};