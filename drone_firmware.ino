/*  
  *   Author: Eren Ozturk
  */
 
 /*
  * Network functionality:
  */
 /*  
 *  Author: Timothy Woo (www.botletics.com)
 *  Github: https://github.com/botletics/SIM7000-LTE-Shield
 *  Last Updated: 1/7/2021
 *  License: GNU GPL v3.0
  */
/******* ORIGINAL ADAFRUIT FONA LIBRARY TEXT *******/
/***************************************************
  This is an example for our Adafruit FONA Cellular Module
  Designed specifically to work with the Adafruit FONA
  ----> http://www.adafruit.com/products/1946
  ----> http://www.adafruit.com/products/1963
  ----> http://www.adafruit.com/products/2468
  ----> http://www.adafruit.com/products/2542
  These cellular modules use TTL Serial to communicate, 2 pins are
  required to interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/
 /*
  * SBUS encoding math in encodeSBUS():
  *  Author: Brian R. Taylor (Bolder Flight Systems)
  *  Github: https://github.com/bolderflight/SBUS/blob/master/src/SBUS.cpp
  *  License: GNU GPL v3.0
  */


/*
 * HARDWARE USED AS EXAMPLE:
 * Central Computer: Arduino Uno (or compatible board)
 * LTE/GPS Module: Botletics SIM7000 LTE CAT-M/NB-IoT shield
 * Flight Controller: Generic hexacopter flight controller
*/

#include "Adafruit_FONA.h" // https://github.com/botletics/SIM7000-LTE-Shield/tree/master/Code
#include <SoftwareSerial.h>
#include <elapsedMillis.h>

/************************* PIN DEFINITIONS *********************************/
// For botletics SIM7000 shield
#define FONA_PWRKEY 6
#define FONA_RST 7
//#define FONA_DTR 8 // Connect with solder jumper
//#define FONA_RI 9 // Need to enable via AT commands
#define FONA_TX 10 // Microcontroller RX
#define FONA_RX 11 // Microcontroller TX
//#define T_ALERT 12 // Connect with solder jumper

/************************* GPS/LTE MODEM VARIABLES *********************************/

#define SIMCOM_7000

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX, false);

SoftwareSerial *fonaSerial = &fonaSS;
  
Adafruit_FONA_LTE fona = Adafruit_FONA_LTE();
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
char imei[16] = {0}; // Use this for device ID
uint8_t type;
float latitude, longitude, speed_kph, heading, altitude, second, address_lat, address_long, charging_lat, charging_long;
uint16_t year;
uint8_t month, day, hour, minute;
uint8_t counter = 0;
char URL[200];
char body[200];
char latBuff[12], longBuff[12], locBuff[50], speedBuff[12],
     headBuff[12], altBuff[12], tempBuff[12], battBuff[12];
bool availabl;

/************************* FLIGHT CONTROLLER DEFINITIONS/VARIABLES *********************************/
float OPERATIONAL_ALTITUDE;

// The exact byte content of these commands will depend on exact hardware solution, and they are only
// placeholders for now. These will be encoded into SBUS; a low-latency, computationally cheap and
// easy-to-implement serial data protocol supported by a wide array of generic flight controllers.
// The exact balancing and flight stability can be ensured in any flight controller card.

uint16_t* movePosXbyUnit, moveNegXbyUnit, movePosYbyUnit, moveNegYbyUnit,
          movePosZbyUnit, moveNegZbyUnit, addUnitYaw, subtractUnitYaw, stopAndFloat;

void setup() {
  Serial.begin(9600);
  
  pinMode(FONA_RST, OUTPUT);
  digitalWrite(FONA_RST, HIGH); // Default state

  fona.powerOn(FONA_PWRKEY); // Power on the module
  moduleSetup(); // Establishes first-time serial comm

  // Set modem to full functionality
  fona.setFunctionality(1); // AT+CFUN=1

  fona.setNetworkSettings(F("placeholder apn"));
  fona.setNetworkSettings(F("placeholder.com"));
  fona.setNetworkSettings(F("placeholder name"));

   // Perform first-time GPS/GPRS setup if the shield is going to remain on,
  // otherwise these won't be enabled in loop() and it won't work!
#ifndef turnOffShield
  // Enable GPS
  while (!fona.enableGPS(true)) {
    delay(1000); // Retry every second
  }

  #if !defined(SIMCOM_3G) && !defined(SIMCOM_7500) && !defined(SIMCOM_7600)
    // Disable GPRS just to make sure it was actually off so that we can turn it on
    fona.enableGPRS(false);
    
    // Turn on GPRS
    while (!fona.enableGPRS(true)) {
      delay(2000); // Retry every 2s
    }
  #endif
#endif
}

void loop() {
  // put your main code here, to run repeatedly:
  if(availabl){
    while(!getNextAddress()){
      delay(10000); // Every 10 seconds, ping server for a new address
      if(batteryLow()){
        goTo(charging_lat, charging_long, false);
      }
    }
    goTo(address_lat, address_long, true);
  }
}

bool batteryLow(){
  // Battery management to be implemented
}

void encodeSBUS(uint16_t* command){
  
  static uint8_t cmd[25];

  cmd[0] = 0x0F; //Header

  if (command) {
    cmd[1] = (uint8_t) ((command[0] & 0x07FF));
    cmd[2] = (uint8_t) ((command[0] & 0x07FF)>>8 | (command[1] & 0x07FF)<<3);
    cmd[3] = (uint8_t) ((command[1] & 0x07FF)>>5 | (command[2] & 0x07FF)<<6);
    cmd[4] = (uint8_t) ((command[2] & 0x07FF)>>2);
    cmd[5] = (uint8_t) ((command[2] & 0x07FF)>>10 | (command[3] & 0x07FF)<<1);
    cmd[6] = (uint8_t) ((command[3] & 0x07FF)>>7 | (command[4] & 0x07FF)<<4);
    cmd[7] = (uint8_t) ((command[4] & 0x07FF)>>4 | (command[5] & 0x07FF)<<7);
    cmd[8] = (uint8_t) ((command[5] & 0x07FF)>>1);
    cmd[9] = (uint8_t) ((command[5] & 0x07FF)>>9 | (command[6] & 0x07FF)<<2);
    cmd[10] = (uint8_t) ((command[6] & 0x07FF)>>6 | (command[7] & 0x07FF)<<5);
    cmd[11] = (uint8_t) ((command[7] & 0x07FF)>>3);
    cmd[12] = (uint8_t) ((command[8] & 0x07FF));
    cmd[13] = (uint8_t) ((command[8] & 0x07FF)>>8 | (command[9] & 0x07FF)<<3);
    cmd[14] = (uint8_t) ((command[9] & 0x07FF)>>5 | (command[10] & 0x07FF)<<6);
    cmd[15] = (uint8_t) ((command[10] & 0x07FF)>>2);
    cmd[16] = (uint8_t) ((command[10] & 0x07FF)>>10 | (command[11] & 0x07FF)<<1);
    cmd[17] = (uint8_t) ((command[11] & 0x07FF)>>7 | (command[12] & 0x07FF)<<4);
    cmd[18] = (uint8_t) ((command[12] & 0x07FF)>>4 | (command[13] & 0x07FF)<<7);
    cmd[19] = (uint8_t) ((command[13] & 0x07FF)>>1);
    cmd[20] = (uint8_t) ((command[13] & 0x07FF)>>9 | (command[14] & 0x07FF)<<2);
    cmd[21] = (uint8_t) ((command[14] & 0x07FF)>>6 | (command[15] & 0x07FF)<<5);
    cmd[22] = (uint8_t) ((command[15] & 0x07FF)>>3);
  }

  cmd[23] = 0x00; //flag
  cmd[24] = 0x00; //footer

  Serial.write(cmd, 25);
}

void sendLocationGET(){
  
  #if defined(SIMCOM_3G) || defined(SIMCOM_7500) || defined(SIMCOM_7600)
    sprintf(URL, "GET [DRONE LOCATION RESOURCE NAME AT SERVER] HTTP/1.1\r\nHost: [SERVER URL]\r\n\r\n",
            imei, latBuff, longBuff, speedBuff, headBuff, altBuff);
            
    // Try until post is successful, try every second
    while (!fona.postData("[SERVER URL]", 443, "HTTPS", URL)) { // Server, port, connection type, URL
      delay(1000);
    }
  #else
    sprintf(URL, "[SERVER URL]", imei, latBuff, longBuff,
            speedBuff, headBuff, altBuff, tempBuff, battBuff);
          
    while (!fona.postData("GET", URL)) {
      delay(1000);
    }
  #endif
  
}

bool getNextAddress(){
  #if defined(SIMCOM_3G) || defined(SIMCOM_7500) || defined(SIMCOM_7600)
    sprintf(URL, "GET [RESOURCE CONTAINING THIS DRONE'S NEXT ADRESS AT SERVER] HTTP/1.1\r\nHost: [SERVER URL]\r\n\r\n",
            imei, latBuff, longBuff);
            
    // Try until post is successful, try every second
    while (!fona.postData("[SERVER_URL]", 443, "HTTPS", URL)) { // Server, port, connection type, URL
      delay(1000);
    }
  #else
    sprintf(URL, "[SERVER URL]", imei, latBuff, longBuff);
          
    while (!fona.postData("GET", URL)) {
      delay(1000);
    }
  #endif

 // Function incomplete, exact format of client location data TBD by server development.
 // [Response parsing into latitude and longitude goes here]
 int replyLen;

 if (replyLen > 0){
  return true;
 }else{
  return false;
 }

 uint8_t ParsedLat, ParsedLong;

 address_lat = ParsedLat;
 address_long = ParsedLong;
}

void sendLocationPOST(){
  
  #if defined(SIMCOM_3G) || defined(SIMCOM_7500) || defined(SIMCOM_7600)
    sprintf(body, "{\"lat\":%s,\"long\":%s,\"speed\":%s,\"heading\":%s},\"altitude\":%s}\r\n", latBuff, longBuff, speedBuff, headBuff, altBuff); // Terminate with CR+NL
    sprintf(URL, "POST [DRONE LOCATION RESOURCE NAME AT SERVER] HTTP/1.1\r\nHost: [SERVER URL]", imei, strlen(body));

    while (!fona.postData("[SERVER URL]", 443, "HTTPS", URL, body)) { // Server, port, connection type, URL
      delay(1000);
    }
  #else
    sprintf(URL, "[SERVER URL]", imei);
    sprintf(body, "{\"lat\":%s,\"long\":%s,\"speed\":%s,\"heading\":%s},\"altitude\":%s}\r\n", latBuff, longBuff, speedBuff, headBuff, altBuff);

    while (!fona.postData("POST", URL, body)) {
      delay(1000);
    }
  #endif
}

void moduleSetup() {
  // Software serial:
  fonaSS.begin(115200); // Default SIM7000 shield baud rate
  fonaSS.println("AT+IPR=9600"); // Set baud rate
  delay(100); // Short pause to let the command run
  fonaSS.begin(9600);
  if (! fona.begin(fonaSS)) {
    while (1); // Don't proceed if it couldn't find the device
  }
}

void sendLocation(){
  // Get a fix on location, try every second
  while (!fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude, &year, &month, &day, &hour, &minute, &second)) {
    delay(1000); // Retry every 2s
  }

  // Format the floating point numbers
  dtostrf(latitude, 1, 6, latBuff);
  dtostrf(longitude, 1, 6, longBuff);
  dtostrf(speed_kph, 1, 0, speedBuff);
  dtostrf(heading, 1, 0, headBuff);
  dtostrf(altitude, 1, 1, altBuff);

  // Also construct a combined, comma-separated location array
  // (many platforms require this for dashboards, like Adafruit IO):
  sprintf(locBuff, "%s,%s,%s,%s", speedBuff, latBuff, longBuff, altBuff); // This could look like "10,33.123456,-85.123456,120.5"
 
  sendLocationPOST();
}

void ascendToOperationalAltitude(){
  elapsedMillis sinceAscentBegan;
  
  while (!fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude, &year, &month, &day, &hour, &minute, &second)) {
    delay(1000); // Retry every second
    }
    
    while(altitude < OPERATIONAL_ALTITUDE){
      
      fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude, &year, &month, &day, &hour, &minute, &second);
      encodeSBUS(movePosYbyUnit);
      if(sinceAscentBegan > 7000){ // Limits ascent to 7s in case GPS signal is not found
        break;
    }
  }
}

bool goTo(uint8_t lat, uint8_t lon, bool dropoff){
  availabl = false;
  ascendToOperationalAltitude();
  elapsedMillis sinceGPSLost;
  
  while (!fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude, &year, &month, &day, &hour, &minute, &second)) {
    delay(1000); // Retry every second
    if(sinceGPSLost > 30000){availabl = true; return false;} // Looks for GPS for 30s, and then returns failure
    }

  elapsedMillis sinceGPSLostAgain;
  
  float required_heading = tan(lat/lon);
  
  while(tan(latitude/longitude) != required_heading){
    encodeSBUS(addUnitYaw);
    
    while (!fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude, &year, &month, &day, &hour, &minute, &second)) {
      delay(1000); // Retry every second
      if(sinceGPSLostAgain > 15000){availabl = true; return false;} // Spins around for 15s, and then returns failure
    }
  }
  while((lat != latitude) || (lon != longitude)){
    encodeSBUS(movePosZbyUnit);
    if (!fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude, &year, &month, &day, &hour, &minute, &second)){
      encodeSBUS(stopAndFloat); // GPS lost
      availabl = true;
      return false;
    }
  }
  if(dropoff){
    dropoffRoutine();
  }
}

void dropoffRoutine(){
  //To be implemented
  availabl = true;
}
