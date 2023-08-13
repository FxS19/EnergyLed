#include <TZ.h>
#include <time.h>
#include <FS.h> 

#include "Log.h"
#include "Led.h"
#include "FlashAnimation.h"
#include "SparkAnimation.h"
#include "Network.h"
#include "Ota.h"
#include "PowerDisplay.h"
#include "Mqtt.h"
#include "Webserver.h"
 
uint timeoutMs = 60000;
long long lastUpdateTick = 0.0 - timeoutMs;
const float bright = 0.8;
const float dark = 0.15;
float brightness = 1;
bool timeIsSet = false;
const char * host = "EnergyLed";

Settings settings = Settings();
Led led(&brightness);
Network network(&led, &settings);
PubSubClient client(*network.GetClient());
PowerDisplay powerDisplay(&led);
Mqtt mqtt(&network, &powerDisplay, &settings, &led, &lastUpdateTick);
Ota ota(&mqtt, &settings, &led);
Webserver webserver(&settings, &powerDisplay);

void setup() {
  Serial.begin(115200);
  Log::writeln("");
  led.Begin();
  for (int i = 0; i < 5; i++){
    for (int a = 0; a<65; a+=1){
      led.SetPixel(0, 0, 0, a);
      led.Show();
      delay(10);
    }
    for (int a = 65; a>0; a-=1){
      led.SetPixel(0, 0, 0, a);
      led.Show();
      delay(10);
    }
  }
  InitalizeFileSystem();

  //Initialize
  settings.Begin();
  SetupMDNS();
  network.Begin();
  SetupTime();
  ota.Begin();
  mqtt.Begin();

  mqtt.Connect();
  webserver.Begin();

  FlashAnimation animation(&led, 0x00FF00, 15, 1);
  while(!animation.IsComplete()){
    animation.Update();
  } 
  Log::writeln("Setup complete");

  if (settings.IsTEST()){
    settings.SetOK();
    ESP.restart();
  }
}

void timeIsSetCb(bool from_sntp /* <= this parameter is optional */) {
  if (from_sntp) {
    Log::writeln(F("Time was set"));
    timeIsSet = true;
  }
}

void SetupTime(){
  Log::write("Setup time");
  settimeofday_cb(timeIsSetCb);
  configTime(TZ_Europe_Berlin, F("0.de.pool.ntp.org"), F("1.de.pool.ntp.org"), F("2.de.pool.ntp.org"));
  Log::writeln(" (complete)");
}

void SetupMDNS() { // Start the mDNS responder
  Log::write("Setup MDNS");
  MDNS.begin(host);                        // start the multicast domain name server
  Log::write("mDNS responder started: http://");
  Log::write(host);
  Log::write(".local");
  Log::writeln(" (complete)");
}

void updateBrightness(){
  if (timeIsSet){
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
      Log::writeln(F("Failed to obtain time"));
    } else {
      if (timeinfo.tm_hour >= 18 || timeinfo.tm_hour < 8){
        brightness = dark;
      } else {
        brightness = bright;
      }
    }
  }
}

bool isValueTimeout(){
  return lastUpdateTick + timeoutMs < millis();
}
bool isHugeValueTimeout(){
  return lastUpdateTick + timeoutMs * 15 < millis();
}

void displayValueTimeout(){
  static SparkAnimation spark(&led, 0x808080, 13, true);
  spark.Update();
}

bool InitalizeFileSystem() {
  bool initok = false;
  initok = SPIFFS.begin();
  if (!initok) // Format SPIFS, of not formatted. - Try 1
  {
    Serial.println("SPIFFS Dateisystem formatiert.");
    SPIFFS.format();
    initok = SPIFFS.begin();
  }
  if (!initok) // Format SPIFS. - Try 2
  {
    SPIFFS.format();
    initok = SPIFFS.begin();
  }
  if (initok) { Serial.println("SPIFFS ist  OK"); } else { Serial.println("SPIFFS ist nicht OK"); }
  return initok;
}

void loop() {
  if (settings.IsOK()){  
    updateBrightness();
    if (WiFi.status() != WL_CONNECTED){
      mqtt.Disconnect();
      network.Reconnect();
      return;
    }
  }

  if(!settings.IsOK()){
    static SparkAnimation spark(&led, 0xD00505, 13, true);
    spark.Update();
  }

  ota.Update();
  mqtt.Update();
  webserver.Update();

  if (settings.IsOK()){ 
    if (isValueTimeout()){
      displayValueTimeout();

      if (isHugeValueTimeout()){
        ESP.restart();
      }
    } else {
      powerDisplay.Update();
    }
  }

  delay(5);
}