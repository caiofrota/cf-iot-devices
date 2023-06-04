/**
 * CF ThingsBoard usage example with Alexa.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0.0
 * @since   Nov, 2022
 */

// Libraries.
#include <CFThingsBoardHelper.h>  // CF ThingsBoard Helper.
#include <CFWiFiManagerHelper.h>  // CF WiFiManager Helper.
#include <Logger.h>               // Logger.
#include <fauxmoESP.h>            // Alexa FauxmoESP.

// Software info.
#define APP_CODE "cf-iot-thingsboard-example"  // App code.
#define APP_VERSION "1.0.0"                    // App version.

// CF Helpers.
CFWiFiManagerHelper _cfWiFiManager(3000);                   // CF WiFiManager Helper.
CFThingsBoardHelper _cfThingsBoard(APP_CODE, APP_VERSION);  // CF WiFiManager Helper.

// WiFiManager parameters.
#define CF_WM_MAX_PARAMS_QTY 3
WiFiManagerParameter _params[] = {{"p_device_name", "Device Name", _cfWiFiManager.getDefaultSSID().c_str(), 50},
                                  {"p_server_url", "Server URL", "", 50},
                                  {"p_server_token", "Token", "", 50}};

// Alexa FauxmoESP.
fauxmoESP _fauxmo;

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

  // Setup alexa device.
  Logger::notice("Setting up FauxmoESP Alexa.");
  _fauxmo.addDevice(_cfWiFiManager.getParameter("p_device_name").c_str());
  _fauxmo.setPort(80);  // Required for Gen3 devices.
  _fauxmo.enable(true);
  _fauxmo.onSetState([](unsigned char device_id, const char* device_name, bool state, unsigned char value) {
    onAlexaStatusChangeCallback(device_id, device_name, state, value);
  });
}

void loop() {
  // Add a telemetry data to be sent to ThingsBoard.
  _cfThingsBoard.setTelemetryValue("test", millis());

  _cfWiFiManager.loop();  // Do WiFiManager loop.
  _cfThingsBoard.loop();  // Do ThingsBoard loop.
  _fauxmo.handle();       // Do Alexa FauxmoESP loop.

  //_fauxmo.setState(_cfWiFiManager.getParameter("p_device_name").c_str(), true, 254);
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
 * Callback to be called when Alexa has status changed.
 */
void onAlexaStatusChangeCallback(unsigned char device_id, const char* device_name, bool state, unsigned char value) {
  Serial.printf("[MAINS] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
}

/**
 * Callback to be called when receive default RPC call from ThingsBoard.
 */
RPC_Response defaultRPCCallback(const RPC_Data& data) {
  int value = data["value"];

  // Return value.
  return RPC_Response(NULL, 0);
}

/**
 * Callbacks list to be called when receive a RPC call from ThingsBoard.
 */
const size_t RPCCallbackListSize = 1;
RPC_Callback RPCCallbackList[RPCCallbackListSize] = {
    {"default_rpc", defaultRPCCallback}};

/**
 * Callback to subscribe to ThingsBoard RPC.
 */
void onThingsBoardConnectCallback() {
  _cfThingsBoard.RPCSubscribe(RPCCallbackList, RPCCallbackListSize);
}
