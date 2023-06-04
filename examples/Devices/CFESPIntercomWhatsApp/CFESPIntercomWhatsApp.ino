#include <ESP8266HTTPClient.h>
#include <Logger.h>
#include <CFWiFiManagerHelper.h>

WiFiClientSecure wifiClient;
HTTPClient https;

CFWiFiManagerHelper _cfWiFiManager(3000);
#define CF_WM_MAX_PARAMS_QTY 4
WiFiManagerParameter _params[] = {{"p_device_name", "Device Name", _cfWiFiManager.getDefaultSSID().c_str(), 50},
                                  {"p_phone", "Phone", "", 20},
                                  {"p_api_key", "API Key", "", 20},
                                  {"p_message", "Message", "", 50}};


#define PIN_INTERCOM A0

const char *WEBHOOK_BASE = "https://api.callmebot.com/whatsapp.php?";
const char *PHONE = "phone=";
const char *MESSAGE = "&text=";
const char *API_KEY = "&apikey=";
const int READ_DELAY = 100;
const int NOTIFICATION_DELAY = 5000;

char webhookURL[200];
unsigned long lastRead = 0;
unsigned long lastNotificationSent = 0;
int voltage;

void setup() {
  Serial.begin(115200);
  Logger::setLogLevel(Logger::VERBOSE);  // VERBOSE, NOTICE, WARNING, ERROR, FATAL, SILENT.

  // Config WiFiManager.
  _cfWiFiManager.setCustomParameters(_params, CF_WM_MAX_PARAMS_QTY);
  _cfWiFiManager.setOnSaveParametersCallback(onSaveParametersCallback);
  _cfWiFiManager.begin();
  onSaveParametersCallback();

  refreshWebhookURL();
}

void loop() {
  _cfWiFiManager.loop();
  if (millis() < lastRead || millis() - lastRead > READ_DELAY) {
    lastRead = millis();
    voltage = analogRead(PIN_INTERCOM) / 1024.0 * 12;
    if (voltage > 6) {
      if (millis() < lastNotificationSent || millis() - lastNotificationSent > NOTIFICATION_DELAY) {
        lastNotificationSent = millis();
        Logger::notice("Voltage change indentified. Sending notification.");
        wifiClient.setInsecure();
        https.begin(wifiClient, webhookURL);
        int httpCode = https.GET();
        if (httpCode == HTTP_CODE_OK) {
          String payload = https.getString();
          Logger::notice("A notification has been sent.");
          Logger::notice("[HTTPS] Received payload telegram: " + payload);
        } else {
          Logger::notice("Sending notification has failed. Error: " + String(httpCode) + " (" + webhookURL + ")");
        }
        https.end();
      }
    }
  }
}

void refreshWebhookURL() {
  String message = _cfWiFiManager.getParameter("p_message");
  message.replace(" ", "+");
  strcpy(webhookURL, WEBHOOK_BASE);
  strcat(webhookURL, PHONE);
  strcat(webhookURL, _cfWiFiManager.getParameter("p_phone").c_str());
  strcat(webhookURL, MESSAGE);
  strcat(webhookURL, message.c_str());
  strcat(webhookURL, API_KEY);
  strcat(webhookURL, _cfWiFiManager.getParameter("p_api_key").c_str());
}

void onSaveParametersCallback() {
  Logger::notice("On save parameters callback called.");
  refreshWebhookURL();
}
