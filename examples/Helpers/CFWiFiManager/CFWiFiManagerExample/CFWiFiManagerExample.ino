/**
 * CF Wi-Fi Manager usage example.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0.0
 * @since   Nov, 2022
 */

// Libraries.
#include <CFWiFiManagerHelper.h>  // CF WiFiManager Helper.
#include <Logger.h>               // Logger.

// CF Helpers.
CFWiFiManagerHelper _cfWiFiManager(3000);  // CF WiFiManager Helper.

// WiFiManager parameters.
#define CF_WM_MAX_PARAMS_QTY 3
WiFiManagerParameter _params[] = {{"p_device_name", "Device Name", _cfWiFiManager.getDefaultSSID().c_str(), 50},
                                  {"p_server_url", "Server URL", "", 50},
                                  {"p_server_token", "Token", "", 50}};

void setup() {
  // Setup Serial.
  Serial.begin(115200);

  // Setup logger.
  Logger::setLogLevel(Logger::NOTICE);  // VERBOSE, NOTICE, WARNING, ERROR, FATAL, SILENT.

  // Config WiFiManager.
  _cfWiFiManager.setCustomParameters(_params, CF_WM_MAX_PARAMS_QTY);
  _cfWiFiManager.setOnSaveParametersCallback(onSaveParametersCallback);
  _cfWiFiManager.setOnConfigModeCallback(onConfigModeCallback);
  _cfWiFiManager.begin();

  // Call the callback once to update the first time.
  onSaveParametersCallback();
}

void loop() {
  _cfWiFiManager.loop();  // Do WiFiManager loop.
}

/**
 * Callback to be called when Wi-Fi config mode is called.
 */
void onConfigModeCallback() {
  Logger::notice("On config mode callback called.");
}

/**
 * Callback to update parameters when they have been modified.
 */
void onSaveParametersCallback() {
  Logger::notice("On save parameters callback called.");
}