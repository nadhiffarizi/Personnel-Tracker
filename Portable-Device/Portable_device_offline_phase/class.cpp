#include "header.h"

/*Global Variables*/
SdFat sd;
SdFile myFile;
uint16_t chip_select = D8;

const int capacity = JSON_OBJECT_SIZE(4) + 60;
DynamicJsonBuffer jsonBuffer(capacity);

/*Implementation AccessPoint Class*/
//properties
String AccessPoint::SSID_to_connect(){ 
  return ssid;}

String AccessPoint::pass_to_connect() { 
  return pass; }

uint16_t AccessPoint::totalScanNetwork(){
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
void DataAcquisition::initialize(JsonObject* jobj){
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
}

bool DataAcquisition::getRSSI(JsonObject* dataNow){
  //scan network(s)
  uint16_t n = WiFi.scanNetworks();

  //search if the scanned network match the SSIDmeasure list
  uint16_t totalData = 0;
  
  String BSSID_eduroam = "54:EC:2F:15:59:A8"; //depends on particular AP to measure

  for(int i=0; i<n; i++){
    Serial.print(WiFi.SSID(i));
    Serial.print(" BSSID: ");
    Serial.println(WiFi.BSSIDstr(i));
    if( (*dataNow).containsKey(WiFi.SSID(i)) ){
      if (WiFi.SSID(i) == "eduroam"){
        if (WiFi.BSSIDstr(i) == BSSID_eduroam){
           (*dataNow)[WiFi.SSID(i)] = WiFi.RSSI(i); // push only if BBSID same as BSSID_eduroam
        }
      }
      else{
        (*dataNow)[WiFi.SSID(i)] = WiFi.RSSI(i);
      }
      
      
    }
  }
}
