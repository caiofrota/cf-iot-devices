/**
 * CFDHTHelper.h
 *
 * DHT Helper for CF Arduino Devices.
 *
 * Libraries:
 *      - DHT Sensor Library Version 1.4.1 (https://github.com/adafruit/DHT-sensor-library/releases/tag/1.4.1).
 *
 * Components:
 *      DHT11 or DHT22 (Not tested with other DHT type).
 *
 * DHT Acknowledged BUG: https://github.com/adafruit/DHT-sensor-library/issues/94
 *
 *      Was noticed that DHT stop working after some uptime, after a long research about that
 *      was figured that it's a acknowledged BUG with no solution so far. So i've implemented
 *      a workaround until this problem is definitely solved.
 *
 * Workaround:
 *    A pin is being used for physically restarting DHT when it's getting NaN.
 *    Connect the + DHT pin to any pulled down digital write pin and pass it to the constructor.
 *    If it fails reading the pin will be turned off, then turned on again doing a physical reset.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0
 * @since   Nov, 2021
 */

#ifndef CFDHTHelper_h
#define CFDHTHelper_h

#include <DHT.h>     // DHT.
#include <Logger.h>  // Logger.

class CFDHTHelper {
 private:
  // Attributes.
  DHT _dht;             // DHT object.
  int _pinReset;        // DHT Workaround for fail reading failure.
  bool _read;           // Flag that indicate if data was read.
  float _temperatureC;  // Temperature in C.
  float _temperatureF;  // Temperature in F.
  float _heatIndexC;    // Heat index in C.
  float _heatIndexF;    // Heat index in F.
  float _humidity;      // Humidity.

  // Loop control.
  unsigned long _lastReading;   // Last time data was read.
  unsigned long _readingDelay;  // Time between readings.

 public:
  // Constructors.
  CFDHTHelper(int dhtType, int pinData);                // Constructor.
  CFDHTHelper(int dhtType, int pinData, int pinReset);  // Constructor with DHT Workaround for fail reading failure.

  // Methods.
  void begin();  // Initialize.
  void loop();   // Loop.

  // Accessors.
  void setReadingInterval(long readingDelay);  // Define time between readings.
  bool isRead();                               // Check if it's read.
  float getTemperatureC();                     // Get temperature in C.
  float getTemperatureF();                     // Get temperature in F.
  float getHeatIndexC();                       // Get heat index in C.
  float getHeatIndexF();                       // Get heat inter in F.
  float getHumidity();                         // Get humidity.
  DHT getDHT();                                // Get DHT object.
};

#endif
