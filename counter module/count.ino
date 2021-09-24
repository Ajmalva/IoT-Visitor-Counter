#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include<WiFiManager.h>                 
#include <FirebaseArduino.h>      
#include "ThingSpeak.h"

#define FIREBASE_HOST "xxxxxxxxxxxx.firebaseio.com"      
#define FIREBASE_AUTH "##############################"  

unsigned long myChannelNumber = #####;
const char * myWriteAPIKey = "######";         
           
#define iir 2  //GPIO 2
#define oir 0  //GPIO 0
int count = 0;
int pcount = 0;
int cval = 0;
int pval = 0;
int no = 0;
int yes = 1; 
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 15000;  //the value is a number of milliseconds
WiFiClient  client;
void setup() 
{
  Serial.begin(115200);
  delay(1000);
  WiFiManager wifiManager;
  wifiManager.autoConnect("AjEspCount");
  Serial.println("Connected.....");
 
  Serial.println();
  Serial.print("Connected");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // connect to the firebase

  startMillis = millis();  //initial start time
  Serial.println("mills");
  Serial.println(startMillis);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  
 pinMode(iir,INPUT);
 pinMode(oir,INPUT);
}
 
void loop() 
{
if (digitalRead(iir))
  {
    count++;
  }
if (digitalRead(oir))
  {
    count--;
  }

if (count <= 0)
  {
    if (count != pcount){
      pcount = count;
      count = 0; 
      Serial.println(count);
      Firebase.setInt("/stat", no);
      Firebase.setInt("/count", count);
      if (Firebase.failed()) 
      {
      Serial.print("pushing /logs failed:");
      Serial.println(Firebase.error()); 
      }
    }
  }
 else 
  {
    if (count != pcount){
      pcount = count;
      Serial.println(count);
      Firebase.setInt("/stat", yes); 
      Firebase.setInt("/count", count);   
      if (Firebase.failed()) 
        {
          Serial.print("pushing /logs failed:");
          Serial.println(Firebase.error()); 
        }
     }
  }
 currentMillis = millis();
 if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  { 
    cval = count;
    if (cval != pval){
    pval = cval;
    ThingSpeak.setField(1, cval);
    // write to the ThingSpeak channel
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
   }
  }
  }
