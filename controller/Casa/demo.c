#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <fcntl.h>
#include <sys/socket.h>

#include "demo.h"

#include "command.h"

int run_interactive_demo(void)
{
	printf("\n\n> (or type \"help\") ");

	int rgb_colors[7] = { 0xff0000, 0x0000ff, 0xffc500, 0x9400d3, 0x00c5cd, 0xff00ff, 0xffffff };

	char *input[1024];
	char *delimiter = " ";

	char *token;

	while (fgets(input, 1024, stdin))
	{
		token = strtok(input, delimiter);

		int token_length = strlen(token);
		if (token_length > 1 && token[token_length - 1] == '\n')
		{
			token[token_length - 1] = '\0'; // replace newline with null terminator for evaluation
		}

		char *message[512];
		char *payload[128];

		if (strcmp(token, "help") == 0) // print help table
		{
			puts(INTERACTIVE_HELP_TABLE);
			printf("> (or type \"help\") ");

			continue;
		}
		else if (strcmp(token, "rgb") == 0) // select random color from above
		{
			int color = rgb_colors[rand() % (7 + 1 - 0) + 0]; // select random color;

			sprintf(payload, DUMMY_REQUEST_BODY_NODE, "home", "side_lighting", color);
			sprintf(message, DUMMY_REQUEST_BODY_CORE, CMDNODE, payload);
		}
		else if (strcmp(token, "blockchain") == 0) // print blockchain
		{
			sprintf(payload, DUMMY_REQUEST_BODY_DEMO, token);
			sprintf(message, DUMMY_REQUEST_BODY_CORE, CMDDEMO, payload);
		}
		else if (strcmp(token, "reject") == 0) // print blockchain
		{
			sprintf(payload, DUMMY_REQUEST_BODY_NODE, "kitchen", "stove", 1);
			sprintf(message, DUMMY_REQUEST_BODY_CORE, CMDNODE, payload);
		}
		else // process node request/unresolvable
		{
			if (strcmp(token, "porch") == 0)
			{
				int arg = atoi(strtok(NULL, delimiter));

				sprintf(payload, DUMMY_REQUEST_BODY_NODE, "home", "porch_light", arg);
				sprintf(message, DUMMY_REQUEST_BODY_CORE, CMDNODE, payload);
			}
			else if (strcmp(token, "garage") == 0)
			{
				int arg = atoi(strtok(NULL, delimiter));

				sprintf(payload, DUMMY_REQUEST_BODY_NODE, token, "entrance_light", arg);
				sprintf(message, DUMMY_REQUEST_BODY_CORE, CMDNODE, payload);
			}
			else
			{
				printf("> (or type \"help\") ");
				continue;
			}
		}

		evaluate_message(0, message);

		printf("> (or type \"help\") ");
	}
}