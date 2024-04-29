#include "IWebServer.h"
#include <Audio.h>

#pragma once

struct AudioInfo{
  const char * Link = "";
  const char * Type = "";
};

class WebServer : public IWebServer{

    private:
        std::unique_ptr<memoryManager> mem_;
        Audio* audio_;
        AsyncWebServer server_ = AsyncWebServer(80);
        WebSocketsServer webSocket_ = WebSocketsServer(81);

        AudioInfo INF_;

        StaticJsonDocument<200> doc_rx_;

        const int MAXCONNECT{10};

        const char * AP_SSID_ = "CadenzaAP";
        const char * AP_password_ = "PASSWORD";

        void handler(uint8_t num, WStype_t type, uint8_t * payload, size_t length) override;

        void sendNew(const char * M_Type, const char * M_Link);
        void loadAudio();
        void update();
        void restart();

    public:

        WebServer(std::unique_ptr<memoryManager> mem,  Audio* audio);

        void connect() override;
        void loop() override;

        void updateAudio(int volume) override;
};