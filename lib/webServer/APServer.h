#include "IWebServer.h"

#pragma once

/// @brief Access Point server used to setup wifi creditentials
class APServer : public IWebServer{

    private:
        std::unique_ptr<memoryManager> mem_;
        AsyncWebServer server_ = AsyncWebServer(80);
        WebSocketsServer webSocket_ = WebSocketsServer(81);

        StaticJsonDocument<200> doc_rx_;

        IPAddress AP_IP_ = IPAddress(192,167,1,22); // IP address
        IPAddress AP_GATEWAY_ = IPAddress(192,167,1,5); // Gateway
        IPAddress AP_SUBNET_ = IPAddress(255,255,255,0); // Subnet mask

        const char * AP_SSID_ = "CadenzaAP"; //SSID for created AP
        const char * AP_password_ = "PASSWORD"; //Password for created AP

        void handler(uint8_t num, WStype_t type, uint8_t * payload, size_t length) override;

    public:

        APServer(std::unique_ptr<memoryManager> mem);

        void connect() override;
        void loop() override;
};