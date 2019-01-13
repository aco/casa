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
<<<<<<< HEAD
	unsigned short gpio[3];
	unsigned short value;
=======
	uint8_t gpio[3];
	uint8_t value;
>>>>>>> 74d36b798d227c747c4e05ccce04a0f342b36110

	char name[32];
	NodeType type;

	void(*change_handler)(void);

	UT_hash_handle hh; // hashable
};