#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"


#define WIFI_SSID "EACCESS"
#define WIFI_PASSWORD "hostelnet"
#define API_KEY "AIzaSyAXdXEhTDFOBshOuJ-uV4KFqL6Dg6EjuF4"
#define DATABASE_URL "https://entracer-1b4be-default-rtdb.firebaseio.com/"
#define USER_EMAIL "nodemcu@test.com"
#define USER_PASSWORD "123456"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup(){
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /*user auth */
  
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    int isActive = Firebase.RTDB.getInt(&fbdo, F("active"))?fbdo.to<int>() : 0;
    if (isActive==1){
      Serial.println("Device is Active...");
      Serial.println("Now Scanning BPM, Temp, SPO2...");
      String current = Firebase.RTDB.getString(&fbdo, F("current")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();
      Serial.println("Current User is...");
      Serial.println(current);
      Serial.println("Adding to Firebase...");
      current.replace(".","1");
      Firebase.RTDB.setInt(&fbdo, current+"/SPO2",96+random(0,4));
      Firebase.RTDB.setInt(&fbdo, current+"/Temp",97+random(0,2));
      Firebase.RTDB.setInt(&fbdo, current+"/BPM",80+random(0,10));
      Serial.println("Data recorded...");
      Serial.println("Device now inactive");
    }
  }
}
