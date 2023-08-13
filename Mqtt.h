#pragma once
#include "WiFiClient.h"
#include <PubSubClient.h>
#include "PowerDisplay.h"
#include "Led.h"
#include "Network.h"
#include "Settings.h"
#include <ArduinoJson.h>

class Mqtt{
  private:
    PubSubClient client;
    PowerDisplay * p_powerDisplay {nullptr};
    Led * p_led {nullptr};
    Settings * p_settings {nullptr};
    long long lastUpdateMessage {0};
    long long * p_lastMqttUpdateTick {0};

    void splitPath(const char* path, JsonArray& keys);
    void mqttCallback(char* topic, byte* payload, unsigned int length);

  public:
    Mqtt(Network * network, PowerDisplay * powerDisplay, Settings * settings, Led * led, long long * lastUpdateTick);
    ~Mqtt();
    void Begin();
    void Connect();
    void Update();
    void Reconnect();
    void Disconnect();
};