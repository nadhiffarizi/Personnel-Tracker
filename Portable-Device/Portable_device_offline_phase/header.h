#ifndef HEADER_H

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <SdFat.h>
#include <RTClib.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include "sdios.h"

/*define*/
#define delay_WiFi 1000
#define delay_SD 1000
#define delay_Program 500      
#define MAX_SSID_LEN 32
#define PERIODIC 1000
#define MAX_CONNECT_TIME 3000
#define ACCEPT_TYPE "application/json"
#define Content_Type "application/json"



/*Global variable*/
extern SdFat sd;
extern SdFile myFile;
extern uint16_t chip_select;

/*CONNECTION TO SPREADSHEET*/
/*host and https port*/
extern const char* host;
extern const int httpsPort;
/*create wificlientsecure object*/
extern WiFiClientSecure client;
/*spreadsheet script ID*/
extern String GAS_ID; //--> spreadsheet script ID

/*Class Definition*/

class AccessPoint {
    private:
        /*PRIVATE VARIABLES*/
        String ssid;
        String pass;
        uint16_t total_scan_network;
        bool is_connected;
        /*METHOD*/
        bool searchPass(SdFile* myFile, const char* ssid_search);  //search password
        bool connectSSID(const char* ssid_search); //for connecting to gateaway
    public:
        /*PROPERTIES*/
        String SSID_to_connect();
        String pass_to_connect();
        uint16_t totalScanNetwork();
        bool isConnected();

        /*METHOD*/
        bool connectToGateaway(); //for scanning network available and then sorting from the strongest to weakest
};

class DataAcquisition{
    private:
        /*PRIVATE VARIABLES*/
        bool _statusSend;
        /*PRIVATE METHOD*/
        
    public:
        //properties
        bool statusSend();
        //method get AP list to obtain RSSI
        void initialize(JsonObject* jobj);
        //method getRSSI
        bool getRSSI(JsonObject* dataNow);
        //method senddata
        bool sendDataNow(JsonObject* dataNow);
};

/*Function*/
void sendData(JsonObject* dataNow, bool* is_the_first_time);
String FormattedString(String x);
#endif
