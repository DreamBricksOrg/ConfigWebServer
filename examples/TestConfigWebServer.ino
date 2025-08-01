#include <WiFi.h>
#include "ConfigWebServer.h"

int brightness = 75;
float threshold = 2.5;
bool enabled = true;
unsigned long lastVarShown;
unsigned long timeoutMs = 1000;
ConfigWebServer configServer(true);

void setup() {
  Serial.begin(115200);

  // Create Wi-Fi Access Point
  const char* ssid = "ESP_ConfigBox";
  const char* password = "12345678"; // min 8 characters
  WiFi.softAP(ssid, password);
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP started. Connect to: http://");
  Serial.println(ip);

  // Start config server
  configServer.begin("ESP Config");

  configServer.addVariable("brightness", "LED Brightness", &brightness, 0, 100);
  configServer.addVariable("threshold", "Sensor Threshold", &threshold, 0.0f, 5.0f);
  configServer.addVariable("enabled", "Feature Enabled", &enabled);
  configServer.addVariable("timeout", "Timeout (ms)", &timeoutMs, 0UL, 60000UL);

  configServer.loadFromJSON();
  lastVarShown = millis();
}

void loop() {
  configServer.handleClient();

  if (millis() - lastVarShown > 2000) {
    Serial.printf("brightness: %d, threshold: %f, enable: %d, timeout: %lu\n", brightness, threshold, enabled, timeoutMs);
    lastVarShown = millis();
  }
}
