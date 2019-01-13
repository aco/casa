#include <stdio.h>
#include <wiringPi.h>
#include <ifaddrs.h>

#include "cJSON.h"

/// <summary>
/// Suffix classification for an attribute of a status report, interpreted by the client.
/// </summary>
typedef enum
{
	/// <summary>
	/// No suffix.
	/// </summary>
	SUFFNONE = 0,
	/// <summary>
	/// A temperature suffix.
	/// </summary>
	SUFFTHERMAL,
	/// <summary>
	/// A percentage (%) suffix.
	/// </summary>
	SUFFPERCENTAGE
} StatSuffix;

cJSON *generate_system_stat_object(const char *name, int value, int upper_bound, StatSuffix suffix);

int probe_thermal_zone_temperature(void);
const char *retrieve_local_machine_address(void);

void emit_system_report_json(int dispatch_socket);