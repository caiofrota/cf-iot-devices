/**
 * CFThingsBoardHelper.cpp
 *
 * A library for Arduino that helps to integrate with ThingsBoard.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0
 * @since   Nov, 2022
 */

#include <CFThingsBoardHelper.h>  // CF Wi-Fi Manager.

/**
 * Constructor.
 */
CFThingsBoardHelper::CFThingsBoardHelper(String appCode, String appVersion) : _wifiClient(),
                                                                              _thingsBoard(_wifiClient),
                                                                              _ttRetry(60000),
                                                                              _ttSend(60000),
                                                                              _data(64),
                                                                              _attributes(1024),
                                                                              _TBconnected(false),
                                                                              _appCode(appCode),
                                                                              _appVersion(appVersion) {
}

/**
 * Loop.
 */
void CFThingsBoardHelper::loop() {
  // Check if it's disconnected.
  if (!_thingsBoard.connected()) {
    _TBconnected = false;
    // Check the last attempt.
    if (_tLastSent == 0 || (millis() - _tLastSent) > _ttRetry) {
      // Connect to ThingsBoard.
      Logger::notice("Connecting to Things Board node.");
      Logger::verbose("ServerURL: " + _serverURL);
      Logger::verbose("Token: " + _token);

      char serverURL[50];
      strcpy(serverURL, _serverURL.c_str());
      char token[50];
      strcpy(token, _token.c_str());
      if (_thingsBoard.connect(serverURL, token)) {
        _TBconnected = true;
        // Get chip id.
        char espChipId[6];
        sprintf(espChipId, "%06X", ESP.getChipId());

        // Send attributes to ThingsBoard.
        _thingsBoard.sendAttributeString("app_code", _appCode.c_str());
        _thingsBoard.sendAttributeString("app_version", _appVersion.c_str());
        _thingsBoard.sendAttributeString("device_chip_id", espChipId);
        _thingsBoard.sendAttributeString("device_local_ip", _localIP.c_str());

        // Call on ThingsBoard connect callback.
        if (_onThingsBoardConnectCallback) {
          _onThingsBoardConnectCallback();
        }
      } else {
        Logger::warning("Fail connecting Things Board. Retrying in " + String(_ttRetry / 1000) + " second(s).");
        _tLastSent = millis();
        return;
      }
    } else {
      // It's not time to retry.
      return;
    }
  }

  // Check the last submission.
  if (_tLastSent == 0 || (millis() - _tLastSent) > _ttSend) {
    Logger::notice("Sending data to Things Board.");

    // Send telemetry.
    char serializedJson[64];
    serializeJson(_data, serializedJson);
    if (strlen(serializedJson) > 4) {
      _thingsBoard.sendTelemetryJson(serializedJson);
    }

    // Send attributes.
    for (JsonPair p : _attributes.as<JsonObject>()) {
      const char* key = p.key().c_str();
      if (p.value().is<const char*>()) {
        _thingsBoard.sendAttributeString(key, p.value());
      }
      if (p.value().is<int>()) {
        _thingsBoard.sendAttributeInt(key, p.value());
      }
    }
    // Update last sent time.
    _tLastSent = millis();
  }

  _thingsBoard.loop();
}

/**
 * Subscribe to attr.
 *
 * @param callbacks List of callback methods to be called when receive an attr request.
 * @param size Quantity of callback methods.
 */
void CFThingsBoardHelper::ATTRSubscribe(const Shared_Attribute_Callback* callbacks, size_t size) {
  if (!_thingsBoard.Shared_Attributes_Subscribe(callbacks, size)) {
    Logger::warning("Fail subscribing to attributes.");
    return;
  }
  Logger::notice("Subscribed to Shared Attributes.");
}

/**
 * Subscribe to RPC.
 *
 * @param callbacks List of callback methods to be called when receive a RPC request.
 * @param size Quantity of callback methods.
 */
void CFThingsBoardHelper::RPCSubscribe(const RPC_Callback* callbacks, size_t size) {
  if (!_thingsBoard.RPC_Subscribe(callbacks, size)) {
    Logger::warning("Fail subscribing to RPC.");
    return;
  }
  Logger::notice("Subscribed to RPC.");
}

/**
 * Define server URL.
 *
 * @param serverURL Server URL.
 */
void CFThingsBoardHelper::setServerURL(String serverURL) {
  _serverURL = serverURL;
}

/**
 * Define token.
 *
 * @param token Token.
 */
void CFThingsBoardHelper::setToken(String token) {
  _token = token;
}
/**
 * Define local IP.
 *
 * @param localIP Local IP.
 */
void CFThingsBoardHelper::setLocalIP(String localIP) {
  _localIP = localIP;
}

/**
 * True if ThingsBoard is connected.
 */
bool CFThingsBoardHelper::isConnected() {
  return _TBconnected;
}

/**
 * Set telemetry int value.
 *
 * @param key Key.
 * @param value Int value.
 */
void CFThingsBoardHelper::setTelemetryValue(String key, int value) {
  _data[key] = value;
}

/**
 * Set telemetry String value.
 *
 * @param key Key.
 * @param value String value.
 */
void CFThingsBoardHelper::setTelemetryValue(String key, String value) {
  _data[key] = value;
}

/**
 * Set attribute int value.
 *
 * @param key Key.
 * @param value Int value.
 */
void CFThingsBoardHelper::setAttributeValue(String key, int value) {
  _attributes[key] = value;
}

/**
 * Set attribute String value.
 *
 * @param key Key.
 * @param value String value.
 */
void CFThingsBoardHelper::setAttributeValue(String key, String value) {
  _attributes[key] = value;
}

/**
 * Define on ThingsBoard connect callback.
 *
 * @param onThingsBoardConnectCallback On ThingsBoard connect callback.
 */
void CFThingsBoardHelper::setOnThingsBoardConnectCallback(VoidCallback onThingsBoardConnectCallback) {
  _onThingsBoardConnectCallback = onThingsBoardConnectCallback;
}

/**
 * Send pending data to ThingsBoard.
 */
void CFThingsBoardHelper::sendData() {
  // Reset last sent attribute in order to send what's pending in the next loop call.
  _tLastSent = 0;
}
