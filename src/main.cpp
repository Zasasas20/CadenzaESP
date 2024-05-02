#include <Arduino.h>
#include <../lib/memorySocket/memorySocket.h>
#include <../lib/webServer/APServer.h>
#include <../lib/webServer/WebServer.h>

#define I2S_DOUT 22
#define I2S_BCLK 26
#define I2S_LRC 25

std::unique_ptr<memoryManager> mem = std::unique_ptr<memoryManager>(new memoryManager);

std::unique_ptr<IWebServer> server;

Audio audio;

void setup() {
  Serial.begin(9600);

  Serial.println("Started...");

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  audio.setVolume(10);

  // Create new Server instance based on ssid/password status
  if (mem->isSetup()){
    Serial.println("Read");
    server = std::unique_ptr<WebServer>(new WebServer(std::move(mem), &audio));
  }
  else{
    Serial.println("Read AP");
    server = std::unique_ptr<APServer>(new APServer(std::move(mem)));
  }

  server->connect();
}

void loop() {
  server->loop();
}