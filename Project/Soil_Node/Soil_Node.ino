#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "" // firebase host name of your project
#define FIREBASE_AUTH "" // authentication key 
#define WIFI_SSID "" // Change the name of your WIFI
#define WIFI_PASSWORD "" // Change the password of your WIFI
SoftwareSerial s(D6, D5);
int i = 0;
int motor_on=0;
int motor_off=0;
int motor_in=0;
int j=0;
void setup()
{
  WiFi.mode(WIFI_STA);
  Serial.begin(9600);
  s.begin(9600);

  while (!Serial) {
    Serial.print("....")
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println();
  if (Serial)
  {
    Serial.println("Contected");
  }
  WiFi.begin (WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}
int flag = 0;
void loop() {


  // put your main code here, to run repeatedly:
  StaticJsonBuffer<1000> staticJsonBuffer;
  JsonObject& root = staticJsonBuffer.parseObject(s);
  root.prettyPrintTo(Serial);
  int motorState = root["d1"];
  int valve1State = root["d2"];
  int valve2State = root["d3"];
  int msvalue1 = root["d4"];
  int msvalue2 = root["d5"];
  long distance = root["d10"];
  float l_hour = root["d11"];
  int ASM1 = 0;
  int BSM1 = 0;
  int ASM2 = 0;
  int BSM2 = 0;
  j=j+1;
  Serial.println(j);
  if (root.size() > 0)
  {

    Firebase.setString("Flag", "Data from arduino");
    if (valve1State == 1)
    {
      BSM1 = msvalue1;
    }
    else
    {
      ASM1 = msvalue1;
    }
    if (valve2State == 1)
    {
      BSM2 = msvalue2;
    }
    else
    {
      ASM2 = msvalue2;
    }
    Firebase.setInt("User/101/Current_Water_Level", distance);
    Firebase.setInt("User/101/Moisture_Id/SM01", msvalue1);
    Firebase.setInt("User/101/Moisture_Id/SM02", msvalue2);
    if (motorState == 1)
    { 
      motor_on=1+motor_on;
      motor_off=0;
      motor_in=1;
      Firebase.setString("User/101/Motor_indication", "ON");
    }
    else
    {
      motor_on=0;
      motor_off=1+motor_off;
      Firebase.setString("User/101/Motor_indication", "OFF");
    }
    if(motor_on==1)
    {
      motor_on_notify();
    //motor on notification
    }
    if(motor_off==1 && motor_in==1)
    {
      motor_off_notify();
      motor_in=0;
    //motor off notification
    }
    if (valve1State == 1)
    {
      Firebase.setString("User/101/Valve/V01", "ON");
    }
    else
    {
      Firebase.setString("User/101/Valve/V01", "OFF");
    }
    if (valve1State == 1)
    {
      Firebase.setString("User/101/Valve/V02", "ON");
    }
    else
    {
      Firebase.setString("User/101/Valve/V02", "OFF");
    }
    Firebase.setFloat("User/101/Water_Frequency", l_hour);
    flag = 0;
    Firebase.setInt("F", flag);
  }
  else
  {
    Firebase.setString("Flag", "No Data From arduino");
    Firebase.setInt("F", flag);
    flag = flag + 1;
  }
  if (flag > 3)
  {
    Serial.println("Restart");
    ESP.restart();
  }
  delay(1000);
}

int motor_on_notify()
{
  HTTPClient http;
  int ret = 0;
  Serial.print("[HTTP] begin...\n");
  // configure ifttt server and url  should be HTTP only..not https!!!  (http://)
  http.begin("");// ifttt url //HTTP
  

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    ret = -1;
    Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    delay(500); // wait for half sec before retry again
  }

  http.end();
  return ret;
}

int motor_off_notify()
{
  HTTPClient http;
  int ret = 0;
  Serial.print("[HTTP] begin...\n");
  // configure ifttt server and url  should be HTTP only..not https!!!  (http://)
  http.begin("");//ifttt url //HTTP
  
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    ret = -1;
    Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    delay(500); // wait for half sec before retry again
  }

  http.end();
  return ret;
}

int low_level_notify()
{
  HTTPClient http;
  int ret = 0;
  Serial.print("[HTTP] begin...\n");
  // configure ifttt server and url  should be HTTP only..not https!!!  (http://)
  http.begin("");//ifttt url //HTTP
 

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    ret = -1;
    Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    delay(500); // wait for half sec before retry again
  }
  http.end();
  return ret;
}
