#include <ESP8266WiFi.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>

#include <ArduinoJson.h>

const int ESP8266_userBtn = 16;
const int ESP8266_flashBtn = 0;

void setup()
{
    //setup the two button pins
    pinMode (ESP8266_userBtn, OUTPUT);
    digitalWrite (ESP8266_userBtn, HIGH);
    digitalWrite (ESP8266_flashBtn, HIGH);
    pinMode (ESP8266_flashBtn, INPUT);

    //setup a nice name before connecting, to find it in the list on the router
    String hostName ("ESPLamprey");
    String hostID (ESP.getChipId(), HEX);
    hostID.toUpperCase();
    hostName += "-";
    hostName += hostID;
    WiFi.hostname (hostName);

    //check if userbtn is pressed while starting, if so, go in config mode.
    //otherwise try to do the autoconnect.
    pinMode (ESP8266_userBtn, INPUT);
    digitalWrite (ESP8266_userBtn, HIGH);
    int btnVal = digitalRead (ESP8266_userBtn);
    pinMode (ESP8266_userBtn, OUTPUT);

    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout (180);

    if (btnVal == LOW)
    {
        //force config portal
        digitalWrite (ESP8266_userBtn, LOW);
        wifiManager.startConfigPortal (hostName.c_str());
        digitalWrite (ESP8266_userBtn, HIGH);
    }
    else
        wifiManager.autoConnect (hostName.c_str());

    //save current settings (mqtt server) to the json settings file

    //subscribe, start connection with the mqtt server

}

void loop()
{
    if (digitalRead (ESP8266_flashBtn) == HIGH)
        digitalWrite (ESP8266_userBtn, HIGH);
    else
        digitalWrite (ESP8266_userBtn, LOW);
}
