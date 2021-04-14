#include "header.h"

/*initialization object*/
Employee emp;             //Object for employee. Consist of name and id employee
AccessPoint AP;           //Object AP for connecting to gateaway
DataAcquisition dataAc;   //Object for data acquisition (RSSI)
Backup backup;            //Object for backup data for storing data when failed to connect to gateaway

const int capacity = JSON_OBJECT_SIZE(4) + 60; 
DynamicJsonBuffer buff(capacity); 
JsonObject& dataNow = buff.createObject(); //object created with 0 member

/*initialization variable*/
long lastMillis;

void setup() {
  /* Serial setup*/
  Serial.begin(115200);

  /*Set portable device as WiFi station*/
  WiFi.mode(WIFI_STA);
  delay(delay_WiFi);

  /*initialize Sd card*/
  if(!sd.begin(chip_select, SPI_HALF_SPEED)) sd.initErrorHalt("SD card failed to initialize");
  delay(delay_SD);

  /*Check and get identity employee data from SD Card*/
  emp.getIdentity(&myFile);
  // Serial.println(emp.getID());

  /*initialize RTC*/
  if(!rtc.begin()){
    Serial.println("Couldn't find RTC");
    while(1);
  }

  /*try connect to gateaway*/
  AP.connectToGateaway();
  
  /*Data acquisition initialize*/
  dataAc.initialize(&dataNow, emp.getID());
  dataNow.prettyPrintTo(Serial);
  
}

void loop() {

  if((!AP.isConnected() || !dataAc.statusSend()) && (millis() - lastMillis)>= PERIODIC){
    // if last status not send or not currently connected to AP. At the same time time hits PERIODIC sec or more 
    //get data now
    Serial.println("Case 1");
    dataAc.getRSSI(&dataNow);
    dataNow.prettyPrintTo(Serial);

    //save to pending data in backup.txt
    backup.storeData(&dataNow, backup.countRecord());

    //try to connect again
    AP.connectToGateaway();

    //update last millis
    lastMillis = millis();
  }
  
  else if((millis() - lastMillis) >= PERIODIC){
    // This is the condition when time hits PERIODIC or more and device is connected to AP
    Serial.println("Case 2");

    //check pending data and get pending data
    if(backup.countRecord() != 0){
      backup.getBackupData(&dataNow);

      //send pending data in backup.txt
      dataAc.sendDataNow(&dataNow);
      if(dataAc.statusSend()){
        backup.getUpdate();
      }
      
      //delay
      delay(delay_Program);
    }
    
    //get dataNow
    dataAc.getRSSI(&dataNow);
    dataNow.prettyPrintTo(Serial);

    //send dataNow
    dataAc.sendDataNow(&dataNow);

    //checking sended or not. If not store in backup data
    if(!dataAc.statusSend()){
      backup.storeData(&dataNow, backup.countRecord());
    }
    
    //update last millis
    lastMillis = millis();

    //delay 
    delay(delay_Program);
  }
  
  else if (!AP.isConnected() && ((millis() - lastMillis) < PERIODIC)){
    // use this period of time to send retry connection

    Serial.println("Case 3");
    //try to connect again
    AP.connectToGateaway();

  }
  
  delay(50);
}




