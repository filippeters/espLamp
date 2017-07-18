#include "FS.h"
#include <ESP8266WiFi.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager

#include <PubSubClient.h>         // https://github.com/knolleary/pubsubclient
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson

#if defined(DEBUG_TELNET)
WiFiServer  telnetServer (23);
WiFiClient  telnetClient;
#define     DEBUG_PRINT(x)    telnetClient.print (x)
#define     DEBUG_PRINTLN(x)  telnetClient.println (x)
#elif defined(DEBUG_SERIAL)
#define     DEBUG_PRINT(x)    Serial.print (x)
#define     DEBUG_PRINTLN(x)  Serial.println (x)
#else
#define     DEBUG_PRINT(x)
#define     DEBUG_PRINTLN(x)
#endif

const int ESP8266_userBtn = 16;
const int ESP8266_flashBtn = 0;

//define your default values here, if there are different values in config.json, they are overwritten.
static char mqttServer[40];
static char mqttPort[6] = "8080";
static bool shouldSaveConfig = false;

//the main mqtt client
WiFiClient wifiClient;
PubSubClient  mqttClient (wifiClient);

//callback notifying us of the need to save config
void saveConfigCallback ()
{
    DEBUG_PRINTLN ("Should save config");
    shouldSaveConfig = true;
}


static void MqttCallback (const char* topic, byte* payload, unsigned int length)
{

}

void setup()
{
    //setup the two button pins
    pinMode (ESP8266_userBtn, OUTPUT);
    digitalWrite (ESP8266_userBtn, HIGH);
    digitalWrite (ESP8266_flashBtn, HIGH);
    pinMode (ESP8266_flashBtn, INPUT);

    //check if userbtn is pressed while starting, if so, go in config mode.
    //otherwise try to do the autoconnect.
    pinMode (ESP8266_userBtn, INPUT);
    digitalWrite (ESP8266_userBtn, HIGH);
    int btnVal = digitalRead (ESP8266_userBtn);
    pinMode (ESP8266_userBtn, OUTPUT);

    //turn led on to acknowledge the btn was pressed on power up
    if (btnVal == LOW)
        digitalWrite (ESP8266_userBtn, LOW);

    //setup a nice name before connecting, to find it in the list on the router
    String hostName ("ESPLamprey");
    String hostID (ESP.getChipId(), HEX);
    hostID.toUpperCase();
    hostName += "-";
    hostName += hostID;
    WiFi.hostname (hostName);

    //load the file system, read some settings for the mqtt server and last used display settings
    if (SPIFFS.begin())
    {
        DEBUG_PRINTLN ("mounted FS");
        if (SPIFFS.exists ("/config.json"))
        {
            DEBUG_PRINTLN ("loaded json config");
            File configFile = SPIFFS.open ("/config.json", "r");
            if (configFile)
            {
                DEBUG_PRINTLN ("opened config file");
                size_t size = configFile.size();

                // Allocate a buffer to store contents of the file.
                std::unique_ptr<char[]> buf (new char[size]);

                configFile.readBytes (buf.get(), size);
                DynamicJsonBuffer jsonBuffer;
                JsonObject& json = jsonBuffer.parseObject (buf.get());
                json.printTo (Serial);
                if (json.success())
                {
                    DEBUG_PRINTLN ("parsed json");

                    strcpy (mqttServer, json["mqtt_server"]);
                    strcpy (mqttPort, json["mqtt_port"]);
                }
                else
                    DEBUG_PRINTLN ("failed to load json config");
            }
        }
        else
            DEBUG_PRINTLN ("failed to load json config");
    }
    else
        DEBUG_PRINTLN ("failed to mount FS");

    WiFiManager wifiManager;

    //set a timeout value. if it can't connect, then at least revert to last used light state
    wifiManager.setConfigPortalTimeout (180);

    //set config save notify callback
    wifiManager.setSaveConfigCallback (saveConfigCallback);

    //add custom params to wifimanager
    WiFiManagerParameter customMqttServer ("server", "mqtt server", mqttServer, 40);
    WiFiManagerParameter customMqttPort ("port", "mqtt port", mqttPort, 6);
    wifiManager.addParameter (&customMqttServer);
    wifiManager.addParameter (&customMqttPort);

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
    strcpy (mqttServer, customMqttServer.getValue());
    strcpy (mqttPort, customMqttPort.getValue());
    if (shouldSaveConfig)
    {
        DEBUG_PRINTLN ("saving config");
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();
        json["mqtt_server"] = mqttServer;
        json["mqtt_port"] = mqttPort;

        File configFile = SPIFFS.open ("/config.json", "w");
        if (!configFile)
            DEBUG_PRINTLN ("failed to open config file for writing");
        else
        {
            json.printTo (configFile);
            configFile.close();
        }
    }

    //subscribe, start connection with the mqtt server (if address and port are set)
    String mqttHost = customMqttServer.getValue();
    String mqttPort = customMqttPort.getValue();
    if (mqttHost.length() != 0)
    {
        mqttClient.setServer (mqttHost.c_str(), mqttPort.toInt());
        mqttClient.setCallback (MqttCallback);
    }
}


void loop()
{
    //initial testing purposes.
    if (digitalRead (ESP8266_flashBtn) == HIGH)
        digitalWrite (ESP8266_userBtn, HIGH);
    else
        digitalWrite (ESP8266_userBtn, LOW);

    //check if something arrived

    //run the display loop
}
