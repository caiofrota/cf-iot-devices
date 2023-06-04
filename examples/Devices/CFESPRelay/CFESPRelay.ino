/**
 * CF Wi-Fi ESP Relay.
 *
 * IoT - Configurable ESP Relay device using ESP-01 (Generic ESP 8266 Module) and ThingsBoard as server.
 * Works with Amazon Alexa.
 *
 * Dependencies:
 *    - CFThingsBoardHelper -
 *    - CFWiFiManagerHelper -
 *    - Logger - Logger Library Version
 *    - FauxmoESP - Library to
 *
 * Components:
 *    - ESP-01 Relay Module
 *    OR
 *    - ESP-01
 *    - Relay Module
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
#define APP_CODE "cf-iot-esp-relay"  // App code.
#define APP_VERSION "1.0.0"          // App version.

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

// Device pins.
#define PIN_RELAY 0  // ESP-01 GPIO0.

// Device attributes.
bool _value = false;

void setup() {
  // Config relay pin as output.
  pinMode(PIN_RELAY, OUTPUT);
  updateRelay(_value);

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

  Logger::notice(_cfWiFiManager.getParameter("p_device_name").c_str());  // REMOVE

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
  _cfThingsBoard.setTelemetryValue("value", _value);

  _cfWiFiManager.loop();  // Do WiFiManager loop.
  _cfThingsBoard.loop();  // Do ThingsBoard loop.
  _fauxmo.handle();       // Do Alexa FauxmoESP loop.
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

void updateRelay(bool value) {
  _value = value;

  // Normally Open configuration, send LOW signal to let current flow.
  digitalWrite(PIN_RELAY, value ? LOW : HIGH);

  // Normally Closed configuration, send HIGH signal to let current flow.
  // digitalWrite(PIN_RELAY, !value ? LOW : HIGH);
}

/**
 * Callback to be called when Alexa has status changed.
 */
void onAlexaStatusChangeCallback(unsigned char device_id, const char* device_name, bool state, unsigned char value) {
  Serial.printf("[NOTICE] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

  // Update value.
  updateRelay(state);

  // Update value on ThingsBoard.
  _cfThingsBoard.setTelemetryValue("value", state);
  _cfThingsBoard.sendData();
}

/**
 * Callback to be called when receive getValue RPC call from ThingsBoard.
 */
RPC_Response getValueRPCCallback(const RPC_Data& data) {
  // Return value.
  return RPC_Response(NULL, _value);
}

/**
 * Callback to be called when receive setValue RPC call from ThingsBoard.
 */
RPC_Response setValueRPCCallback(const RPC_Data& data) {
  bool value = data["value"];

  // Update value.
  updateRelay(value);

  // Update Alexa.
  _fauxmo.setState(_cfWiFiManager.getParameter("p_device_name").c_str(), value, 254);

  // Return value.
  return RPC_Response(NULL, value);
}

/**
 * Callbacks list to be called when receive a RPC call from ThingsBoard.
 */
const size_t RPCCallbackListSize = 2;
RPC_Callback RPCCallbackList[RPCCallbackListSize] = {
    {"getValue", getValueRPCCallback},
    {"setValue", setValueRPCCallback}};

/**
 * Callback to subscribe to ThingsBoard RPC.
 */
void onThingsBoardConnectCallback() {
  _cfThingsBoard.RPCSubscribe(RPCCallbackList, RPCCallbackListSize);
}
