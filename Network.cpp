#include "WString.h"
#include "HardwareSerial.h"
#include "WiFiClient.h"
#include "core_esp8266_features.h"
#include "Network.h"
#include "SparkAnimation.h"
#include "FlashAnimation.h"
#include "Led.h"
#include "Settings.h"

Network::Network(Led * led, Settings * settings):
  p_led(led),
  p_settings(settings){}

void Network::Begin(){
  Log::writeln("Network begin");
  if (p_settings->IsOK() || p_settings->IsTEST()) {
    Log::writeln("Network connecting to SSID");
    WiFi.begin(this->p_settings->Value.networkSsid, this->p_settings->Value.networkPassword);
    WiFi.hostname(this->p_settings->Value.hostname);
    this->Connect();
    if (WiFi.status() != WL_CONNECTED) {
      delay(500);
      ESP.restart();
    }
  } else {
    Log::writeln("Network is fallback");
    WiFi.softAP("ENERGY_LED");
  }
  Log::writeln("Network complete");
}

void Network::Connect(){
  Log::writeln("Network connect");
  WiFi.setAutoConnect(true);
  SparkAnimation sparkAnimation(p_led, 0x000040, 15, false);
  int maxCycles = 80;
  if (p_settings->IsTEST()){
    maxCycles = 10;
  }
  while (WiFi.status() != WL_CONNECTED) {
    sparkAnimation.Update();
    if (sparkAnimation.GetCycles() > maxCycles){
      sparkAnimation.CLear();
      
      FlashAnimation flashAnimation(p_led, 0xFF0000, 15, 1);
      while (!flashAnimation.IsComplete()) flashAnimation.Update();
      flashAnimation.CLear();
      if (p_settings->IsTEST()){
        Log::writeln("Network config not correct: restart to config");
        p_settings->SetNOK();
        ESP.restart();
      }
      return;
    }
  }
  Log::write("IP is: ");
  Serial.println(WiFi.localIP());
  Log::writeln("Network connect (complete)");
}

void Network::Reconnect(){
  while (WiFi.status() != WL_CONNECTED){
    this->Connect();
    if (WiFi.status() != WL_CONNECTED){
      WiFi.scanComplete();
      WiFi.begin(this->p_settings->Value.networkSsid, this->p_settings->Value.networkPassword);
    }
  }
  FlashAnimation successAnimation(p_led, 0x00FF00, 15, 1);
  while (!successAnimation.IsComplete()) {
    successAnimation.Update();
    delay(10);
  }
  successAnimation.CLear();
}

WiFiClient * Network::GetClient(){
  return &this->wifiClient;
}