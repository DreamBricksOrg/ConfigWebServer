#include "ConfigWebServer.h"

ConfigWebServer::ConfigWebServer() : server(80) {}

void ConfigWebServer::begin(const String& title_) {
  title = title_;
  SPIFFS.begin(true);

  server.on("/", [this]() { handleRoot(); });
  server.on("/syscfg", [this]() { handleRoot(); });
  server.on("/save", [this]() { handleSave(); });
  server.begin();
}

void ConfigWebServer::addVariable(const String& name, const String& label, int* ptr, int minVal, int maxVal) {
  vars.push_back({name, label, ptr, VAR_INT, (float)minVal, (float)maxVal});
}

void ConfigWebServer::addVariable(const String& name, const String& label, float* ptr, float minVal, float maxVal) {
  vars.push_back({name, label, ptr, VAR_FLOAT, minVal, maxVal});
}

void ConfigWebServer::addVariable(const String& name, const String& label, bool* ptr) {
  vars.push_back({name, label, ptr, VAR_BOOL, 0, 1});
}

void ConfigWebServer::handleClient() {
  server.handleClient();
}

void ConfigWebServer::handleRoot() {
  String html = "<html><head><title>" + title + "</title></head><body>";
  html += "<h1>" + title + "</h1><form action='/save' method='POST'>";

  for (auto& v : vars) {
    html += "<label>" + v.label + ": </label>";
    String val;
    if (v.type == VAR_INT) val = String(*(int*)v.ptr);
    if (v.type == VAR_FLOAT) val = String(*(float*)v.ptr, 2);
    if (v.type == VAR_BOOL) {
      val = (*(bool*)v.ptr) ? "1" : "0";
      html += "<input type='checkbox' name='" + v.name + "' ";// + "' value='" + val + "'";
      if (*(bool*)v.ptr) html += " checked";
      html += "><br>";
    } else {
      html += "<input type='number' step='any' name='" + v.name + "' value='" + val + "' min='" + v.minVal + "' max='" + v.maxVal + "'><br>";
    }
  }

  html += "<br><input type='submit' value='Save'></form></body></html>";
  server.send(200, "text/html", html);
}

void ConfigWebServer::handleSave() {
  for (auto& v : vars) {
    if (!server.hasArg(v.name)) {
      if (v.type == VAR_BOOL) {
        *(bool*)v.ptr = false;
      }
      continue;
    }

    String s = server.arg(v.name);

    if (v.type == VAR_INT) {
      int val = constrain(s.toInt(), (int)v.minVal, (int)v.maxVal);
      *(int*)v.ptr = val;
    } else if (v.type == VAR_FLOAT) {
      float val = constrain(s.toFloat(), v.minVal, v.maxVal);
      *(float*)v.ptr = val;
    } else if (v.type == VAR_BOOL) {
      *(bool*)v.ptr = (s == "1" || s == "on");
    }
  }

  saveToJSON();
  server.sendHeader("Location", "/");
  server.send(303);  // Redirect
}

void ConfigWebServer::saveToJSON() {
  DynamicJsonDocument doc(1024);

  for (auto& v : vars) {
    if (v.type == VAR_INT) {
      doc[v.name] = *(int*)v.ptr;
    } else if (v.type == VAR_FLOAT) {
      doc[v.name] = *(float*)v.ptr;
    } else if (v.type == VAR_BOOL) {
      doc[v.name] = *(bool*)v.ptr ? true : false;  // explicit cast
    }
  }

  File f = SPIFFS.open("/config.json", FILE_WRITE);
  if (f) {
    serializeJson(doc, f);
    f.close();
    Serial.println("[SAVE] Config saved to /config.json");

    // Print JSON to Serial for debugging
    serializeJsonPretty(doc, Serial);
    Serial.println();

  } else {
    Serial.println("[ERROR] Failed to open /config.json for writing");
  }
}


void ConfigWebServer::loadFromJSON() {
  if (!SPIFFS.exists("/config.json")) {
    Serial.println("[LOAD] /config.json not found");
    return;
  }

  File f = SPIFFS.open("/config.json", FILE_READ);
  if (!f) {
    Serial.println("[ERROR] Failed to open /config.json for reading");
    return;
  }

  DynamicJsonDocument doc(1024);
  DeserializationError err = deserializeJson(doc, f);
  f.close();

  if (err) {
    Serial.println("[ERROR] JSON deserialization failed");
    return;
  }

  for (auto& v : vars) {
    if (!doc.containsKey(v.name)) continue;

    if (v.type == VAR_INT) {
      *(int*)v.ptr = constrain(doc[v.name].as<int>(), (int)v.minVal, (int)v.maxVal);
    } else if (v.type == VAR_FLOAT) {
      *(float*)v.ptr = constrain(doc[v.name].as<float>(), v.minVal, v.maxVal);
    } else if (v.type == VAR_BOOL) {
      *(bool*)v.ptr = doc[v.name].as<bool>();
    }
  }

  Serial.println("[LOAD] Config loaded from /config.json");
}
