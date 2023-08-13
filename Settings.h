#pragma once
#include <Arduino.h>

struct Config {
  bool IsSet;
  String networkSsid {""};
  String networkPassword {""};
  String hostname {"EnergyLed"};
  String mqttAddress {""};
  String mqttPassword {""};
  String mqttUser {""};
  String mqttEnergyTopic {""};
  String mqttEnergyElementPath {""};
  String otaPassword {""};
};

class Settings{
    private:
      bool settingsTest {false};
      bool settingsOk {false};

    public:
      Settings();
      void Begin();
      Config Value;
      void Save();
      bool IsOK();
      bool IsTEST();
      void SetOK();
      void SetNOK();
};