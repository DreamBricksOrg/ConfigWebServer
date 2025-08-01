# ConfigWebServer

ConfigWebServer is a simple Arduino library for the ESP32 that provides a web interface for viewing and editing configuration variables. Values are stored in a JSON file on SPIFFS so they persist across resets.

## Features

- Create a small configuration portal accessible from a web browser
- Supported variable types: `int`, `float`, `bool`, and `unsigned long`
- Values are saved in `/config.json` on the ESP32's SPIFFS filesystem
- Minimal API that integrates with existing sketches

## Installation

Copy this repository into your Arduino libraries folder or install it using the Library Manager when available. Make sure the ESP32 platform and the [ArduinoJson](https://arduinojson.org/) library are also installed.

## Basic Usage

1. Include the library and create a `ConfigWebServer` instance.
2. Add variables you want to expose through the web interface.
3. Start the server and optionally load values from the JSON file.
4. Call `handleClient()` regularly in `loop()` to process requests.

```cpp
#include <WiFi.h>
#include "ConfigWebServer.h"

int brightness = 75;
float threshold = 2.5;
bool enabled = true;
unsigned long timeoutMs = 1000;
ConfigWebServer configServer;

void setup() {
    Serial.begin(115200);
    WiFi.softAP("ESP_ConfigBox", "12345678");

    configServer.begin("ESP Config");
    configServer.addVariable("brightness", "LED Brightness", &brightness, 0, 100);
    configServer.addVariable("threshold", "Sensor Threshold", &threshold, 0.0f, 5.0f);
    configServer.addVariable("enabled", "Feature Enabled", &enabled);
    configServer.addVariable("timeout", "Timeout (ms)", &timeoutMs, 0UL, 60000UL);

    configServer.loadFromJSON();
}

void loop() {
    configServer.handleClient();
}
```

Navigate to the device's IP address in a browser to view and edit the variables. When you submit the form, the new values are saved to SPIFFS and will be restored on the next boot.

## License

This project is licensed under the terms of the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details.

