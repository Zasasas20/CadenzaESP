#include <Arduino.h>
#include <WiFi.h>
#include <Audio.h>


#define I2S_DOUT 22
#define I2S_BCLK 26
#define I2S_LRC 25

#define VOL 39

int volume = 10;

Audio audio;

String ssid = "TP-Link_7B03";
String password = "58015742";

void setup() {
  Serial.begin(9600);

  WiFi.disconnect();
  WiFi.begin(ssid.c_str(), password.c_str());

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Connecting");
  }

  Serial.println("Connected");

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  audio.setVolume(volume);

  //audio.connecttohost("https://mbcfm-riyadh-prod-dub.shahid.net/out/v1/69c8a03f507e422f99cf5c07291c9e3a/index_1.m3u8");

  audio.connecttohost("http://stream.antennethueringen.de/live/aac-64/stream.antennethueringen.de/");
}

void loop() {
  audio.loop();

  volume = map ((analogRead(VOL)), 0, 4095, 0, 20);

  Serial.print("volume: ");
  Serial.println(volume);
  audio.setVolume(volume);
}