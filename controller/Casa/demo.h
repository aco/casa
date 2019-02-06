#pragma once

#define DUMMY_REQUEST_BODY_CORE "{\"type\": %d, \"payload\": {%s}}"
#define DUMMY_REQUEST_BODY_NODE "\"room\": \"%s\", \"node\": \"%s\", \"value\": %d"
#define DUMMY_REQUEST_BODY_DEMO "\"type\": \"%s\""

#define INTERACTIVE_HELP_TABLE "\nCasa 1.0 Interactive Demo\n\nAvailable commands:\n\
-help\t\tDisplays table of information\n\
-rgb\t\tToggles home RGB lighting to a random colour\n\
-blockchain\tSerialises and prints the blockchain ledger\n\
-reject\t\tSimulates a rejected transaction request\n\
-any of the following, with a value:\n\
\t-porch\t\tLight next to the front door\n\
\t-garage\t\tLight above the garage door\n\
\t-fireplace\tFireplace in the lounge (visible through the door)\n\n\
e.g. \"garage 60\" - sets garage to 60% intensity.\n"

/// <summary>
/// Runs a CLI-based, interactive demonstration.
/// </summary>
int run_interactive_demo(void);