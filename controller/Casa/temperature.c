// adapted from https://github.com/ccoong7/DHT22

#pragma once

#include <stdio.h>
#include <wiringPi.h>

#include "temperature.h"

void calibrate_temperature_gpio(int gpio_pin)
{
	pinMode(gpio_pin, OUTPUT);

	digitalWrite(gpio_pin, LOW);
	delay(20);					// Stay LOW for 5~30 milliseconds
	pinMode(gpio_pin, INPUT);		// 'INPUT' equals 'HIGH' level. And signal read mode
}

void probe_temperature_pin(int gpio_pin, unsigned short *sensor_data)
{
	unsigned short val = 0x00;
	unsigned short signal_length = 0;
	unsigned short val_counter = 0;
	unsigned short loop_counter = 0;

	while (1)
	{
		while (digitalRead(gpio_pin) == HIGH) // Count only HIGH signal
		{
			signal_length++;

			// When sending sensor_data ends, high signal occur infinite.
			// So we have to end this infinite loop.
			if (signal_length >= 200)
			{
				return;
			}

			delayMicroseconds(1);
		}

		if (signal_length > 0) // If signal is HIGH
		{
			loop_counter++;	// HIGH signal counting

			// The DHT22 sends a lot of unstable signals.
			// So extended the counting range.
			if (signal_length < 10)
			{
				// Unstable signal
				val <<= 1;		// 0 bit. Just shift left
			}
			else if (signal_length < 30)
			{
				// 26~28us means 0 bit
				val <<= 1;		// 0 bit. Just shift left
			}
			else if (signal_length < 85)
			{
				// 70us means 1 bit	
				// Shift left and input 0x01 using OR operator
				val <<= 1;
				val |= 1;
			}

			else // Unstable signal
			{
				return;
			}

			signal_length = 0;	// Initialize signal length for next signal
			val_counter++;		// Count for 8 bit sensor_data
		}

		// The first and second signal is DHT22's start signal. So ignore these sensor_data.
		if (loop_counter < 3)
		{
			val = 0x00;
			val_counter = 0;
		}

		if (val_counter >= 8) // If 8 bit sensor_data input complete
		{
			// 8 bit sensor_data input to the sensor_data array
			sensor_data[(loop_counter / 8) - 1] = val;

			val = 0x00;
			val_counter = 0;
		}
	}

	return 0;
}

struct DHT22 *build_temperature_reading(unsigned short *sensor_data)
{
	// The sum is maybe over 8 bit like this: '0001 0101 1010'. Remove the '9 bit' sensor_data
	short checksum = (sensor_data[0] + sensor_data[1] + sensor_data[2] + sensor_data[3]) & 0xFF;

	// If checksum sensor_data is correct (NOT 0x00), display humidity and temperature
	if (sensor_data[4] == checksum && checksum != 0x00)
	{
		struct DHT22 *reading = malloc(sizeof(struct DHT22));

		reading->humidity = ((sensor_data[0] * 256) + sensor_data[1]) / 10.0;
		reading->temperature = sensor_data[3] / 10.0;

		if (sensor_data[2] == 0x80) // If 'sensor_data[2]' sensor_data like 1000 0000, It means minus temperature
		{
			reading->temperature *= -1;
		}

		return reading;
	}

	return NULL;
}

struct DHT22 *read_temperature_celsius(int gpio_pin)
{
	unsigned short sensor_data[5] = { 0, 0, 0, 0, 0 };

	calibrate_temperature_gpio(gpio_pin);
	probe_temperature_pin(gpio_pin, sensor_data);

	return build_temperature_reading(sensor_data);
}

struct DHT22 *read_temperature_fahrenheit(int gpio_pin)
{
	struct DHT22 *reading = read_temperature_celsius(gpio_pin);

	if (reading != NULL)
	{
		reading->temperature = ((reading->temperature * 9) / 5) + 32;
	}

	return reading;
}