#include "core_esp8266_features.h"
#pragma once
#include "WString.h"
#include "HardwareSerial.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Settings.h"
#include "WebserverFiles.h"
#include "PowerDisplay.h"
#include <FS.h>           // Include the SPIFFS library


constexpr unsigned int str2int(const char* str, int h = 0)
{
  return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

constexpr unsigned int HOST_KEY = str2int("HOST");
constexpr unsigned int SSID_KEY = str2int("SSID");
constexpr unsigned int NETWORKPASSWORD_KEY = str2int("NETWORKPASSWORD");
constexpr unsigned int MQTTTOPIC_KEY = str2int("MQTTTOPIC");
constexpr unsigned int MQTTUSER_KEY = str2int("MQTTUSER");
constexpr unsigned int MQTTPASSWORD_KEY = str2int("MQTTPASSWORD");
constexpr unsigned int ENERGYTOPICPATH_KEY = str2int("ENERGYTOPICPATH");
constexpr unsigned int ENERGYTOPIC_KEY = str2int("ENERGYTOPIC");
constexpr unsigned int OTAPASSWORD_KEY = str2int("OTAPASSWORD");
// ... Weitere Schlüssel hier hinzufügen, falls nötig


class Webserver{
  private:
    AsyncWebServer server;
    AsyncWebSocket ws;
    Settings * p_settings {nullptr};
    long long lastWsPublish {0};
    PowerDisplay * p_powerDisplay;

    // Replaces placeholder with button section in your web page
    String settingsFormProcessor(){
      String buttons = "";
      buttons += R"(<label class="input-label" for="hostname">Hostname:</label><br>)";
      buttons += R"(<input class="input-field" type="text" data-key="HOST" value=")" + p_settings->Value.hostname + R"(">)";
      buttons += R"(<br>)";
      buttons += R"(<label class="input-label" for="ssid">SSID:</label><br>)";
      buttons += R"(<input class="input-field" type="text" data-key="SSID" value=")" + p_settings->Value.networkSsid + R"(">)";
      buttons += R"(<br>)";
      buttons += R"(<label class="input-label" for="networkPassword">Network Password:</label><br>)";
      buttons += R"(<input class="input-field" type="password" data-key="NETWORKPASSWORD"><br>)";
      buttons += R"(<br><br>)";
      buttons += R"(<label class="input-label" for="mqttAddress">MQTT Address:</label><br>)";
      buttons += R"(<input class="input-field" type="text" placeholder="Address of mqtt server" data-key="MQTTTOPIC" value=")" + p_settings->Value.mqttAddress + R"(">)";
      buttons += R"(<br>)";
      buttons += R"(<label class="input-label" for="mqttUser">MQTT User:</label><br>)";
      buttons += R"(<input class="input-field" type="text" data-key="MQTTUSER" value=")" + p_settings->Value.mqttUser + R"(">)";
      buttons += R"(<br>)";
      buttons += R"(<label class="input-label" for="mqttPassword">MQTT Password:</label><br>)";
      buttons += R"(<input class="input-field" type="password" data-key="MQTTPASSWORD">)";
      buttons += R"(<br>)";
      buttons += R"(<label class="input-label" for="mqttEnergyTopic">Energie Topic:</label><br>)";
      buttons += R"(<input class="input-field" type="text" data-key="ENERGYTOPIC" placeholder="Toppic to subscribe to" value=")" + p_settings->Value.mqttEnergyTopic + R"(">)";
      buttons += R"(<br>)";
      buttons += R"(<label class="input-label" for="mqttEnergyElementPath">Energie Topic:</label><br>)";
      buttons += R"(<input class="input-field" type="text" data-key="ENERGYTOPICPATH" placeholder="path/inside/json" value=")" + p_settings->Value.mqttEnergyElementPath + R"(">)";
      buttons += R"(<br><br>)";
      buttons += R"(<label class="input-label" for="otaPassword">OTA Password:</label><br>)";
      buttons += R"(<input class="input-field" type="password" data-key="OTAPASSWORD">)";
      return buttons;
    }

    void configureEndpoints(){
      // Route for root / web page
      server.on("/", HTTP_GET, [this] (AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html);
      });

      server.on("/settings", HTTP_GET, [this] (AsyncWebServerRequest *request) {
        String data = settings_html;
        data.replace("[[SETTINGSFORM]]", settingsFormProcessor());
        request->send_P(200, "text/html", data.c_str());
      });



      server.on("/api/settings", HTTP_POST, [this] (AsyncWebServerRequest *request) {
        // ESP/api/settings?key=<inputMessage1>&value=<inputMessage2>
        if (request->hasParam("key") && request->hasParam("value")) {
          String key = request->getParam("key")->value();
          String value = request->getParam("value")->value();
          key.toUpperCase();
          switch (str2int(key.c_str())) {
            case HOST_KEY:
                p_settings->Value.hostname = value;
                break;
            case SSID_KEY:
                p_settings->Value.networkSsid = value;
                break;
            case NETWORKPASSWORD_KEY:
                p_settings->Value.networkPassword = value;
                break;
            case MQTTTOPIC_KEY:
                p_settings->Value.mqttAddress = value;
                break;
            case MQTTUSER_KEY:
                p_settings->Value.mqttUser = value;
                break;
            case MQTTPASSWORD_KEY:
                p_settings->Value.mqttPassword = value;
                break;
            case ENERGYTOPIC_KEY:
                p_settings->Value.mqttEnergyTopic = value;
                break;
            case ENERGYTOPICPATH_KEY:
                p_settings->Value.mqttEnergyElementPath = value;
                break;
            case OTAPASSWORD_KEY:
                p_settings->Value.otaPassword = value;
                break;
            // ... Fälle für weitere Schlüssel hinzufügen
            default:
                Serial.println("Key not found");
          };
          
          Serial.print("Key: ");
          Serial.print(key);
          Serial.print(" - Set to: ");
          Serial.println(value);
          p_settings->Save();
          request->send_P(200, "text/plain", "OK");
        } else {
          request->send_P(400, "text/plain", "Invalid value");
        }
      });

      server.on("/api/reboot", HTTP_GET, [this] (AsyncWebServerRequest *request) {
        Log::writeln(F("Reboot requested"));
        request->send_P(200, "text/html", "OK");
        delay(50);
        ESP.restart();
      });

      server.on("/api/reset", HTTP_GET, [this] (AsyncWebServerRequest *request) {
        Log::writeln(F("Reset requested"));
        request->send_P(200, "text/html", "OK");
        delay(50);
        SPIFFS.format();
        ESP.restart();
      });
    }

    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                void *arg, uint8_t *data, size_t len) {
      switch (type) {
        case WS_EVT_CONNECT:
          Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
          break;
        case WS_EVT_DISCONNECT:
          Serial.printf("WebSocket client #%u disconnected\n", client->id());
          break;
        case WS_EVT_DATA:
          //handleWebSocketMessage(arg, data, len);
          break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
          break;
      }
    }

    void sendWatt(){
      ws.textAll(String(p_powerDisplay->GetWatt() * -1));
    }

  public:
    Webserver(Settings * settings, PowerDisplay * powerDisplay):
    server(AsyncWebServer(80)),
    ws(AsyncWebSocket("/ws")),
    p_powerDisplay(powerDisplay),
    p_settings(settings){};
    
    void Begin(){
      configureEndpoints();
      ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
        this->onEvent(server, client, type, arg, data, len);
      });
      server.addHandler(&ws);
      server.begin();
    }


    void Update(){
      if(lastWsPublish + 1000 < millis()){
        sendWatt();
        lastWsPublish = millis();
      }
      ws.cleanupClients();
    }
};