#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <../memorySocket/memorySocket.h>

#pragma once

/// @brief Interface for server class
class IWebServer {

    private:
        std::unique_ptr<memoryManager> mem_; // Memory manager object to write/read
        AsyncWebServer server_ = AsyncWebServer(80); // Server object
        WebSocketsServer webSocket_ = WebSocketsServer(81); //Websocket object

        StaticJsonDocument<200> doc_rx_; //JSON document for receiving

        /// @brief Handler function for websocket
        virtual void handler(uint8_t num, WStype_t type, uint8_t * payload, size_t length) = 0; 

    public:

        /// @brief Connect function
        virtual void connect() = 0;

        /// @brief Loop function
        virtual void loop() = 0;
        
        virtual ~IWebServer() = default;
};
