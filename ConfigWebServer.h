#pragma once
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <vector>

enum VarType { VAR_INT, VAR_FLOAT, VAR_BOOL, VAR_ULONG };

struct ConfigVar {
  String name;
  String label;
  void* ptr;
  VarType type;
  double minVal;
  double maxVal;
};

class ConfigWebServer {
public:
  explicit ConfigWebServer(bool debug = false);
  void begin(const String& title = "ESP Config");
  void addVariable(const String& name, const String& label, int* ptr, int minVal, int maxVal);
  void addVariable(const String& name, const String& label, float* ptr, float minVal, float maxVal);
  void addVariable(const String& name, const String& label, unsigned long* ptr, unsigned long minVal, unsigned long maxVal);
  void addVariable(const String& name, const String& label, bool* ptr);
  void handleClient();
  void loadFromJSON();
  void saveToJSON();

private:
  WebServer server;
  std::vector<ConfigVar> vars;
  String title;
  bool debug;
  void handleRoot();
  void handleSave();
};
