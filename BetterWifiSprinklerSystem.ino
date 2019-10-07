//Todo: update time from calendar data (ensure current time matches timezone and daylight savings)
//TODO: test that time is correct over daylight savings
//Find out if the fingerprint changes

/*
 * Node MCU 1.0
 * 160Mhz
 * 
 */

//wifi
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal

//over the air updates
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h> 

//calendar
#include "icalendar.h"
ICalendar Calendar("hi.txt");

void setup() {
  Serial.begin(115200);//serial for debugging
  Serial.println("\nStarting...");
  Serial.setDebugOutput(true);

  relaySetup();
  setUpWifiConnection();
  OTASetup();
  updateGlobalTimeFromInternet();

}


std::list<Event*>::iterator it;
tm* nowstruct;
time_t t;

unsigned long updateCalendarTime=500000;
const unsigned long updateCalendarWaitTime = updateCalendarTime;
unsigned long updateRelayTime = 1000;
const unsigned long updateRelayWaitTime = updateRelayTime;
void loop() {
  //OTA
  OTALoop();
   
  //Get calendar every 50 seconds
  if( (millis()-updateCalendarTime) > updateCalendarWaitTime){
    Serial.println("beginning memory calendar:");
    Serial.println(ESP.getFreeHeap());
    
    updateCalendarTime = millis();
    //delete Calendar;
    //Calendar = new ICalendar("blank.txt");
    for (it=Calendar.Events.begin(); it != Calendar.Events.end();++it) {
      delete (*it);
    }
    Calendar.Events.clear();//deletes old events
    DownloadAndParseCalendar();

    Serial.println("ending memory Calendar:");
    Serial.println(ESP.getFreeHeap());
  }

  //update relays every second
  if( (millis()-updateRelayTime) > updateRelayWaitTime){
    Serial.println("beginning memory:");
    Serial.println(ESP.getFreeHeap());
    
    updateRelayTime = millis();
    
    t = time(NULL);
    nowstruct = localtime(&t);
    for (it=Calendar.Events.begin(); it != Calendar.Events.end();++it) {
      if ((*it)->checkActive(nowstruct)) {
        Serial.println(((string)(**it)).c_str());
        int relay = (*it)->Summary[0]-'0';
        setRelay(relay);
      }
    }
    outputRelayState();
    resetRelayState();

    Serial.println("ending memory:");
    Serial.println(ESP.getFreeHeap());
  }
}
