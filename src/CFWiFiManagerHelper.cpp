/**
 * CFWiFiManagerHelper.cpp
 *
 * A library for Arduino that helps to integrate with WiFiManager.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0
 * @since   Nov, 2022
 */

// Libraries.
#include <CFWiFiManagerHelper.h>  // CF Wi-Fi Manager.

/**
 * Constructor.
 */
CFWiFiManagerHelper::CFWiFiManagerHelper() : _customPort(80),
                                             _wifiManager(),
                                             _wifiServer(80),
                                             _fileSystemPath("/cfwmconfig.json"),
                                             _defaultWifiPassword("12345678") {
  _defaultWifiSSID = _wifiManager.getDefaultAPName();
}

/**
 * Constructor with default WiFi password.
 *
 * @param defaultWifiPassword Default password that should be used when WiFi on AP mode.
 */
CFWiFiManagerHelper::CFWiFiManagerHelper(String defaultWifiPassword) : _customPort(80),
                                                                       _wifiManager(),
                                                                       _wifiServer(80),
                                                                       _fileSystemPath("/cfwmconfig.json"),
                                                                       _defaultWifiPassword(defaultWifiPassword) {
  _defaultWifiSSID = _wifiManager.getDefaultAPName();
}

/**
 * Constructor with custom port.
 */
CFWiFiManagerHelper::CFWiFiManagerHelper(int customPort) : _customPort(customPort),
                                                           _wifiManager(),
                                                           _wifiServer(customPort),
                                                           _fileSystemPath("/cfwmconfig.json"),
                                                           _defaultWifiPassword("12345678") {
  _defaultWifiSSID = _wifiManager.getDefaultAPName();
}

/**
 * Constructor with default WiFi password and custom port.
 *
 * @param defaultWifiPassword Default password that should be used when WiFi on AP mode.
 */
CFWiFiManagerHelper::CFWiFiManagerHelper(String defaultWifiPassword, int customPort) : _customPort(customPort),
                                                                                       _wifiManager(),
                                                                                       _wifiServer(customPort),
                                                                                       _fileSystemPath("/cfwmconfig.json"),
                                                                                       _defaultWifiPassword(defaultWifiPassword) {
  _defaultWifiSSID = _wifiManager.getDefaultAPName();
}

/**
 * Initialize.
 */
void CFWiFiManagerHelper::begin() {
  // Load parameters.
  _loadParameters();

  // Config Wi-Fi Manager.
  _wifiManager.setAPCallback(  // Define callback when ap mode is connected.
      std::bind(&CFWiFiManagerHelper::_APCallback, this, std::placeholders::_1));
  _wifiManager.setSaveParamsCallback(  // Define callback when parameters are saved.
      std::bind(&CFWiFiManagerHelper::_saveParameters, this));
  std::vector<const char *> menu = {"wifi",     // Config Wi-Fi.
                                    "param",    // Config Params.
                                    "info",     // Wi-Fi info.
                                    "update",   // Firmware update.
                                    "sep",      // Separator.
                                    "restart",  // Restart device.
                                    "exit",     // Exit portal.
                                    "sep",      // Separator.
                                    "erase"};   // Erase device data.
  _wifiManager.setMenu(menu);
  _wifiManager.setConfigPortalTimeout(30);  // Auto close config portal timeout.
  _wifiManager.setClass("invert");          // Dark theme.

  // Start Wi-Fi Manager.
  WiFi.mode(WIFI_STA);
  bool res = _wifiManager.autoConnect(_defaultWifiSSID.c_str(), _defaultWifiPassword.c_str());
  if (res) {
    // Connected.
    _wifiSSID = WiFi.SSID();
    _wifiIP = WiFi.localIP().toString();
    _wifiManager.setHttpPort(_customPort);
    _wifiManager.startWebPortal();
    _wifiServer.begin();
    _wifiConnected = true;
  }
}

/**
 * Loop.
 */
void CFWiFiManagerHelper::loop() {
  _wifiManager.process();
}

/**
 * Define the params that should be managed by WiFiManager.
 *
 * @param params Params that should be managed by WiFiManager.
 * @param paramsQt Params quantity.
 */
void CFWiFiManagerHelper::setCustomParameters(WiFiManagerParameter *params, int paramsQt) {
  _maxParamsQty = paramsQt;
  _wifiManagerParameters = params;

  for (int i = 0; i < _maxParamsQty; i++) {
    _wifiManager.addParameter(&_wifiManagerParameters[i]);
  }
}

/**
 * Load parameters from file into WiFiManager.
 */
void CFWiFiManagerHelper::_loadParameters() {
  if (SPIFFS.begin()) {
    if (SPIFFS.exists(_fileSystemPath)) {
      File file = SPIFFS.open(_fileSystemPath, "r");
      if (file) {
        // Create a JSON Object.
        DynamicJsonDocument jsonParams(1024);

        // Deserialize file into JSON object.
        DeserializationError error = deserializeJson(jsonParams, file);
        /*
        if (error) {
            // Fail deserializing file. Code: 'error.c_str()'
        }
        */

        // Set custom parameters.
        for (int i = 0; i < _maxParamsQty; i++) {
          String key = _wifiManagerParameters[i].getID();
          if (jsonParams.containsKey(key)) {
            String value = jsonParams[key];
            _wifiManagerParameters[i].setValue(value.c_str(), _wifiManagerParameters[i].getValueLength());
          }
        }
      }
    }
  }
}

/**
 * Save parameters into file from WiFiManager.
 */
void CFWiFiManagerHelper::_saveParameters() {
  // Open file for writing.
  File file = SPIFFS.open(_fileSystemPath, "w");
  if (file) {
    // Create JSON objects.
    DynamicJsonDocument doc(1024);

    // Set custom parameters.
    for (int i = 0; i < _maxParamsQty; i++) {
      doc[_wifiManagerParameters[i].getID()] = _wifiManagerParameters[i].getValue();
    }

    _onSaveParametersCallback();

    if (serializeJson(doc, file) != 0) {
      // Custom parameters saved.
    }

    // Close file.
    file.close();
  }
}

/**
 * Config mode callback.
 *
 * @param wifiManager Wi-Fi Manager.
 */
void CFWiFiManagerHelper::_APCallback(WiFiManager *wifiManager) {
  _wifiIP = WiFi.softAPIP().toString();
  if (_onConfigModeCallback) {
    _onConfigModeCallback();
  }
}

/**
 * Get parameter value from key.
 *
 * @param key Parameter key.
 * @return Parameter value.
 */
String CFWiFiManagerHelper::getParameter(String key) {
  for (int i = 0; i < _maxParamsQty; i++) {
    String foundKey = _wifiManagerParameters[i].getID();
    if (foundKey == key) {
      return _wifiManagerParameters[i].getValue();
    }
  }
  return "";
}

/**
 * Define parameter value with a key.
 *
 * @param key Parameter key.
 * @return Parameter value.
 */
void CFWiFiManagerHelper::setParameter(String key, String value) {
  for (int i = 0; i < _maxParamsQty; i++) {
    String foundKey = _wifiManagerParameters[i].getID();
    if (foundKey == key) {
      _wifiManagerParameters[i].setValue(value.c_str(), value.length());
      _saveParameters();
    }
  }
}

/**
 * Get default SSID.
 *
 * @return Default SSID.
 */
String CFWiFiManagerHelper::getDefaultSSID() {
  return _defaultWifiSSID;
}

/**
 * Get default password.
 *
 * @return Default password.
 */
String CFWiFiManagerHelper::getDefaultPassword() {
  return _defaultWifiPassword;
}

/**
 * Get SSID.
 *
 * @return SSID.
 */
String CFWiFiManagerHelper::getSSID() {
  return _wifiSSID;
}

/**
 * Get Local IP.
 *
 * @return Local IP.
 */
String CFWiFiManagerHelper::getLocalIP() {
  return _wifiIP;
}

bool CFWiFiManagerHelper::isConnected() {
  return _wifiConnected;
}

/**
 * Define on config mode callback.
 *
 * @param callback Callback.
 */
void CFWiFiManagerHelper::setOnConfigModeCallback(VoidCallback callback) {
  _onConfigModeCallback = callback;
}

/**
 * Define on save parameters callback.
 *
 * @param callback Callback.
 */
void CFWiFiManagerHelper::setOnSaveParametersCallback(VoidCallback callback) {
  _onSaveParametersCallback = callback;
}

/**
 * Hard reset config.
 */
void CFWiFiManagerHelper::resetSettings() {
  SPIFFS.format();
  _wifiManager.resetSettings();
}
