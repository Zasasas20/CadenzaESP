#include "IWebServer.h"
#include <Audio.h>

#pragma once

/// @brief Datastructure that stores most recent link and type of audio
struct AudioInfo{
  const char * Link = "";
  const char * Type = "";
};

/// @brief Webserver that handles connection to audio
class WebServer : public IWebServer{

    private:
        std::unique_ptr<memoryManager> mem_;
        Audio* audio_; // Pointer to audio object for link manipulation
        AsyncWebServer server_ = AsyncWebServer(80);
        WebSocketsServer webSocket_ = WebSocketsServer(81);

        AudioInfo INF_;

        StaticJsonDocument<200> doc_rx_;

        const int MAXCONNECT{10};

        const char * AP_SSID_ = "CadenzaAP";
        const char * AP_password_ = "PASSWORD";

        void handler(uint8_t num, WStype_t type, uint8_t * payload, size_t length) override;

        /// @brief Sends currently playing details to new connecting client to keep them updated
        /// @param M_Type Type of link
        /// @param M_Link Link
        void sendNew(const char * M_Type, const char * M_Link);

        /// @brief Loads default audio link from memory
        void loadAudio();

        /// @brief Updates audio with new link
        void update();

        /// @brief Restarts cadenza
        void restart();

    public:

        WebServer(std::unique_ptr<memoryManager> mem,  Audio* audio);

        void connect() override;
        void loop() override;
};