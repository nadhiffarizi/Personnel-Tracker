#include "header.h"

/*CONNECTION TO SPREADSHEET*/
/*host and https port*/
const char* host = "script.google.com";
const int httpsPort = 443;
/*create wificlientsecure object*/
WiFiClientSecure client;
/*spreadsheet script ID*/
String GAS_ID = "<Fill with script ID from google-sheet>"; //--> spreadsheet script ID


String FormattedString(String x)
{
  int i=0; int from=0;
  String first_part_string;
  String second_part_string;
  String temp;
  x.replace(" ", "%20");
  x.replace("+", "%2B");
  x.replace("_", "%5F");
  x.replace("-", "%2D");

  return x;
}


/*=============================================================================*/
/*SENDING DATA*/

/*Sending values to spreadsheet*/
void sendData(JsonObject* dataNow, bool *is_the_first_time)
{

   Serial.println("==========");
   Serial.print("connecting to ");
   Serial.println(host);

  /*connect to google host*/
  if(!client.connect(host, httpsPort))
  {
    Serial.println("connection failed");
    return;
  }

  String url;
  if(*is_the_first_time)
  {
    *is_the_first_time = false;
    String header1 = FormattedString("AP1");
    String header2 = FormattedString("AP2");
    String header3 = FormattedString("AP3");
    String header4 = FormattedString("AP4");
    String header5 = FormattedString("eduroam");
    url = "/macros/s/" + GAS_ID + "/exec?rssi1="+ header1
                + "&rssi2=" + header2
                + "&rssi3=" + header3
                + "&rssi4=" + header4
                + "&rssi5=" + header5;
  }
  else
  {
    long data1 = (*dataNow)["AP1"];
    long data2 = (*dataNow)["AP2"];
    long data3 = (*dataNow)["AP3"];
    long data4 = (*dataNow)["AP4"];
    long data5 = (*dataNow)["eduroam"];
    String rssi1_data = String(data1);
    String rssi2_data = String(data2);
    String rssi3_data = String(data3);
    String rssi4_data = String(data4);
    String rssi5_data = String(data5);
    url = "/macros/s/" + GAS_ID + "/exec?rssi1="+ rssi1_data
                + "&rssi2=" + rssi2_data
                + "&rssi3=" + rssi3_data
                + "&rssi4=" + rssi4_data
                + "&rssi5=" + rssi5_data;
  }
  

  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");
   delay(delay_WiFi);
  Serial.println("request sent");
  //----------------------------------------

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
  delay(delay_WiFi);
 
}
