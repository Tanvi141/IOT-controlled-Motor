#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include "ThingSpeak.h"
#include <Wire.h>
#include <Arduino.h>

//char * wifi_ssid = "esw-m19@iiith";
//char * wifi_pwd = "e5W-eMai@3!20hOct";

//char * wifi_ssid = "Jimin <3";
//char * wifi_pwd = "55025502";

char * wifi_ssid = "JioFi_10317B3";
char * wifi_pwd = "dfzb9thvhc";

String cse_ip = "onem2m.iiit.ac.in";

//StaticJsonBuffer < 200 > jsonBuffer;
//JsonObject & user_data = jsonBuffer.createObject();
//JsonObject & temp_user_data = jsonBuffer.createObject();
//JsonObject & sensor_data = jsonBuffer.createObject();

//Onemtom
String cse_port = "443";
String server = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";

//Thingspeak
const char * myWriteAPIKey = "NK3AMTK25MUQDCP7";
const char * server_t = "api.thingspeak.com";
unsigned long myChannelNumber = 906378;
String myStatus = "";

WiFiClient client;
unsigned long myTalkBackID = 36107;
const char * myTalkBackKey = "BQG33AKQC4K3NSOE" ;



String createCI(String server, String ae, String cnt, String val) {

    HTTPClient http;
    http.begin(server + ae + "/" + cnt + "/");

    http.addHeader("X-M2M-Origin", "admin:admin");
    http.addHeader("Content-Type", "application/json;ty=4");
    http.addHeader("Content-Length", "100");
    http.addHeader("Connection", "close");
    int code = http.POST("{\"m2m:cin\": {\"cnf\": \"text/plain:0\",\"con\": " + String(val) + "}}");
    http.end();
    Serial.print(code);
    delay(300);
    //return something 
    return (String(code)); 
}

// defines pins numbers
const long trigPin1 = 2;
const long echoPin1 = 5;
const long trigPin2 = 16;
const long echoPin2 = 17;

const long motorPin1 = 25;
const long motorPin2 = 26;
const long encoder_pin = 18;

// defines variables
long duration;
long distance;
long p = 100;
int pulsesperturn = 1;
long pulses, rpm, timeold, timeold_post,timeold_m;
int typeflag=4;
void IRAM_ATTR isr() {
    pulses++;
    //Serial.println(pulses);

}

void setup() {
    Serial.begin(115200);
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    ThingSpeak.begin(client);  // Initialize ThingSpeak
    WiFi.disconnect();
    delay(100);

    WiFi.begin(wifi_ssid, wifi_pwd);
    timeold_post = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - timeold_post <= 10000) {
        delay(500);
        Serial.println("Connecting to WiFi.. ");
    }
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Unable to connect to Wifi\n");
    } else {
        Serial.println("Connection successful!\n");
    }
    pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin1, INPUT); // Sets the echoPin as an Input
    pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin2, INPUT); // Sets the echoPin as an Input
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    digitalWrite(motorPin1, LOW); //permanent
    pinMode(18, INPUT_PULLUP); // the interrupt pin
    attachInterrupt(18, isr, FALLING); // falling edge ie when there is hole
    pulses = 0; // initializing pulses
    rpm = 0; // intializing rpm
    timeold = millis(); // initailizng time
    timeold_post = millis(); //posting to thingspeak server
    timeold_m = millis(); //posting to om2m server
    
}

// General function to POST to ThingSpeak
int httpPOST(String uri, String postMessage, String &response,long d){

  bool connectSuccess = false;
  connectSuccess = client.connect("api.thingspeak.com",80);

  if(!connectSuccess){
      return -301;   
  }
  
  postMessage += "&headers=false";
  
  String Headers =  String("POST ") + uri + String(" HTTP/1.1\r\n") +
                    String("Host: api.thingspeak.com\r\n") +
                    String("Content-Type: application/x-www-form-urlencoded\r\n") +
                    String("Connection: close\r\n") +
                    String("Content-Length: ") + String(postMessage.length()) +
                    String("\r\n\r\n");

  client.print(Headers);
  client.print(postMessage);

  long startWaitForResponseAt = millis();
  while(client.available() == 0 && millis() - startWaitForResponseAt < 5000){
      if (d >= 60) {
        digitalWrite(motorPin2, HIGH);
        delay(p);
    } else if (d <= 10) {
        digitalWrite(motorPin2, LOW);
        delay(p);
    } else {
        int delay = (d - 10) * 2;
        digitalWrite(motorPin2, HIGH);
        delayMicroseconds(delay * 500);
        digitalWrite(motorPin2, LOW);
        int dd = p - delay;
        delayMicroseconds(dd * 10);
    }
  }

  if(client.available() == 0){       
    return -304; // Didn't get server response in time
  }

  if(!client.find(const_cast<char *>("HTTP/1.1"))){
      return -303; // Couldn't parse response (didn't find HTTP/1.1)
  }
  
  int status = client.parseInt();
  if(status != 200){
    return status;
  }

  if(!client.find(const_cast<char *>("\n\r\n"))){
    return -303;
  }

  String tempString = String(client.readString());
  response = tempString;
  
  return status;
    
}

long dist(long trigPin, long echoPin) {
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH); // Sets the trigPin on HIGH state for 10 micro seconds
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2;
    // Prlongs the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.println(distance);
    return distance;
}

void thingspeak_post(long distance_1, long distance_2)
{
  Serial.println("Trying to post\n");
        timeold_post = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - timeold_post <= 10000) {
            delay(500);
            Serial.println("Connecting to WiFi.. ");
        }
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Unable to connect to Wifi\n");
        } else {
            Serial.println("Connection successful :) \n");

            // set the fields with the valuesM
            ThingSpeak.setField(1, distance_1);
            ThingSpeak.setField(2, distance_2);
            ThingSpeak.setField(3, rpm);

              // write to the ThingSpeak channel
              int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
              if(x == 200){
              Serial.println("Channel update successful.");
              }
              else{
              Serial.println("Problem updating channel. HTTP error code " + String(x));
              }

            
            //Updating the time
            timeold_post = millis();
        }
}

void onem2m_post(long distance_1, long distance_2)
{
         Serial.println("Trying to post to OM2M\n");
        timeold_m = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - timeold_m <= 10000) {
            delay(500);
            Serial.println("Connecting to WiFi.. ");
        }
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Unable to connect to Wifi\n");
        } else {
            Serial.println("Connection successful :) \n");

            // when single sensor gives multiple values
            String sensor1 = "(" + String(distance_1) + "," + String(distance_2) + ")";

            //when single sensor gives single value
            String sensor2 = String(rpm);

            // Make it a single string
            String sensor_string = sensor1 + "," + sensor2;

            // Make it OneM2M complaint
            sensor_string = "\"" + sensor_string + "\""; // DO NOT CHANGE THIS LINE

            // Send data to OneM2M server
            Serial.println(createCI(server, "Team16_IOT_Wireless_Control", "node_1", sensor_string));
            Serial.println("Posted to Onemtom\n");

            //Updating the time
            timeold_m = millis();
        } 
}

void talkback(long d)
{
    // Create the TalkBack URI
  String tbURI = String("/talkbacks/") + String(myTalkBackID) + String("/commands/execute");
  
  // Create the message body for the POST out of the values
  String postMessage =  String("api_key=") + String(myTalkBackKey);                      
                       
   // Make a string for any commands that might be in the queue
  String newCommand = String();

  // Make the POST to ThingSpeak
  int x = httpPOST(tbURI, postMessage, newCommand,d);
  client.stop();
  
  // Check the result
  if(x == 200){
    Serial.println("checking queue..."); 
    // check for a command returned from TalkBack
    if(newCommand.length() != 0){

      Serial.print("  Latest command from queue: ");
      Serial.println(newCommand);
      
      if(newCommand == "STOP"){
        typeflag=1;
      }

      if(newCommand == "SLOW"){
        typeflag=2;
      }
      if(newCommand == "FAST"){
        typeflag=3;
      }
      if(newCommand == "GO"){
        typeflag=4;
      }
    }
    else{
      Serial.println("  Nothing new.");  
    }
    
  }
  else{ 
    Serial.println("Problem checking queue. HTTP error code " + String(x));
  }
}

void loop() {

    //creating the PWM
    long ontime;
    long d = dist(trigPin1, echoPin1);
    long distance_1 = d;
    long d2 = dist(trigPin2, echoPin2);
    long distance_2 = d2;
    if (d2 < d) d = d2;

    if(typeflag==4);
    else if (typeflag==1)
      d=0;
    else if(typeflag==2)
      d=12;
    else if (typeflag==3)
      d=100;

    if (d >= 60) {
        digitalWrite(motorPin2, HIGH);
        delay(p);
    } else if (d <= 10) {
        digitalWrite(motorPin2, LOW);
        delay(p);
    } else {
        int delay = (d - 10) * 2;
        digitalWrite(motorPin2, HIGH);
        delayMicroseconds(delay * 500);
        digitalWrite(motorPin2, LOW);
        int dd = p - delay;
        delayMicroseconds(dd * 10);
    }

    if (millis() - timeold >= 5000) {
        rpm = ((60 * 1000 / pulsesperturn) / (millis() - timeold) * pulses) / 36;
        rpm/=20;
        Serial.print("--------------------------------------------------------------   RPM = ");
        pulses = 0;
        Serial.println(rpm);
        talkback(d);
        timeold = millis();
    }

    if (millis() - timeold_post >= 60000) {
        thingspeak_post(distance_1, distance_2);
    }
    if (millis() - timeold_m >= 300000) {
        onem2m_post(distance_1, distance_2);
    }
}
