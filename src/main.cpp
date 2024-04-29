#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Audio.h>
#include <SPIFFS.h>
#include <../lib/memorySocket/memorySocket.h>

#define MAXCONNECT 5

#define I2S_DOUT 22
#define I2S_BCLK 26
#define I2S_LRC 25

#define VOL 39

memoryManager mem;

struct {
  const char * Link;
  const char * Type;
} AudioInfo;

int volume = 10;

Audio audio;

StaticJsonDocument<200> doc_rx;
StaticJsonDocument<200> doc_tx;

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const char * AP_SSID = "CadenzaAP";
const char * AP_password = "PASSWORD";

IPAddress AP_IP(192,167,1,22);
IPAddress AP_GATEWAY(192,167,1,5);
IPAddress AP_SUBNET(255,255,255,0);

void connectionHandler(byte num, WStype_t type, uint8_t * payload, size_t length);
void setupHandler(byte num, WStype_t type, uint8_t * payload, size_t length);
void connectToWifi();
void APMode();
void sendNew(const char * M_Type, const char * M_Link);
void loadAudio();
void update();

void update(){
  audio.connecttohost(AudioInfo.Link);
}

void connectionHandler(byte num, WStype_t type, uint8_t * payload, size_t length){
  if (type == WStype_TEXT){
      DeserializationError error = deserializeJson(doc_rx, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      else {
        const char* Command = doc_rx["Command"];
        Serial.print("Received command: ");
        Serial.println(Command);
        if (strcmp(Command, "Reset") == 0){
          const char * Config = doc_rx["Config"];
          if (strcmp(Config, "All") == 0){
            mem.resetConfig();
          }
          else{
            mem.resetAudio();
          }
        }
        else if (strcmp(Command, "Default") == 0){
          mem.writeDefault(doc_rx["Link"]);
        }
        else{
          AudioInfo.Link = doc_rx["Link"];
          AudioInfo.Type = doc_rx["Type"];
          update();
          sendNew(AudioInfo.Type, AudioInfo.Link);
        }
      }
  }
  else if (type == WStype_CONNECTED){
    sendNew(AudioInfo.Type, AudioInfo.Link);
  }
}

void setupHandler(byte num, WStype_t type, uint8_t * payload, size_t length){
    StaticJsonDocument<200> doc_rx;
    if (type == WStype_TEXT){
      DeserializationError error = deserializeJson(doc_rx, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      else {
        const char* ID = doc_rx["SSID"];
        const char* PASS = doc_rx["PASSWORD"];
        if(strcmp(ID, "") != 0 && strcmp(PASS, "") != 0){
          mem.writeNetwork(ID, PASS);
          Serial.println("Stored");
          WiFi.softAPdisconnect(true);
          Serial.println("Disconnected");
          server.end();
          webSocket.close();
          Serial.println("Successfully closed");
          delay(200);
          ESP.restart();
        }
      }
  }
}

void connectToWifi(){

  String ssid = mem.getSSID();
  String pass = mem.getPass();

  Serial.println(ssid);
  Serial.println(pass);
  WiFi.disconnect();
  WiFi.begin(ssid.c_str(), pass.c_str());
  
  int count=0;

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Connecting");
    count++;

    if (count >= MAXCONNECT){
      WiFi.disconnect();
      APMode();
      Serial.println("Connection Failed");
      return;
    }
  }

  Serial.print("Connected on: ");
  Serial.println(WiFi.localIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send(SPIFFS, "/musicplayer.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/");

  webSocket.begin();
  webSocket.onEvent(connectionHandler);

  server.begin();

  loadAudio();
}

void APMode(){

  mem.disableWiFi();

  WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);

  WiFi.softAP(AP_SSID, AP_password);

  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send(SPIFFS, "/setup.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/");

  webSocket.begin();
  webSocket.onEvent(setupHandler);
  Serial.println("Websocket Initialized");

  server.begin();
  Serial.println("Server Initialized");
}

void sendNew(const char * M_Type, const char * M_Link){
  String Jstring = "";
  StaticJsonDocument<200> doc_tx;
  JsonObject object = doc_tx.to<JsonObject>();
  object["Command"] = "New";
  object["Type"] = M_Type;
  object["Link"] = M_Link;
  serializeJson(doc_tx, Jstring);
  webSocket.broadcastTXT(Jstring);
}

void loadAudio(){
  audio.connecttohost(mem.getDefault().c_str());
}

void setup() {
  Serial.begin(9600);

  Serial.println("Started...");

  if(!SPIFFS.begin()){
    Serial.println("Failed to initialize SPIFFS");
  }

  if (mem.isSetup()){
    connectToWifi();
  }
  else{
    APMode();
  }

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  audio.setVolume(volume);
}

void loop() {
  webSocket.loop();
  audio.loop();
  volume = map ((analogRead(VOL)), 0, 4095, 0, 20);
  audio.setVolume(volume);
}