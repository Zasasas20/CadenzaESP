#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <../memorySocket/memorySocket.h>

#pragma once

class IWebServer {

    private:
        std::unique_ptr<memoryManager> mem_;
        AsyncWebServer server_ = AsyncWebServer(80);
        WebSocketsServer webSocket_ = WebSocketsServer(81);

        StaticJsonDocument<200> doc_rx_;

        virtual void handler(uint8_t num, WStype_t type, uint8_t * payload, size_t length) = 0;

    public:

        virtual void connect() = 0;

        virtual void loop() = 0;
        virtual void updateAudio(int volume) = 0;
        
        virtual ~IWebServer() = default;
};
