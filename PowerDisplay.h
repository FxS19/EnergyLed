#pragma once
#include "Led.h"

class PowerDisplay{
  private:
    Led * p_led;
    int lastWatt {0};
    long long lastupdatetime {0};
    void analyzeDisplayPower(float power);
    void displayPower(float power, byte r, byte g, byte b);
    void updatePowerConsumptionDisplay();
    int exponential(int value, int timestimes);

  public:
    PowerDisplay(Led * led);
    void UpdatePower(int watt);
    void Update();
    int GetWatt();

};