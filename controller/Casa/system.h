#pragma once

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

/// <summary>
/// Generates a JSON representation of a status report attribute.
/// </summary>
/// <param name="key">Name/key of the object.</param>
/// <param name="value">Value of the object.</param>
/// <param name="upper_bound">Maximum possible value for this attribute.</param>
/// <param name="suffix">Suffix for display of the attribute.</param>
/// <returns>cJSON object.</returns>
cJSON *generate_system_stat_object(const char *name, int value, int upper_bound, StatSuffix suffix);

/// <summary>
/// Probes the temperaeture from the controller's own thermal zone.
/// </summary>
/// <returns>Thermal zone reading as int (cJSON struggled with the original value).</returns>
int probe_thermal_zone_temperature(void);

/// <summary>
/// Retrieves the local machine's address in octet notation (1.1.1.1).
/// </summary>
/// <returns>'String' of the machine wlan0 address.</returns>
const char *retrieve_local_machine_address(void);

/// <summary>
/// Generates a JSON representation of a system status report. Reports uptime, system temperature and memory consumption.
/// </summary>
/// <param name="dispatch_socket">Destination socket.</param>
void emit_system_report_json(int dispatch_socket);