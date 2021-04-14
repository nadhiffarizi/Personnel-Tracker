#include "header.h"

/*Global Variables*/
SdFat sd;
SdFile myFile;
uint16_t chip_select = D8;
RTC_DS3231 rtc;

const int capacity = JSON_OBJECT_SIZE(4) + 60;
DynamicJsonBuffer jsonBuffer(capacity);


/*Implementation Employee Class*/
Employee::Employee(){
    ID = "";
}

//properties
String Employee::getID(){
    return ID;
}

//method
void Employee::getIdentity(SdFile* myFile){

    if(!myFile->open("identity.txt", O_READ)) sd.errorHalt("Cannot open file");

    //can open file
    Serial.println("identity.txt has been opened: ");
       
    int read_byte;
    String data;
    while((read_byte = myFile->read()) >= 0){
        char c = read_byte;
        if(c== ';')
          ID.concat(data);
        else{
          data+=c;
        }
    }
    myFile->close();
    
}

/*Implementation AccessPoint Class*/
//properties
String AccessPoint::SSID_to_connect(){ 
  return ssid;}

String AccessPoint::pass_to_connect() { 
  return pass; }

uint16_t  AccessPoint::totalScanNetwork(){
  return total_scan_network;
}

bool AccessPoint::isConnected() {
  if(WiFi.status() == WL_CONNECTED){
    is_connected = true;
  }
  else{
    is_connected = true;
  }
  return is_connected;
}

//Method
bool AccessPoint::searchPass(SdFile* myFile, const char* ssid_search) {
  pass = "";
  ssid="";
  //vars 
  bool check = true;
  bool readPass = false;
  //try to open myfile
  if(!myFile->open("ssidpass.txt", O_READ)) sd.errorHalt("Cannot open file");

  //success
  Serial.println("Opening ssidpass.txt.. :");

  int read_byte;
  char c;
  String data="";
  int i=0; 
  while((read_byte = myFile->read()) >=0){
    c = read_byte;
    if(c == *(ssid_search+i) && check == true && !readPass){
      // checks ssid name
      check = true;
      ssid += c;
      i+=1;
    }
    else if(c == ';' && check == true && !readPass){
      // if ssid name confirmed, scans for password
      i=0;
      readPass = true;
    }
    else if(readPass){
      // starts collecting password
      if (c != '\r'){
        pass += c;
      }
      else{
        myFile->close();
        return check;
        break;
      }
    }
    else{
      // new line
      if( c == '\n'){
        check = true;
      }
      else{
        check = false;
      }
      ssid = "";
      i=0;
      
    }

  }
  myFile->close();
  return check;
}

bool AccessPoint::connectSSID(const char* ssid_search){
     
  if(!searchPass(&myFile, ssid_search)) {
    Serial.println("Can't find password");
    return false;
  }
  else{
    // String _ssid = "skp";      
    // String _password = "iburia142";
    // Serial.println(ssid);
    // Serial.println(ssid.length());
    // Serial.println(pass);
    // Serial.println(pass.length());
    WiFi.begin(ssid, pass);
    delay(delay_WiFi);
    long last_millis = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(delay_WiFi);
      Serial.print("connecting.... ");
      // WiFi.begin(ssid_search, pass);
    }
    // WiFi.begin(ssid_search, pass);
    delay(delay_WiFi);
    delay(delay_WiFi);
    delay(delay_WiFi);
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Can't connect to network : ");
      Serial.print(ssid);
      Serial.print(" ");
      Serial.println(pass);
      is_connected = false;
      return false;
    }
    else{
      Serial.print("Connected to: ");
      Serial.print(ssid);
      Serial.print(" ");
      Serial.println(pass);
      is_connected = true;
      return true;
    }
      
    
  }
}

bool AccessPoint::connectToGateaway(){
  total_scan_network = WiFi.scanNetworks();
  
  if(total_scan_network == 0){
    Serial.println("There's no gateaway available");
    return total_scan_network;
  }

  //if scanned network is not 0
  Serial.print(total_scan_network);
  Serial.println("Network(s)");
  
  //create indices buat pencacah wifi.rssi
  uint16_t indices[total_scan_network];
  for(int i=0; i<total_scan_network; i++){
    indices[i] = i;
  }

   /*sorting from the strongest to lowest power*/
  for(int i=0; i<total_scan_network; i++){
    for(int j=i+1; j<total_scan_network; j++){
      if(WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])){
        std::swap(indices[i], indices[j]);
      }
    }
  }

  /*Trying to connect from available network(s)*/
  char ssid_search[32] = "";
  for(int i=0; i<total_scan_network; i++){
    memset(ssid_search, 0, MAX_SSID_LEN);
    strncpy(ssid_search, WiFi.SSID(indices[i]).c_str(), MAX_SSID_LEN);
    Serial.println(ssid_search);
    if(connectSSID(ssid_search)){
      return true;
    }
    delay(delay_WiFi);
  }
  return false;
}

/*Implementation Data Acquisition Class*/
void DataAcquisition::initialize(JsonObject* jobj, String ID){
  //initialize first _statusSend state
  _statusSend = true;

  Serial.println("Initialize dataNow to get list AP to scan and statusSend becomes true");

  //open SSIDmeasure from sd card
  if(!myFile.open("SSIDmeasure.txt", O_READ)) sd.errorHalt("Cannot open file");

  //File success opened
  Serial.println("SSIDmeasure.txt opened");

  //Reads data from txt file
  int read_byte;
  String data = "";
  String key_name;
  while((read_byte = myFile.read())>= 0 ){
    char c = read_byte;
    //Serial.write(read_byte);
    if(c == ';'){
      key_name.concat(data);
      // Serial.println(key_name);
      (*jobj)[key_name] = 0;
      key_name = "";
    }
    else{
     if(c != '\n'){
        data+=c;
      }
      else{
        data="";
      }
    }
  }  
  myFile.close();

  //initialize key for time data
  (*jobj)["time"] = "";
  (*jobj)["ID"] = ID;
}

bool DataAcquisition::getRSSI(JsonObject* dataNow){
  //scan network(s)
  uint16_t n = WiFi.scanNetworks();

  //search if the scanned network match the SSIDmeasure list
  uint16_t totalData = 0;
  
  for(int i=0; i<n; i++){
    // Serial.println(WiFi.SSID(i));
    if( (*dataNow).containsKey(WiFi.SSID(i)) && totalData<4){
      (*dataNow)[WiFi.SSID(i)] = WiFi.RSSI(i);
      totalData +=1;

      DateTime now = rtc.now();
      char buf[] = "YYMMDD-hh:mm:ss";
      // Serial.println(now.toString(buf));
      (*dataNow)["time"] = now.toString(buf);
      // return true;
      //push timestamp data to dataNow
      // if(totalData == 4){
      //   String _timeNow;
      //   DateTime now = rtc.now();
      //   char buf[] = "YYMMDD-hh:mm:ss";
      //   Serial.println(now.toString(buf));
      //   (*dataNow)["time"] = now.toString(buf);

      //   //at this point, data acquisition succeed and return
      //   return true;
      // }
      
    }
    // else{
    //   Serial.println("Data is not valid");
      
    //   //if data is not valid, then reset the Json and return false
      
    //   return false;
    // }
  }
}

bool DataAcquisition::statusSend(){
  return _statusSend;
}

bool DataAcquisition::sendDataNow(JsonObject* dataNow){

  /* this function is for push received data to apache nifi, http://<host-address>:<PORT-number>/contentListener  */
  const char* server = "http://192.168.100.13:7001/contentListener"; //IP may vary

  //http object
  HTTPClient http;

  // String message = "{\"bejo\":-56, \"beji\": -90, \"haha\": -60, \"hihi\: -70}";
  String msg;
  (*dataNow).printTo(msg);
  String fullRequestHTTP = String(server);
  http.begin(fullRequestHTTP);
  http.addHeader("Accept", ACCEPT_TYPE, false, false);
  http.addHeader("Content-Type", Content_Type, false, false);
  Serial.println(fullRequestHTTP);

  int httpcode = http.sendRequest("POST", msg);
  Serial.println(httpcode);

  delay(1000);
}


/*implementation Backup Class*/
int Backup::countRecord(){
  //open file 
  if(!myFile.open("backup.txt", O_READ)) sd.errorHalt("Cannot open file");

  //checking record
  int read_byte;
  _countRecord =0;

  while((read_byte = myFile.read()) >= 0){
    char c = read_byte;
    if(c == '\n'){
      _countRecord +=1;
    }
  }
  myFile.close();
  return _countRecord;
}

void Backup::getBackupData(JsonObject* dataNow){
  //open file from sd card
  if(!myFile.open("backup.txt", O_READ))  sd.errorHalt("Cannot open file");

  //prompt message success
  Serial.println("Opening backup.txt :");

  //scan and save in local variable
  int read_byte;
  String data= "";

  while ((read_byte = myFile.read()) >= 0){
    char c = read_byte;
    //Serial.println(data);
    if(c == ';'){
      // "{"skp":-20,"Ap2":-90,"Ap3":-50,"timeNow":"210201-18:06:59"};"
      //create Json buffer and store the first record using (*dataNow)
      
      JsonObject& root = jsonBuffer.parseObject(data);
      delay(1000);

      //Error parsing checking
      if(!root.success()){
        Serial.println("Parse failed");
      }
      else{
        //save in dataNow Json Object
        //depends on what the JSON structure for dataNow
        int skp = root["skp"]; 
        int Ap2 = root["Ap2"]; 
        int Ap3 = root["Ap3"]; 
        const char* timeNow = root["time"];
        
        (*dataNow)["skp"] = skp;
        (*dataNow)["Ap2"] = Ap2;
        (*dataNow)["Ap3"] = Ap3;
        (*dataNow)["time"] = String(timeNow);

        (*dataNow).prettyPrintTo(Serial);
      }

      //reset data
      data="";

      //close file
      myFile.close();
      break;
    }
    else{
      data+= c;
    }
  }
}

void Backup::getUpdate(){
  //open backup.txt
  if(!myFile.open("backup.txt", O_READ)) sd.errorHalt("Cannot open file backup.txt");
  //get the second line data until EOF
  bool _secondLine = false;
  int read_byte;
  String temp = "";
  while((read_byte = myFile.read()) >= 0){
    char c = read_byte;
    if(c == '\n' && !_secondLine){
      _secondLine = true;
    }
    else if(_secondLine){
      temp+=c;
    }
  }
  
  myFile.close();
  Serial.println(temp);
  
  
  //create temp file
  if(!myFile.open("temp.txt", O_WRITE|O_CREAT)) sd.errorHalt("Cannot create file temp.txt");

  //copy from second line until eof to temp file
  myFile.println(temp.c_str());
  myFile.close();

  //reset string
  temp="";

  //delete backup file
  sd.remove("backup.txt");

  //rename temp file with "backup.txt"
  sd.rename("temp.txt", "backup.txt");
}

void Backup::storeData(JsonObject* dataNow, int _countRecord){
  if(_countRecord< _maxRecord){
    Backup::storeDataNormal(dataNow);
  }
  else{
    //if total record is already at maximum (20)
    Backup::storeDataMax(dataNow);
  }
}

void Backup::storeDataNormal(JsonObject* dataNow){
   
  //Prepare dataNow structure in string
  String _dataNow;
  (*dataNow).printTo(_dataNow);
  _dataNow = _dataNow + ";";
  // Serial.println(_dataNow);

  //open backup file
  if(!myFile.open("backup.txt", O_READ)) sd.errorHalt("Cannot open file backup.txt");

  //get content from backup.txt
  int read_byte;
  String temp = "";
  while((read_byte = myFile.read()) >= 0){
    char c = read_byte;
    temp+=c;
  }
  myFile.close();
  Serial.println(temp.c_str());

  //open or crate temp file
  if(!myFile.open("temp.txt", O_WRITE|O_CREAT)) sd.errorHalt("Cannot create file temp.txt");

  //store dataNow to temp.txt
  myFile.println(_dataNow);
  myFile.close();

  //open or crate temp file
  if(!myFile.open("temp.txt", O_APPEND|O_WRITE)) sd.errorHalt("Cannot append file temp.txt");
  myFile.println(temp.c_str());
  _dataNow="";
  temp="";
  //close file
  myFile.close();

  //delete backup
  sd.remove("backup.txt");
  //rename temp.txt to backup.txt
  sd.rename("temp.txt", "backup.txt");
}

void Backup::storeDataMax(JsonObject* dataNow){
  Serial.println("Masuk ke storeDataMax");
  //Prepare dataNow structure in string
  String _dataNow;
  (*dataNow).printTo(_dataNow);
  _dataNow = _dataNow + ";";

  //open backup file
  if(!myFile.open("backup.txt", O_READ)) sd.errorHalt("Cannot open file backup.txt");

  //get content from backup.txt
  int read_byte;
  uint16_t _ithRecord =0;
  String temp = "";
  while((read_byte = myFile.read()) >= 0){
    char c = read_byte;
    if(c == '\n'){
      _ithRecord+=1;
      if(_ithRecord == _maxRecord-1){
        break;
      }
    }
    temp+=c;
  }
  myFile.close();
  Serial.println(temp.c_str());

  //create and write dataNow to temp.txt
  if(!myFile.open("temp.txt", O_WRITE|O_APPEND|O_CREAT)) sd.errorHalt("Cannot open file backup.txt");

  //write dataNow to temp.txt
  myFile.println(_dataNow.c_str());
  //write temp string content to temp.txt
  myFile.println(temp.c_str());

  //close temp.txt
  myFile.close();

  //remove backup.txt
  sd.remove("backup.txt");

  //rename temp.txt
  sd.rename("temp.txt", "backup.txt");

  _dataNow="";
  temp="";
}