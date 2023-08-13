#include "Mqtt.h"
#include "Led.h"
#include "PowerDisplay.h"
#include "FlashAnimation.h"
#include "SparkAnimation.h"
#include "Network.h"
#include <ArduinoJson.h>
#include "Log.h"
#include "Settings.h"

Mqtt::Mqtt(Network * network, PowerDisplay * powerDisplay, Settings * settings, Led * led, long long * lastMqttUpdateTick):
  p_powerDisplay(powerDisplay),
  p_settings(settings),
  p_led(led),
  client(PubSubClient(*network->GetClient())),
  p_lastMqttUpdateTick(lastMqttUpdateTick)
  {};

Mqtt::~Mqtt(){
  Log::write("Deconstructing Mqtt");
  if (client.connected()){
    client.publish("energy/energyLed/STATUS", "UNEXPECTED DISCONNECT");
    client.disconnect();
  }
  Log::writeln(" (complete)");
}

void Mqtt::Begin(){
  if (p_settings->IsOK() || p_settings->IsTEST()){
    Log::write("Mqtt begin");
    client.setServer(this->p_settings->Value.mqttAddress.c_str(), 1883);
    client.setCallback([&](char* topic, byte* payload, unsigned int length){
      mqttCallback(topic, payload, length);
    });
    Log::writeln(" (complete)");
  }
}

void Mqtt::Connect(){
  if (p_settings->IsOK() || p_settings->IsTEST()){
    Log::write("Mqtt connect");
    SparkAnimation connectAnimation(p_led, 0x004040, 10, false);
    while (!client.connected()) {
      Serial.println("Connecting to MQTT...");

      if (client.connect(this->p_settings->Value.hostname.c_str(), this->p_settings->Value.mqttUser.c_str(), this->p_settings->Value.mqttPassword.c_str() )) {
        Serial.println("connected");  
      } else {
        Serial.print("failed with state ");
        Serial.print(client.state());
      }
      connectAnimation.Update();

      if (connectAnimation.GetCycles() >= 1){
        FlashAnimation failAnimation(p_led, 0xFF0000, 10, 1);
        while(!failAnimation.IsComplete()) failAnimation.Update();
        delay(500);
        ESP.restart();
      }
    }
    client.publish("energy/energyLed/STATUS", "STARTED");
    client.subscribe(this->p_settings->Value.mqttEnergyTopic.c_str());
    Log::writeln(" (complete)");
  }
}

void Mqtt::Update(){
  if (!p_settings->IsOK() && !p_settings->IsTEST()){
    return;
  }
  
  if (!client.connected()) {
    this->Reconnect();
  }
  client.loop();
  if (this->lastUpdateMessage + 60000 <  millis()){
    client.publish("energy/energyLed/STATUS", "OK");
  }
}

void Mqtt::Reconnect(){
  p_led->Fill(0x000);
  p_led->Show();
  SparkAnimation connectAnimation(p_led, 0x002040, 10, false);
  for (int i = 0; i < 3 && !client.connected(); i++ ){
    Serial.println("Connecting to MQTT...");

    if (client.connect(this->p_settings->Value.hostname.c_str(), this->p_settings->Value.mqttUser.c_str(), this->p_settings->Value.mqttPassword.c_str() )) {
      Serial.println("connected");  
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
    }
    connectAnimation.Update();
  }
  if (!client.connected()){
    FlashAnimation failAnimation(p_led, 0xFF0000, 10, 1);
      while(!failAnimation.IsComplete()) failAnimation.Update();
      delay(5000);
      ESP.restart();
  }
  client.publish("energy/energyLed/STATUS", "RESTARTED");
}

void Mqtt::mqttCallback(char* topic, byte* payload, unsigned int length) {
  *p_lastMqttUpdateTick = millis();
  Serial.print(F("Message arrived in topic: "));
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
 
  DynamicJsonDocument doc(128);

  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  Serial.print("accessing path: ");
  std::string path = p_settings->Value.mqttEnergyElementPath.c_str();
  std::string delimiter = "/";
  Serial.println(path.c_str());


  //splitPath(p_settings->Value.mqttEnergyElementPath.c_str(), pathArray);

  JsonVariant value = doc;

  // access along provided path
  size_t pos = 0;
  std::string token;
  while ((pos = path.find(delimiter)) != std::string::npos) {
      token = path.substr(0, pos);
      Serial.println(token.c_str());
      value = value[token];
      if (value.isNull()) {
        Serial.println("Path not found.");
        p_settings->SetNOK();
        ESP.restart();
        return;
      }
      path.erase(0, pos + delimiter.length());
  }

  value = value[path];
  Serial.println(path.c_str());
  if (value.isNull()) {
    Serial.println("Path not found.");
    p_settings->SetNOK();
    ESP.restart();
    return;
  }
  float powerValue = value.as<int>();

  Serial.print("Value: ");
  Serial.println(powerValue);

  if (powerValue > 99999){
    return;
  }
  p_powerDisplay->UpdatePower(powerValue);
  Serial.println(F("-----------------------"));
}

void Mqtt::Disconnect(){
  if (client.connected())
  {
    client.publish("energy/energyLed/STATUS", "DISCONNECTED");
    client.disconnect();
  }
}