#include <Preferences.h>
#include "nvs_flash.h"

#ifndef memoryManager_H
#define memoryManager_H

class memoryManager{
    private:
        Preferences pref_;

    public:
        bool isSetup();
        bool writeNetwork(const char * ID, const char * PASS);
        bool writeDefault(const char * Link);
        bool disableWiFi();
        void resetConfig();
        bool resetAudio();

        String getDefault();
        String getSSID();
        String getPass();
};

#endif