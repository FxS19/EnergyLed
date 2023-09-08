#include "core_esp8266_features.h"
#include "PowerDisplay.h"
#include "Led.h"

PowerDisplay::PowerDisplay(Led * led):
  p_led(led){};

void PowerDisplay::analyzeDisplayPower(float power){
  if (power < 0){
    displayPower(fabs(power), 0, 255, 0);
  } else {
    displayPower(power, 255, 0, 0);
  }
}

int PowerDisplay::exponential(int value, int timestimes){
  for (int i = 0; i < timestimes; i++)
  {
    value *= 2;
  }
  return value;
}

void PowerDisplay::displayPower(float power, byte r, byte g, byte b){
  float minValue = 50;
  p_led->Fill(0);
  if (power < minValue){
    if (power <= 10 ){
      p_led->SetPixel(0, 8, 8, 0);
    } else {
      float p = power/minValue;
      p_led->SetPixel(0, r * p, g * p, b * p);
    }
  } else {
    uint8_t i = 0;
    // Fill bpttom
    for (; i < 9 && power >= exponential(minValue, i); i++){
      p_led->SetPixel(i, r, g, b);
    }
    if (i > 0 && i < 9){
      // Faded led
      float minRange = exponential(minValue, i - 1);
      float maxRange = exponential(minValue, i);

      float p = (power - minRange)/(maxRange - minRange);
      //p *= p;
      p_led->SetPixel(i, p * r, p * g, p * b);
    }
  } 
  p_led->Show();
}

void PowerDisplay::updatePowerConsumptionDisplay(){
  static float displayPowerConsumption = 0;
  if (displayPowerConsumption != lastWatt){
    if (displayPowerConsumption + 5 < lastWatt){
      displayPowerConsumption+= 5;
      analyzeDisplayPower(displayPowerConsumption);
    } else if (displayPowerConsumption - 5 > lastWatt) {
      displayPowerConsumption-= 5;
      analyzeDisplayPower(displayPowerConsumption);
    } else if (displayPowerConsumption != lastWatt) {
      displayPowerConsumption = lastWatt;
      analyzeDisplayPower(displayPowerConsumption);
    }
  }
}

void PowerDisplay::Update(){
  if(lastupdatetime + 10 < millis()){
    lastupdatetime = millis();
    this->updatePowerConsumptionDisplay();
  }
}

void PowerDisplay::UpdatePower(int watt){
  if (watt == 0 && abs(abs(lastWatt) - abs(watt)) > 50){
    return;
  }
  lastWatt = watt;
}

int PowerDisplay::GetWatt(){
  return this->lastWatt;
}

