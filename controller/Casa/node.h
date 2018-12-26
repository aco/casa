#include <stdio.h>
#include <stdint.h>

#include "uthash.h"

/// <summary>
/// Named classification for a Node.
/// </summary>
typedef enum
{
	NODEALARM = 0,
	NODEDOOR,
	NODEFIREPLACE,
	NODELIGHT,
	NODERGB,
	NODETEMPERATURE
} NodeType;

/// <summary>
/// struct housing the GPIO pin, value, title, type and event
/// </summary>
struct Node
{
	unsigned short gpio[3];
	unsigned short value;

	char name[32];
	NodeType type;

	void(*change_handler)(void);

	UT_hash_handle hh; // hashable
};