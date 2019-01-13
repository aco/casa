#include <stdio.h>
#include "uthash.h"

/// <summary>
/// struct housing title, thermal GPIO pin # and nodes of a room in the house
/// </summary>
struct Room
{
	char name[32];

<<<<<<< HEAD
	unsigned short thermalGPIO;
=======
	uint8_t thermalGPIO;
>>>>>>> 74d36b798d227c747c4e05ccce04a0f342b36110
	struct Node *nodes;

	UT_hash_handle hh; // hashable
};