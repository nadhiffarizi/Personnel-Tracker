#ifndef HEADER_H

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SdFat.h>
#include <RTClib.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include "sdios.h"

/*define*/
#define delay_WiFi 1000
#define delay_SD 1000
#define delay_Program 500
#define PERIODIC 30000      
#define MAX_SSID_LEN 32
#define MAX_CONNECT_TIME 3000
#define ACCEPT_TYPE "application/json"
#define Content_Type "application/json"



/*Global variable*/
extern SdFat sd;
extern SdFile myFile;
extern uint16_t chip_select;
extern RTC_DS3231 rtc;

/*Class Definition*/
class Employee{
    private:
        /*PRIVATE VARIABLES*/
        String ID;
    public:
        /*CONSTRUCTOR*/
        Employee();

        /*PROPERTIES*/
        String getID();

        /*METHOD*/
        //read name and ID from SD Card
        void getIdentity(SdFile* myFile);
        
};

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
        void initialize(JsonObject* jobj, String ID);
        //method getRSSI
        bool getRSSI(JsonObject* dataNow);
        //method senddata
        bool sendDataNow(JsonObject* dataNow);
};

class Backup{
    private:
        /*PRIVATE VARIABLES*/
        uint16_t _maxRecord = 5;
        int _countRecord;

        /*PRIVATE METHOD*/
        //store data if already reached maximum record (20 record)
        void storeDataMax(JsonObject* dataNow); 

        //store data if data record dont reach maximum
        void storeDataNormal(JsonObject* dataNow);
    public:
        /*METHOD*/
        //get the number of current record, maximum 30 backup record (can change)
        int countRecord();

        //Get backed up data and save in dataNow Json data structure
        void getBackupData(JsonObject* dataNow); 

        //update record once portable device success to send message (after getBackupData)
        void getUpdate();

        //Store dataNow in txt file inside SD Card
        void storeData(JsonObject* dataNow, int _countRecord);
};

#endif
