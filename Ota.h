#pragma once
#include <ArduinoOTA.h>
#include "Mqtt.h"
#include "Led.h"
#include "Settings.h"

class Ota {
  private:
    Mqtt * p_mqtt {nullptr};
    Led * p_led {nullptr};
    Settings * p_settings {nullptr};
  
  public:
    Ota(Mqtt * mqtt, Settings * settings, Led * led);
    void Begin();
    void Update();
};