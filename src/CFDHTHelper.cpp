/**
 * CFDHTHelper.cpp
 *
 * DHT Helper for CF Arduino Devices.
 *
 * Libraries:
 *    - DHT Sensor Library Version 1.4.1 (https://github.com/adafruit/DHT-sensor-library/releases/tag/1.4.1).
 *
 * Components:
 *    DHT11 or DHT22 (Not tested with other DHT type).
 *
 * DHT Acknowledged BUG: https://github.com/adafruit/DHT-sensor-library/issues/94
 *
 *    Was noticed that DHT stop working after some uptime, after a long research about that
 *    was figured that it's a acknowledged BUG with no solution so far. So i've implemented
 *    a workaround until this problem is definitely solved.
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

#include <CFDHTHelper.h>  // CF DHT Helper Header.

/**
 * Constructor.
 *
 * @param pinData Pin data.
 */
CFDHTHelper::CFDHTHelper(int dhtType, int pinData) : _dht(pinData, dhtType),
                                                     _pinReset(-1),
                                                     _read(false),
                                                     _temperatureC(0),
                                                     _temperatureF(0),
                                                     _heatIndexC(0),
                                                     _heatIndexF(0),
                                                     _humidity(0),
                                                     _lastReading(0),
                                                     _readingDelay(1000) {
}

/**
 * Constructor.
 * DHT Workaround for fail reading failure.
 *
 * @param pinData Pin data.
 * @param pinReset Pin used for forced reset when reading is fail.
 */
CFDHTHelper::CFDHTHelper(int dhtType, int pinData, int pinReset) : _dht(pinData, dhtType),
                                                                   _pinReset(pinReset),
                                                                   _read(false),
                                                                   _temperatureC(0),
                                                                   _temperatureF(0),
                                                                   _heatIndexC(0),
                                                                   _heatIndexF(0),
                                                                   _humidity(0),
                                                                   _lastReading(0),
                                                                   _readingDelay(1000) {
}

/**
 * Initialize.
 */
void CFDHTHelper::begin() {
  if (_pinReset != -1) {
    pinMode(_pinReset, OUTPUT);     // DHT Workaround for fail reading failure.
    digitalWrite(_pinReset, HIGH);  // Turn on the DHT pin.
  }
  _dht.begin();
}

/**
 * Loop.
 */
void CFDHTHelper::loop() {
  if (_lastReading == 0 || millis() - _lastReading > _readingDelay) {
    // Read
    _lastReading = millis();

    _temperatureC = _dht.readTemperature();
    _temperatureF = _dht.readTemperature(true);
    _humidity = _dht.readHumidity();

    // Check if it was read.
    if (isnan(_temperatureC) || isnan(_temperatureF) || isnan(_humidity)) {
      _temperatureC = 0;
      _temperatureF = 0;
      _heatIndexC = 0;
      _heatIndexF = 0;
      _humidity = 0;
      _read = false;

      // DHT Workaround for fail reading failure.
      digitalWrite(_pinReset, !digitalRead(_pinReset));  // Force physical power recycle.

      return;
    }

    _temperatureC = roundf(_temperatureC * 10) / 10;
    _temperatureF = roundf(_temperatureF * 10) / 10;
    _humidity = roundf(_humidity * 10) / 10;
    _heatIndexC = roundf(_dht.computeHeatIndex(_temperatureC, _humidity, false) * 10) / 10;
    _heatIndexF = roundf(_dht.computeHeatIndex(_temperatureC, _humidity) * 10) / 10;

    _read = true;
  }
}

/**
 * Define time between readings.
 *
 * @param readingDelay Time between readings.
 */
void CFDHTHelper::setReadingInterval(long readingDelay) {
  _readingDelay = readingDelay;
}

/**
 * Check if it's read.
 *
 * @returns True if it's read.
 */
bool CFDHTHelper::isRead() {
  return _read;
}

/**
 * Get temperature in C.
 *
 * @returns Temperature in C.
 */
float CFDHTHelper::getTemperatureC() {
  return _temperatureC;
}

/**
 * Get temperature in F.
 *
 * @returns Temperature in F.
 */
float CFDHTHelper::getTemperatureF() {
  return _temperatureF;
}

/**
 * Get heat index in C.
 *
 * @returns Heat index in C.
 */
float CFDHTHelper::getHeatIndexC() {
  return _heatIndexC;
}

/**
 * Get heat index in F.
 *
 * @returns Heat index in F.
 */
float CFDHTHelper::getHeatIndexF() {
  return _heatIndexF;
}

/**
 * Get humidity.
 *
 * @returns Humidity.
 */
float CFDHTHelper::getHumidity() {
  return _humidity;
}

/**
 * Get DHT object.
 *
 * @returns DHT.
 */
DHT CFDHTHelper::getDHT() {
  return _dht;
}
