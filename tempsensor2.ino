#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <time.h>

#define myPeriodic 60 //in sec | Thingspeak pub is 15sec
#define ONE_WIRE_BUS 2  // DS18B20 on arduino pin2 corresponds to D4 on physical board

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float prevTemp = 0;
const char* server = "api.thingspeak.com";

const char* aws_server = "ec2-52-12-68-77.us-west-2.compute.amazonaws.com";

String apiKey ="8KOT9BRJ2G6B2S34";
const char* MY_SSID = "SK_WiFiGIGAA360"; 
const char* MY_PWD = "1809085034";
int sent = 0;
void setup() {
  Serial.begin(115200);
  connectWifi();
}

void sendAWS(float);

void loop() {
  float temp;
  
  DS18B20.requestTemperatures(); 
  temp = DS18B20.getTempCByIndex(0);
  Serial.print(String(sent)+" Temperature: ");
  Serial.println(temp);
  
  sendTeperatureTS(temp);
  sendAWS(temp);
  int count = myPeriodic;
  while(count--)
  delay(1000);
}

void connectWifi()
{
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");  
}//end connect

void sendTeperatureTS(float temp)
{  
   WiFiClient client;
  
   if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
   Serial.println("WiFi Client connected ");
   
   String postStr = apiKey;
   postStr += "&field1=";
   postStr += String(temp);
   postStr += "\r\n\r\n";
   
   client.print("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n\n");
   client.print(postStr);
   delay(1000);
   
   }//end if
   sent++;
 client.stop();
}//end send

void sendAWS(float temp)
{  
   WiFiClient client;
  
   if (client.connect(aws_server, 9000)) { // use ip 184.106.153.149 or api.thingspeak.com
   Serial.println("AWS WiFi Client connected ");
   
  /* String postStr="temp=";
   postStr += String(temp);
   //postStr += "\r\n\r\n";
   client.print("POST /log HTTP/1.1\n");
   client.print("Host: ec2-52-12-68-77.us-west-2.compute.amazonaws.com:9000\n");
   //client.print("Connection: close\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n\n");
   client.print(postStr);*/
   configTime(3*3600, 0, "pool.ntp.org", "time.nist.gov");
   while(!time(nullptr)){
    Serial.print("o");
    delay(1000);
   }
   time_t now=time(nullptr);
   String postStr="/log?temp=";
   postStr += String(temp);
   postStr += "&time_t=";
   postStr += String(now);
   Serial.println(postStr);
   client.print("GET " + postStr + "\n");
   client.print("Host: ec2-52-12-68-77.us-west-2.compute.amazonaws.com:9000\n");
   client.print("Connection: close\r\n\r\n\n");
   delay(1000);
   
   }//end if
 client.stop();
}//end send
