#pragma once

#include <stdio.h>
#include "uthash.h"

/// <summary>
/// struct housing title, thermal GPIO pin # and nodes of a room in the house
/// </summary>
struct Room
{
	char name[32];

	uint8_t thermalGPIO;
	struct Node *nodes;

	UT_hash_handle hh; // hashable
};