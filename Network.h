#include "WString.h"
#pragma once
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "Led.h"
#include "Settings.h"

class Network {
  private:
    WiFiClient wifiClient;
    Led * p_led {nullptr};
    Settings * p_settings {nullptr};

  public:
    Network(Led * led, Settings * settings);
    void Begin();
    void Connect();
    void Reconnect();
    WiFiClient * GetClient();
};