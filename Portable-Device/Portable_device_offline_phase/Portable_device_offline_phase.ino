#include "header.h"

AccessPoint AP;
DataAcquisition dataAc;
const int capacity = JSON_OBJECT_SIZE(4) + 60; 
DynamicJsonBuffer buff(capacity); 
JsonObject& dataNow = buff.createObject(); //object created with 0 member

/*initialization variable*/
long lastMillis;
bool is_the_first_time;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  /*Set portable device as WiFi station*/
  WiFi.mode(WIFI_STA);
  delay(delay_WiFi);

  /*initialize Sd card*/
  if(!sd.begin(chip_select, SPI_HALF_SPEED)) sd.initErrorHalt("SD card failed to initialize");
  delay(delay_SD);

  /*try connect to gateaway*/
  AP.connectToGateaway();
  
  /*Data acquisition initialize*/
  dataAc.initialize(&dataNow);
  dataNow.prettyPrintTo(Serial);

  /*clientinsecure*/
  client.setInsecure();

  is_the_first_time = true;
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!AP.isConnected() && (millis() - lastMillis)>= PERIODIC){
    // if last status not send or not currently connected to AP. At the same time time hits PERIODIC sec or more 
    //get data now
    Serial.println("Case 1");
    dataAc.getRSSI(&dataNow);
    dataNow.prettyPrintTo(Serial);

    //try to connect again
    AP.connectToGateaway();

    //update last millis
    lastMillis = millis();
  }
  
  else if((millis() - lastMillis) >= PERIODIC){
    // This is the condition when time hits PERIODIC or more and device is connected to AP
    Serial.println("Case 2");
    
    //get dataNow
    dataAc.getRSSI(&dataNow);
    dataNow.prettyPrintTo(Serial);

    //send data to spreadsheet
    sendData(&dataNow, &is_the_first_time);
    
    // dataNow["AP1"] = 0;
    // dataNow["AP2"] = 0;
    // dataNow["AP3"] = 0;
    // dataNow["AP4"] = 0;
    // dataNow["Ruang KP BME"] = 0;
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


