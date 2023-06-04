/**
 * CFThingsBoardHelper.h
 *
 * A library for Arduino that helps to integrate with ThingsBoard.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0
 * @since   Nov, 2022
 */

#ifndef CFThingsBoardHelper_h
#define CFThingsBoardHelper_h

#include <Logger.h>       // Logger.
#include <ThingsBoard.h>  // Things Board.
#include <WiFiManager.h>  // Wi-Fi.

class CFThingsBoardHelper {
 private:
  // Aliases.
  using VoidCallback = void (*)();  // Alias for callback.

  // ThingsBoard and WiFiClient attributes.
  WiFiClient _wifiClient;    // WiFi Client.
  ThingsBoard _thingsBoard;  // ThingsBoard.

  // Config attributes.
  String _appCode;           // Software code.
  String _appVersion;        // Software version.
  String _serverURL;         // Server URL.
  String _token;             // Device token to connect to ThingsBoard device.
  String _localIP;           // Local IP.
  String _deviceName;        // Device name.
  unsigned long _ttRetry;    // Time between connection attempts.
  unsigned long _ttSend;     // Time between submissions.
  unsigned long _tLastSent;  // Last time data was sent.
  bool _TBconnected;         // Flag that indicates if ThingsBoard is connected.

  // JSON Data.
  DynamicJsonDocument _data;        // JSON telemetry data.
  DynamicJsonDocument _attributes;  // JSON attributes.

  // Callbacks.
  VoidCallback _onThingsBoardConnectCallback;  // On ThingsBoard connect callback.

 public:
  CFThingsBoardHelper(String appCode, String appVersion);                       // Constructor.
  void loop();                                                                  // Loop.
  void ATTRSubscribe(const Shared_Attribute_Callback *callbacks, size_t size);  // Subscribe to attr.
  void RPCSubscribe(const RPC_Callback *callbacks, size_t size);                // Subscribe to RPC.
  void setServerURL(String serverURL);                                          // Define server URL.
  void setToken(String token);                                                  // Define token.
  void setLocalIP(String localIP);                                              // Define device name.
  bool isConnected();                                                           // True if ThingsBoard is connected.
  void setTelemetryValue(String key, int value);                                // Set telemetry int value.
  void setTelemetryValue(String key, String value);                             // Set telemetry String value.
  void setAttributeValue(String key, int value);                                // Set attribute int value.
  void setAttributeValue(String key, String value);                             // Set attribute String value.
  void setOnThingsBoardConnectCallback(const VoidCallback);                     // Define on ThingsBoard connect callback.
  void sendData();                                                              // Send pending data to ThingsBoard.
};

#endif
