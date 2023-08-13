#include "HardwareSerial.h"
#include "Settings.h"
#include <FS.h>           // Include the SPIFFS library
#include "Log.h"

#include <ArduinoJson.h>

#define SETTINGS_FILE_NAME "settings.json"
#define SETTINGS_OK_FILE_NAME "settings.OK"
#define SETTINGS_TEST_FILE_NAME "settings.TEST"


#define SETTINGS_HOSTNAME "hostname"
#define SETTINGS_MQTT_USER "mqttUser"
#define SETTINGS_MQTT_PASSWORD "mqttPassword"
#define SETTINGS_MQTT_ADDRESS "mqttAddress"
#define SETTINGS_MQTT_ENERGY_TOPIC "mqttEnergyTopic"
#define SETTINGS_MQTT_ENERGY_ELEMENT_PATH "mqttEnergyElementPath"
#define SETTINGS_NETWORK_SSID "networkSSID"
#define SETTINGS_NETWORK_PASSWORD "networkPassword"
#define SETTINGS_IS_SET "isSet"
#define SETTINGS_OTA_PASSWORD "otaPassword"

Settings::Settings(){
}

void Settings::Begin(){
  if(!SPIFFS.exists(SETTINGS_FILE_NAME)){
    Value = Config();
  } else {
    File file = SPIFFS.open(SETTINGS_FILE_NAME, "r");

    DynamicJsonDocument settings(2048);
    DeserializationError error = deserializeJson(settings, file);
    file.close();

    if (error){
      Serial.println(F("Failed to read file, using default configuration"));
      Value = Config();
      Value.IsSet = false;
      return;
    }

    Value.hostname = String(settings[SETTINGS_HOSTNAME]);
    Value.mqttAddress = String(settings[SETTINGS_MQTT_ADDRESS]);
    Value.mqttPassword = String(settings[SETTINGS_MQTT_PASSWORD]);
    Value.mqttEnergyTopic = String(settings[SETTINGS_MQTT_ENERGY_TOPIC]);
    Value.mqttUser = String(settings[SETTINGS_MQTT_USER]);
    Value.mqttEnergyElementPath = String(settings[SETTINGS_MQTT_ENERGY_ELEMENT_PATH]);
    Value.networkSsid = String(settings[SETTINGS_NETWORK_SSID]);
    Value.networkPassword = String(settings[SETTINGS_NETWORK_PASSWORD]);
    Value.IsSet = settings[SETTINGS_IS_SET];
    Value.otaPassword = String(settings[SETTINGS_OTA_PASSWORD]);

    settingsOk = SPIFFS.exists(SETTINGS_OK_FILE_NAME);
    settingsTest =SPIFFS.exists(SETTINGS_TEST_FILE_NAME);

    Serial.print(SETTINGS_HOSTNAME); Serial.print("="); Serial.println(Value.hostname);
    Serial.print(SETTINGS_MQTT_ADDRESS); Serial.print("="); Serial.println(Value.mqttAddress);
    Serial.print(SETTINGS_MQTT_PASSWORD); Serial.print("="); Serial.println(Value.mqttPassword);
    Serial.print(SETTINGS_MQTT_ENERGY_TOPIC); Serial.print("="); Serial.println(Value.mqttEnergyTopic);
    Serial.print(SETTINGS_MQTT_ENERGY_ELEMENT_PATH); Serial.print("="); Serial.println(Value.mqttEnergyElementPath);
    Serial.print(SETTINGS_MQTT_USER); Serial.print("="); Serial.println(Value.mqttUser);
    Serial.print(SETTINGS_NETWORK_SSID); Serial.print("="); Serial.println(Value.networkSsid);
    Serial.print(SETTINGS_NETWORK_PASSWORD); Serial.print("="); Serial.println(Value.networkPassword);
    Serial.print(SETTINGS_IS_SET); Serial.print("="); Serial.println(Value.IsSet);
    Serial.print(SETTINGS_OTA_PASSWORD); Serial.print("="); Serial.println(Value.otaPassword);

    Serial.print(SETTINGS_OK_FILE_NAME); Serial.print("="); Serial.println(settingsOk);
    Serial.print(SETTINGS_TEST_FILE_NAME); Serial.print("="); Serial.println(settingsTest);
  }
}


void Settings::Save(){
  if(!SPIFFS.exists(SETTINGS_FILE_NAME)){
    SPIFFS.remove(SETTINGS_FILE_NAME);
  }
  DynamicJsonDocument settings(2048);
  settings[SETTINGS_HOSTNAME] = Value.hostname;
  settings[SETTINGS_MQTT_ADDRESS] = Value.mqttAddress;
  settings[SETTINGS_MQTT_PASSWORD] = Value.mqttPassword;
  settings[SETTINGS_MQTT_ENERGY_TOPIC] = Value.mqttEnergyTopic;
  settings[SETTINGS_MQTT_ENERGY_ELEMENT_PATH] = Value.mqttEnergyElementPath;
  settings[SETTINGS_MQTT_USER] = Value.mqttUser;
  settings[SETTINGS_NETWORK_SSID] = Value.networkSsid;
  settings[SETTINGS_NETWORK_PASSWORD] = Value.networkPassword;
  settings[SETTINGS_OTA_PASSWORD] = Value.otaPassword;
  Value.IsSet = Value.hostname != "" &&
               Value.mqttAddress != "" &&
               Value.mqttEnergyTopic != "" &&
               Value.networkSsid != "" &&
               Value.networkPassword != "" &&
               Value.mqttEnergyElementPath != "";
  settings[SETTINGS_IS_SET] = Value.IsSet;

  Serial.print(SETTINGS_HOSTNAME); Serial.print("="); Serial.println(Value.hostname);
  Serial.print(SETTINGS_MQTT_ADDRESS); Serial.print("="); Serial.println(Value.mqttAddress);
  Serial.print(SETTINGS_MQTT_PASSWORD); Serial.print("="); Serial.println(Value.mqttPassword);
  Serial.print(SETTINGS_MQTT_ENERGY_TOPIC); Serial.print("="); Serial.println(Value.mqttEnergyTopic);
  Serial.print(SETTINGS_MQTT_ENERGY_ELEMENT_PATH); Serial.print("="); Serial.println(Value.mqttEnergyElementPath);
  Serial.print(SETTINGS_MQTT_USER); Serial.print("="); Serial.println(Value.mqttUser);
  Serial.print(SETTINGS_NETWORK_SSID); Serial.print("="); Serial.println(Value.networkSsid);
  Serial.print(SETTINGS_NETWORK_PASSWORD); Serial.print("="); Serial.println(Value.networkPassword);
  Serial.print(SETTINGS_IS_SET); Serial.print("="); Serial.println(Value.IsSet);
  Serial.print(SETTINGS_OTA_PASSWORD); Serial.print("="); Serial.println(Value.otaPassword);

  File file =SPIFFS.open(SETTINGS_FILE_NAME, "w");
  serializeJson(settings, file);
  file.close();

  if (SPIFFS.exists(SETTINGS_OK_FILE_NAME)){
    Log::writeln(F("Settings deleting SETTINGS_OK_FILE_NAME"));
    SPIFFS.remove(SETTINGS_OK_FILE_NAME);
  } else {
    Log::writeln(F("Settings SETTINGS_OK_FILE_NAME does not exist"));
  }
  if (!SPIFFS.exists(SETTINGS_TEST_FILE_NAME) && Value.IsSet){
    Log::writeln(F("Settings creating SETTINGS_TEST_FILE_NAME"));
    File testfile = SPIFFS.open(SETTINGS_TEST_FILE_NAME, "w");
    testfile.write("true");
    testfile.close();
  } else {
    Log::writeln(F("Settings SETTINGS_TEST_FILE_NAME exists"));
  }
}


bool Settings::IsOK(){
  return settingsOk;
}

void Settings::SetOK(){
  if (!SPIFFS.exists(SETTINGS_OK_FILE_NAME) && Value.IsSet){
    Log::writeln(F("Settings removing SETTINGS_OK_FILE_NAME"));
    File testfile = SPIFFS.open(SETTINGS_OK_FILE_NAME, "w");
    testfile.write("true");
    testfile.close();
  }
  if (SPIFFS.exists(SETTINGS_TEST_FILE_NAME)){
    Log::writeln(F("Settings creating SETTINGS_TEST_FILE_NAME"));
    SPIFFS.remove(SETTINGS_TEST_FILE_NAME);
    settingsTest = false;
  }
}

void Settings::SetNOK(){
  Log::writeln(F("Settings set NOK"));
  if (SPIFFS.exists(SETTINGS_OK_FILE_NAME)){
    Log::writeln(F("Settings remove SETTINGS_OK_FILE_NAME"));
    SPIFFS.remove(SETTINGS_OK_FILE_NAME);
    settingsOk = false;
  }
  if (SPIFFS.exists(SETTINGS_TEST_FILE_NAME)){
    Log::writeln(F("Settings remove SETTINGS_TEST_FILE_NAME"));
    SPIFFS.remove(SETTINGS_TEST_FILE_NAME);
    settingsTest = false;
  }
}

bool Settings::IsTEST(){
  return settingsTest;
}

