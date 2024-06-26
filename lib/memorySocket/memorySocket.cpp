#include "memorySocket.h"


bool memoryManager::isSetup(){
    pref_.begin("wifiCreds", true);
    bool status = pref_.getBool("Setup", false);
    pref_.end();
    return status;
}

bool memoryManager::writeNetwork(const char * ID, const char * PASS){
    Serial.println("Writing details");
    bool status = pref_.begin("wifiCreds", false);
    pref_.putString("SSID", ID);
    pref_.putString("PASS", PASS);
    pref_.putBool("Setup", true);
    pref_.end();
    return status;
}

bool memoryManager::writeDefault(const char * Link){
    Serial.println("Writing details");
    bool status = pref_.begin("musicDetails", false);
    pref_.putString("Default", Link);
    pref_.end();
    return status;
}

bool memoryManager::disableWiFi(){  
    bool status = pref_.begin("wifiCreds", false);
    pref_.putBool("Setup", false);
    pref_.end();
    return status;
}

void memoryManager::resetConfig(){
    Serial.println("Resetting");
    nvs_flash_erase();
    nvs_flash_init();
    Serial.println("Reset complete, restarting...");
    delay(1000);
    ESP.restart();
}

bool memoryManager::resetAudio(){
    Serial.println("Resetting audio");
    bool status = writeDefault("");
    return status;
}

String memoryManager::getDefault(){
    Serial.println("Reading details");
    pref_.begin("musicDetails", true);
    String DLink = pref_.getString("Default", "");
    pref_.end();
    return DLink;
}

String memoryManager::getSSID(){
    pref_.begin("wifiCreds", true);
    String ssid = pref_.getString("SSID", "");
    pref_.end();
    return ssid;
}

String memoryManager::getPass(){
    pref_.begin("wifiCreds", true);
    String pass = pref_.getString("PASS", "");
    pref_.end();
    return pass;
}   
