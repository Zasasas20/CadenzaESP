#include "IWebServer.h"

#pragma once

class APServer : public IWebServer{

    private:
        std::unique_ptr<memoryManager> mem_;
        AsyncWebServer server_ = AsyncWebServer(80);
        WebSocketsServer webSocket_ = WebSocketsServer(81);

        StaticJsonDocument<200> doc_rx_;

        IPAddress AP_IP_ = IPAddress(192,167,1,22);
        IPAddress AP_GATEWAY_ = IPAddress(192,167,1,5);
        IPAddress AP_SUBNET_ = IPAddress(255,255,255,0);

        const char * AP_SSID_ = "CadenzaAP";
        const char * AP_password_ = "PASSWORD";

        void handler(uint8_t num, WStype_t type, uint8_t * payload, size_t length) override;

    public:

        APServer(std::unique_ptr<memoryManager> mem);

        void connect() override;
        void loop() override;

        void updateAudio(int volume) override;
};