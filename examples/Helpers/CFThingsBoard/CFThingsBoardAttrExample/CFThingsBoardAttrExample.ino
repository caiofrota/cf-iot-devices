/**
 * CF ThingsBoard usage example with shared attribute update subscription.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0.0
 * @since   Nov, 2022
 */

// Libraries.
#include <CFThingsBoardHelper.h>  // CF ThingsBoard Helper.
#include <CFWiFiManagerHelper.h>  // CF WiFiManager Helper.
#include <Logger.h>               // Logger.

// Software info.
#define APP_CODE "cf-iot-thingsboard-attr-example"  // App code.
#define APP_VERSION "1.0.0"                         // App version.

// CF Helpers.
CFWiFiManagerHelper _cfWiFiManager(3000);                   // CF WiFiManager Helper.
CFThingsBoardHelper _cfThingsBoard(APP_CODE, APP_VERSION);  // CF WiFiManager Helper.

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

  // Config ThingsBoard.
  _cfThingsBoard.setLocalIP(_cfWiFiManager.getLocalIP());
  _cfThingsBoard.setOnThingsBoardConnectCallback(onThingsBoardConnectCallback);
}

void loop() {
  // Add a telemetry data to be sent to ThingsBoard.
  _cfThingsBoard.setTelemetryValue("test", millis());

  _cfWiFiManager.loop();  // Do WiFiManager loop.
  _cfThingsBoard.loop();  // Do ThingsBoard loop.
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
  _cfThingsBoard.setServerURL(_cfWiFiManager.getParameter("p_server_url"));
  _cfThingsBoard.setToken(_cfWiFiManager.getParameter("p_server_token"));
  _cfThingsBoard.setAttributeValue("attr_device_name", _cfWiFiManager.getParameter("p_device_name"));
}

/**
 * Callback to be called when receive any shared attribute update call from ThingsBoard.
 */
void defaultAttrCallback(const Shared_Attribute_Data &data) {
  Logger::notice("Attr received from ThingsBoard.");
  for (JsonObject::iterator it = data.begin(); it != data.end(); ++it) {
    Serial.println(it->key().c_str());
    Serial.println(it->value().as<char *>());  // We have to parse data by it's type in the current example we will show here only char data.
  }

  int jsonSize = measureJson(data) + 1;
  char buffer[jsonSize];
  serializeJson(data, buffer, jsonSize);
  Serial.println(buffer);

  // Update attributes.
  _cfWiFiManager.setParameter("p_device_name", data["attr_device_name"]);
}

/**
 * Callbacks list to be called when receive any shared attribute update call from ThingsBoard.
 */
const size_t AttrCallbackListSize = 1;
Shared_Attribute_Callback AttrCallbackList[AttrCallbackListSize] = {
    defaultAttrCallback};

/**
 * Callback to subscribe to ThingsBoard Shared Attribute update.
 */
void onThingsBoardConnectCallback() {
  _cfThingsBoard.ATTRSubscribe(AttrCallbackList, AttrCallbackListSize);
}
