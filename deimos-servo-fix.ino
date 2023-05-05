#include <Arduino.h>
#include <ESP32_Servo.h>
#include <WiFi.h>
//#include <Firebase_ESP_Client.h>
#include <FirebaseESP32.h>

//token info n
#include "addons/TokenHelper.h"
//realtime database helper
#include "addons/RTDBHelper.h"

// Insert wifi
#define WIFI_SSID "SolocornSTP"
#define WIFI_PASSWORD "merdeka1945"

// Insert Firebase project API Key
#define API_KEY ""

// Insert RTDB
#define DATABASE_URL "" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//Define variable
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
Servo myservo; //servo object
int pos; //servo position
const int servoPin = 14;


void setup(){
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //connecting wifi
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Assign the api key (required
  config.api_key = API_KEY;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  //
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

     //pin servo
  myservo.attach(servoPin);
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 8000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    
      if (Firebase.RTDB.getInt(&fbdo, "servo")) {
            if (fbdo.dataType() == "int") {
              pos = fbdo.intData();
               if (pos != 0 && pos != 110) {
                pos = 0; // or pos = 90;
              }
              
              // code for controlling the speed of the servo
              for (int i = myservo.read(); i != pos; i = i < pos ? i + 1 : i - 1) {
                myservo.write(i);
                delay(15);
              }
              
              myservo.write(pos);
              Serial.println("Servo position updated: " + String(pos));
            }
          }
          else {
            Serial.println(fbdo.errorReason());
          }    
  }
}
