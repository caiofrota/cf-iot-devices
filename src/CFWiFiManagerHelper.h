/**
 * CFWiFiManagerHelper.h
 *
 * A library for Arduino that helps to integrate with WiFiManager.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0
 * @since   Nov, 2022
 */

#ifndef CFWiFiManagerHelper_h
#define CFWiFiManagerHelper_h

#include <ArduinoJson.h>  // Arduino JSON.
#include <WiFiManager.h>  // Wi-Fi Manager.

class CFWiFiManagerHelper {
 private:
  // Aliases.
  using VoidCallback = void (*)();                        // Alias for callback.
  using CollectDataCallback = DynamicJsonDocument (*)();  // Alias for no input parameter callback returns JSON.

  // WiFiManager and WiFiServer attributes.
  int _maxParamsQty;                             // Max parameters quantity.
  WiFiManagerParameter *_wifiManagerParameters;  // WIFiManager parameters.
  WiFiManager _wifiManager;                      // WiFiManager.
  int _customPort;                               // Custom port.
  WiFiServer _wifiServer;                        // Wi-Fi Server.

  // Config attributes.
  String _fileSystemPath;  // Path to store configs.

  // WiFi attributes.
  String _defaultWifiSSID;      // Default SSID.
  String _defaultWifiPassword;  // Default password.
  String _wifiSSID;             // Connected SSID.
  String _wifiPassword;         // Password for wifi connection.
  String _wifiIP;               // Local IP.
  bool _wifiConnected;          // Flag that indicates WiFi is connected.

  // Methods.
  void _loadParameters();  // Load parameters from file into WiFiManager.
  void _saveParameters();  // Save parameters into file from WiFiManager.

  // Inner callbacks.
  void _APCallback(WiFiManager *wifiManager);  // Callback when AP Mode is connected.

  // Available callbacks.
  VoidCallback _onConfigModeCallback;      // On save parameters callback.
  VoidCallback _onSaveParametersCallback;  // On save parameters callback.

 public:
  // Methods.
  CFWiFiManagerHelper();                                                 // Constructor.
  CFWiFiManagerHelper(String defaultWifiPassword);                       // Constructor with WiFi default password.
  CFWiFiManagerHelper(int customPort);                                   // Constructor with custom port.
  CFWiFiManagerHelper(String defaultWifiPassword, int customPort);       // Constructor with WiFi default password and custom port.
  void begin();                                                          // Initialize.
  void loop();                                                           // Loop.
  void setCustomParameters(WiFiManagerParameter *params, int paramsQt);  // Define WiFiManager parameters.
  String getParameter(String key);                                       // Get parameter value from key.
  void setParameter(String key, String value);                           // Define parameter value with a key.
  String getDefaultSSID();                                               // Get default SSID.
  String getDefaultPassword();                                           // Get default password.
  String getSSID();                                                      // Get SSID.
  String getLocalIP();                                                   // Get local IP.
  bool isConnected();                                                    // True if WiFi is connected.
  void setOnConfigModeCallback(const VoidCallback);                      // Define on config mode callback.
  void setOnSaveParametersCallback(const VoidCallback);                  // Define on save parameters callback.
  void resetSettings();                                                  // Hard reset config.
};

#endif
