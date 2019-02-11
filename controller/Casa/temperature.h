/// <summary>
/// Bundles the temperature and humidity readings of a DHT22 sensor.
/// </summary>
struct DHT22
{
	/// <summary>
	/// The humidity.
	/// </summary>
	float humidity;
	/// <summary>
	/// The temperature.
	/// </summary>
	float temperature;
};

/// <summary>
/// Pulls up/down the sensor in preparation for a reading.
/// </summary>
/// <param name="gpio_pin">The gpio pin connected to the sensor sensor_data line.</param>
void calibrate_temperature_gpio(int gpio_pin);

/// <summary>
/// Probes the DHT22 and resolves the signals.
/// </summary>
/// <param name="gpio_pin">The gpio pin connected to the sensor sensor_data line.</param>
/// <param name="sensor_data">Pointer to sensor_data store.</param>
void probe_temperature_pin(int gpio_pin, unsigned short *sensor_data);

/// <summary>
/// Builds a temperature reading from the sensor_data store.
/// </summary>
/// <param name="gpio_pin">The gpio pin connected to the sensor sensor_data line.</param>
/// <returns>struct DHT22 *reading.</returns>
struct DHT22 *build_temperature_reading(unsigned short *sensor_data);

/// <summary>
/// Reads the temperature in Celsius.
/// </summary>
/// <param name="gpio_pin">The gpio pin connected to the sensor sensor_data line.</param>
/// <returns>struct DHT22 *reading.</returns>
struct DHT22 *read_temperature_celsius(int gpio_pin);

/// <summary>
/// Reads the temperature in Fahrenheit.
/// </summary>
/// <param name="gpio_pin">The gpio pin connected to the sensor sensor_data line.</param>
/// <returns>struct DHT22 *reading.</returns>
struct DHT22 *read_temperature_fahrenheit(int gpio_pin);