/**
 * CF DHT Example.
 *
 * An example of using the CF DHT helper.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0
 * @since   Sep, 2021
 */

// Include the sensor library.
#include <CFDHTHelper.h>  // CF soil moisture sensor.
#include <Logger.h>       // Logger.

// DHT Pins.
#define PIN_DHT_DATA D7   // (GPIO13 / D7 - NodeMCU) DHT Pin Data.
#define PIN_DHT_RESET D8  // (GPIO15 / D8 - NodeMCU) DHT Pin VCC (Workaround for DHT reading failure).

// Create a sensor object.
// CFDHTHelper dht(DHT11, PIN_DHT_DATA, PIN_DHT_RESET);  // CF DHT sensor (DHT11).
CFDHTHelper dht(DHT22, PIN_DHT_DATA, PIN_DHT_RESET);  // CF DHT sensor (DHT22).

void setup() {
  // Start serial.
  Serial.begin(115200);

  // Setup logger.
  Logger::setLogLevel(Logger::NOTICE);  // VERBOSE, NOTICE, WARNING, ERROR, FATAL, SILENT.

  // Config DHT.
  dht.begin();
  dht.setReadingInterval(5000);  // Define reading interval. 5 * 1000 milliseconds.
}

void loop() {
  dht.loop();  // Do DHT loop.

  // Print read values.
  if (dht.isRead()) {
    Logger::notice("Humidity: " + String(dht.getHumidity()) +
                   "%  Temperature: " + String(dht.getTemperatureC()) +
                   "ºC " + String(dht.getTemperatureF()) +
                   "ºF  Heat Index: " + String(dht.getHeatIndexC()) +
                   "ºC " + String(dht.getHeatIndexF()) + "ºF");
  } else {
    Logger::notice("Error reading values.");
  }

  // Delay.
  delay(1000);
}