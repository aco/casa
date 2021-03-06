#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "system.h"

#include "cJSON.h"

#include "command.h"
#include "socket.h"

cJSON *generate_system_stat_object(const char *key, int value, int upper_bound, StatSuffix suffix)
{
	cJSON *stat_object = cJSON_CreateObject();

	cJSON_AddStringToObject(stat_object, "name", key);
	cJSON_AddNumberToObject(stat_object, "value", value);
	cJSON_AddNumberToObject(stat_object, "ubound", upper_bound);
	cJSON_AddNumberToObject(stat_object, "suffix", suffix);
	
	return stat_object;
}

int probe_thermal_zone_temperature(void)
{
	int temperature;

	if (fscanf(fopen("/sys/class/thermal/thermal_zone0/temp", "r"), "%d", &temperature) == 0)
	{
		return temperature / 1000;
	}
	
	return 0;
}

const char *retrieve_local_machine_address(void)
{
	struct ifaddrs *myaddrs, *ifa;
	char buffer[64];

	if (getifaddrs(&myaddrs) != 0)
	{
		return buffer;
	}

	for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, "wlan0") == 0)
		{
			struct sockaddr_in *s4 = (struct sockaddr_in *)ifa->ifa_addr;
			inet_ntop(ifa->ifa_addr->sa_family, &s4->sin_addr, buffer, sizeof(buffer));

			break;
		}
	}

	freeifaddrs(myaddrs);

	return buffer;
}

void emit_system_report_json(int dispatch_socket)
{
	struct sysinfo info;
	sysinfo(&info);

	cJSON *root_object = cJSON_CreateObject();
	cJSON *payload_object = cJSON_CreateObject();
	cJSON *stat_array = cJSON_AddArrayToObject(payload_object, "value");

	cJSON_AddNumberToObject(root_object, "type", CMDREPORT);
	cJSON_AddStringToObject(payload_object, "type", "system");

	cJSON_AddItemToArray(stat_array, generate_system_stat_object("uptime", info.uptime, 60, SUFFNONE)); // build uptime
	cJSON_AddItemToArray(stat_array, generate_system_stat_object("sys_temp", probe_thermal_zone_temperature(), 85, SUFFTHERMAL)); //RPi max operating range is 85

	cJSON_AddItemToArray(stat_array, generate_system_stat_object("usage", // build mem usage
		((double)(info.totalram - info.freeram) / (double)info.totalram) * 100, 60, SUFFPERCENTAGE));

	cJSON_AddItemToObject(root_object, "payload", payload_object);

	handle_write_descriptor(cJSON_PrintUnformatted(root_object), dispatch_socket);
	puts("[>] System report");
}
