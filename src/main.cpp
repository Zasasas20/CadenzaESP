#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Audio.h>
#include <Preferences.h>
#include "nvs_flash.h"

#define MAXCONNECT 5

#define I2S_DOUT 22
#define I2S_BCLK 26
#define I2S_LRC 25

#define VOL 39

int volume = 10;

Audio audio;

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
Preferences pref;

StaticJsonDocument<200> doc_tx;
StaticJsonDocument<200> doc_rx;

const char * AP_SSID = "CadenzaAP";
const char * AP_password = "PASSWORD";

String webText = "<!DOCTYPE html><html><style>html, body{ margin: 0; height: 100%;}body{ display: flex; justify-content: center; align-items: center;}#Elements{ display: flex; flex-direction: column; justify-content: center; align-items: center; gap: 20px;}</style><head> <title>Cadenza Setup</title></head><body> <div id = 'Elements'> <div id = 'Info'> <div class = 'textBox'> <p>Network Name: </p> <input type='text' id = 'SSID'> </div> <div class = 'textBox'> <p>Password: </p> <input type='text' id = 'Password'> </div> </div> <div id = 'Button'> <button id='Submit'>Connect</button> </div> </div></body><script> var Socket; document.getElementById('Submit').addEventListener('click', send_info); function init(){ Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event) {}; } function send_info(){ var data = { SSID: document.getElementById('SSID').value, PASSWORD: document.getElementById('Password').value }; console.log(data); Socket.send(JSON.stringify(data)); } window.onload = function(event){ init(); console.log('Webiste Loads'); }</script></html>";
String musicText = "<!DOCTYPE html><html><style>html, body{ margin: 0; height: 100%;}body{ display: flex; justify-content: center; align-items: center;}#Info{ display: flex; align-items:end; gap: 20px;}#Elements{ display: flex; flex-direction: column; justify-content: center; align-items: center; gap: 20px;}</style><head> <title>Cadenza</title></head><body> <div id = 'Elements'> <div id = 'Menu'> <label for='Radio-Channel'>Choose a radio category:</label> <select name='Channels' id='Channels'> <option value='https://strm112.1.fm/bossanova_mobile_mp3?aw_0_req.gdpr=true'>Bossa Nova</option> <option value='http://stream.antennethueringen.de/live/aac-64/stream.antennethueringen.de/'>Hip Hop</option> <option value='http://curiosity.shoutca.st:6035/;'>Arabic</option> <option value='Custom'>Custom</option> </select> </div> <div id = 'Info'> <div class = 'textBox' id='Box' hidden> <p>Radio Channel URL: </p> <input type='text' id = 'Music'> </div> <div class = 'Button'> <button id='Change'>Change Channel</button> </div> </div> <div class = 'Button'> <button id='Reset'>Reset Settings</button> </div> </div></body><script> var Socket; document.getElementById('Reset').addEventListener('click', reset); document.getElementById('Change').addEventListener('click', change); document.getElementById('Channels').addEventListener('change', category); function init(){ Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event) {}; } function reset(){ var data = { Command: 'Reset' }; Socket.send(JSON.stringify(data)); } function change(){ var data = { Command: 'Change' }; if (document.getElementById('Channels').value === 'Custom'){ data.Link = document.getElementById('Music').value; } else{ data.Link = document.getElementById('Channels').value; } Socket.send(JSON.stringify(data)); console.log(data); } function category(){ let choice = document.getElementById('Channels').value; if (choice === 'Custom'){ document.getElementById('Box').hidden = false; } else{ document.getElementById('Box').hidden = true; } } window.onload = function(event){ init(); console.log('Website Loads'); }</script></html>";
IPAddress AP_IP(192,167,1,22);
IPAddress AP_GATEWAY(192,167,1,5);
IPAddress AP_SUBNET(255,255,255,0);

void connectionHandler(byte num, WStype_t type, uint8_t * payload, size_t length);
void setupHandler(byte num, WStype_t type, uint8_t * payload, size_t length);
void connectToWifi();
void APMode();
bool isSetup();
void writeNetwork(const char * ID, const char * PASS);
void reconnectToWifi();
void resetConfigs();

void resetConfigs(){
  Serial.println("Resetting");
  nvs_flash_erase();
  nvs_flash_init();
  Serial.println("Reset complete, restarting...");
  delay(1000);
  ESP.restart();
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
          resetConfigs();
        }
        else{
          const char* Channel = doc_rx["Link"];
          audio.connecttohost(Channel);
        }
      }
  }
}

void setupHandler(byte num, WStype_t type, uint8_t * payload, size_t length){
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
          writeNetwork(ID, PASS);
          Serial.println("Stored");
          WiFi.softAPdisconnect(true);
          Serial.println("Disconnected");
          server.stop();
          webSocket.close();
          Serial.println("Successfully closed");
          delay(200);
          ESP.restart();
        }
      }
  }
}

void connectToWifi(){

  pref.begin("wifiCreds", true);
  String ssid = pref.getString("SSID", "");
  String pass = pref.getString("PASS", "");
  pref.end();

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
  server.on("/", [](){
    server.send(200, "text\html", musicText);
  });
  server.begin();

  webSocket.begin();
  webSocket.onEvent(connectionHandler);
}

void APMode(){

  pref.begin("wifiCreds", false);
  pref.putBool("Setup", false);
  pref.end();

  WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);

  WiFi.softAP(AP_SSID, AP_password);

  Serial.println(WiFi.softAPIP());

  server.on("/", [](){
    server.send(200, "text\html", webText);
  });
  server.begin();
  Serial.println("Server Initialized");

  webSocket.begin();
  webSocket.onEvent(setupHandler);
  Serial.println("Websocket Initialized");
}

bool isSetup(){
  pref.begin("wifiCreds", true);
  bool status = pref.getBool("Setup", false);
  pref.end();
  return status;
}

void writeNetwork(const char * ID, const char * PASS){
  Serial.println("Writing details");
  pref.begin("wifiCreds", false);
  pref.putString("SSID", ID);
  pref.putString("PASS", PASS);
  pref.putBool("Setup", true);
  pref.end();
}


void setup() {
  Serial.begin(9600);

  if (isSetup()){
    connectToWifi();
  }
  else{
    APMode();
  }

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  audio.setVolume(volume);
}

void loop() {
  server.handleClient();
  webSocket.loop();
  audio.loop();
  volume = map ((analogRead(VOL)), 0, 4095, 0, 20);
  audio.setVolume(volume);
}