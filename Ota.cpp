#include "Settings.h"
#include "core_esp8266_features.h"
#include "Ota.h"
#include <ArduinoOTA.h>
#include "Mqtt.h"
#include "Led.h"
#include "FlashAnimation.h"

Ota::Ota(Mqtt * mqtt, Settings * settings, Led * led):
  p_mqtt(mqtt),
  p_settings(settings),
  p_led(led){}


void Ota::Begin(){
  ArduinoOTA.setHostname(this->p_settings->Value.hostname.c_str());
  ArduinoOTA.setPassword(this->p_settings->Value.otaPassword.c_str());
  ArduinoOTA.onStart([&]() {  // switch off all the PWMs during upgrade
    p_mqtt->Disconnect();
    p_led->Fill(0);
    p_led->Show();
  });

  ArduinoOTA.onProgress([&](unsigned int progress, unsigned int total) {
    Serial.println(floor(0.08 * (progress / (total / 100)) + 0.5));
    p_led->SetPixel((int)(0.08 * (progress / (total / 100)) + 0.5), 0x000F000F);
    p_led->Show();
  });

  ArduinoOTA.onEnd([&]() {  // do a fancy thing with our board led at end
    for (int i = 0; i < 8; i++) {
      p_led->Fade();
      p_led->Show();
      delay(100);
    }

    FlashAnimation successFlash(p_led, 0x000F00, 15, 5);
    while (!successFlash.IsComplete()) {
      successFlash.Update();
    }
  });

  ArduinoOTA.onError([&](ota_error_t error) {
    (void)error;
    FlashAnimation errorFlash(p_led, 0xFF0000, 15, 5);
    while (!errorFlash.IsComplete()) {
      errorFlash.Update();
    }
    ESP.restart();
  });

  /* setup the OTA server */
  ArduinoOTA.begin();
  Serial.println(F("OTA Ready"));
}


void Ota::Update(){
  ArduinoOTA.handle();
}