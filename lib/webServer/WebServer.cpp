#include "WebServer.h"

WebServer::WebServer(std::unique_ptr<memoryManager> mem, Audio* audio): 
mem_(std::move(mem)),
audio_(audio)
{   
    if(!SPIFFS.begin()){
        Serial.println("Failed to initialize SPIFFS");
    }

    audio_->setPinout(26, 25, 22);

    audio_->setVolume(10);
}

void WebServer::connect(){
    String ssid = mem_->getSSID();
    String pass = mem_->getPass();

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
        mem_->disableWiFi();
        Serial.println("Connection Failed, restarting in AP Mode");
        delay(1000);
        ESP.restart();
        }
    }

    Serial.print("Connected on: ");
    Serial.println(WiFi.localIP());
    server_.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
        request->send(SPIFFS, "/musicplayer.html", "text/html");
    });

    server_.serveStatic("/", SPIFFS, "/");

    webSocket_.begin();
    webSocket_.onEvent([this](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {this->handler(num, type, payload, length);});

    server_.begin();

    loadAudio();
}

void WebServer::loop(){
    webSocket_.loop();
    audio_->loop();
    audio_->setVolume(map ((analogRead(39)), 0, 4095, 0, 20));
}

void WebServer::handler(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
    if (type == WStype_TEXT){
        DeserializationError error = deserializeJson(doc_rx_, payload);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }
        else {
            const char* Command = doc_rx_["Command"];
            Serial.print("Received command: ");
            Serial.println(Command);
            if (strcmp(Command, "Reset") == 0){
            const char * Config = doc_rx_["Config"];
                if (strcmp(Config, "All") == 0){
                    mem_->resetConfig();
                }
                else{
                    mem_->resetAudio();
                }
            }
            else if (strcmp(Command, "Default") == 0){
                mem_->writeDefault(doc_rx_["Link"]);
            }
            else{
                INF_.Link = doc_rx_["Link"];
                INF_.Type = doc_rx_["Type"];
                update();
                sendNew(INF_.Type, INF_.Link);
            }
        }
    }
    else if (type == WStype_CONNECTED){
        sendNew(INF_.Type, INF_.Link);
    }
}

void WebServer::sendNew(const char * M_Type, const char * M_Link){
    String Jstring = "";
    StaticJsonDocument<200> doc_tx;
    JsonObject object = doc_tx.to<JsonObject>();
    object["Command"] = "New";
    object["Type"] = M_Type;
    object["Link"] = M_Link;
    serializeJson(doc_tx, Jstring);
    webSocket_.broadcastTXT(Jstring);
}

void WebServer::loadAudio(){
    audio_->connecttohost(mem_->getDefault().c_str());
}

void WebServer::update(){
    audio_->connecttohost(INF_.Link);
}

void WebServer::restart(){
    Serial.println("Restarting...");
    delay(200);
    ESP.restart();
}

void WebServer::updateAudio(int volume){
    audio_->setVolume(volume);
}