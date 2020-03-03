/*****
 * DHT22 ON PIN 4
 * MQ6   ON PIN 32
 * MQ7   ON PIN 33
 * MQ135 ON PIN 35
 */

//mq7
float RS_gas = 0;
float ratio = 0;
float Value1 = 0;
float volt = 0;
float R0 = 7200.0;
//mq135
float RS_gas1 = 0;
float ratio1 = 0;
float Value135 = 0;
float volt1 = 0;
float RO = 7200.0;

// mq6
  float Voltage6;
  float Value6;
 


#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>  
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "sps30.h"


/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Asus"
#define WLAN_PASS       "password"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "parth1299"
#define AIO_KEY         "46b6e2eca8364e17a364a1b29ac4bd08"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish sen1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");
Adafruit_MQTT_Publish sen2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/hum");
Adafruit_MQTT_Publish sen3 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/mq6");
Adafruit_MQTT_Publish sen4 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/mq7");
Adafruit_MQTT_Publish sen5 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/mq135");
Adafruit_MQTT_Publish sen6 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/PM1");
Adafruit_MQTT_Publish sen7 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/PM2.5");
Adafruit_MQTT_Publish sen8 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/PM10");

// Setup a feed called 'onoff' for subscribing to changes.
//Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

#define DHTPIN 4   // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

// Senseron
#define SP30_COMMS SERIALPORT2
#define TX_PIN 16
#define RX_PIN 17
#define DEBUG 0



///////////////////////////////////////
AsyncWebServer server(80);
DNSServer dns;

/////////////////////////////////////////
void serialTrigger(char * mess);
void ErrtoMess(char *mess, uint8_t r);
void Errorloop(char *mess, uint8_t r);
void GetDeviceInfo();
bool read_all();

SPS30 sps30;


void setup() {
  Serial.begin(9600);
  delay(10);

 //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    AsyncWiFiManager wifiManager(&server,&dns);
    //reset saved settings
    //wifiManager.resetSettings();
    //set custom ip for portal
    //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("Air Purifier");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

    
  // Setup MQTT subscription for onoff feed.
  //mqtt.subscribe(&onoffbutton);
///////////////////////////////////////////////////////////////////////////////////////////////
//dht////
   dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
 /* Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.*/
  dht.humidity().getSensor(&sensor);
 /* Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.*/
  delayMS = sensor.min_delay / 1000;

/// SENSIRION///////
////////////////////////////////////////////////////////////////////////////////////////////////////////
// serialTrigger("SPS30-Example1: Basic reading. press <enter> to start");

  Serial.println(F("Trying to connect to sensorr"));

  // set driver debug level
  sps30.EnableDebugging(DEBUG);

  // set pins to use for softserial and Serial1 on ESP32
  if (TX_PIN != 0 && RX_PIN != 0) sps30.SetSerialPin(RX_PIN,TX_PIN);

  // Begin communication channel;
  if (sps30.begin(SP30_COMMS) == false) {
    Errorloop("could not initialize communication channel.", 0);
  }

  // check for SPS30 connection
  if (sps30.probe() == false) {
    Errorloop("could not probe / connect with SPS30.", 0);
  }
  else
    Serial.println(F("Detected SPS30."));

  // reset SPS30 connection
  if (sps30.reset() == false) {
    Errorloop("could not reset.", 0);
  }

  // read device info
  GetDeviceInfo();

  // start measurement
  if (sps30.start() == true)
    Serial.println(F("Measurement started"));
  else
    Errorloop("Could NOT start measurement", 0);

//  serialTrigger("Hit <enter> to continue reading");

  if (SP30_COMMS == I2C_COMMS) {
    if (sps30.I2C_expect() == 4)
      Serial.println(F(" !!! Due to I2C buffersize only the SPS30 MASS concentration is available !!! \n"));
  }
////////////////////////////////////////////////////////////////////////////
}/// setup closed

uint32_t x=0;
float temp,hum;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

 /* Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
      int k=atoi((char *)onoffbutton.lastread);
      if(k==1)
      digitalWrite(2,HIGH);
      else
      digitalWrite(2,LOW);
    }
  }
*/
  // Now we can publish stuff!
////////////////////////////////////////////////////////////////////////

///////////////////////////DHT/////////////////////////////////////////
delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  
  temp=event.temperature;
  
  if (isnan(temp)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(temp);
    Serial.println(F("°C"));
    
  Serial.print("v1.val=");              //We print the variable we want to cahnge on the screen
  Serial.print((int)temp);                        //Print the value we want to be displayed
  Serial.write(0xff);                         //Always add 3 full bytes after...       
  Serial.write(0xff);
  Serial.write(0xff);
  
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  hum=event.relative_humidity;
  if (isnan(hum)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(hum);
    Serial.println(F("%"));

  Serial.print("v2.val=");              //We print the variable we want to cahnge on the screen
  Serial.print((int)hum);                        //Print the value we want to be displayed
  Serial.write(0xff);                         //Always add 3 full bytes after...       
  Serial.write(0xff);
  Serial.write(0xff);
    
  }
////////////////////////////////////////////////////////////

///////////////////////MQ7/////////////////////////////////
 Value1 = analogRead(33);
   Serial.print("Mq7=");
   Serial.println(Value1);
   volt = Value1/4095*3.3;
   RS_gas = (3.3-volt)/volt;
   ratio = RS_gas/R0; //Replace R0 with the value found using the sketch above
   float x = 1538.46 * ratio;
   float ppm = pow(x,-1.709);
   Serial.print("PPM: ");
   Serial.println(ppm);
   delay(1000);

  Serial.print("v3.val=");              //We print the variable we want to cahnge on the screen
  Serial.print((int)ppm);                        //Print the value we want to be displayed
  Serial.write(0xff);                         //Always add 3 full bytes after...       
  Serial.write(0xff);
  Serial.write(0xff);
/////////////////////////////////////////////////////////////

////////////////////MQ135///////////////////////////////////
   Value135 = analogRead(35);
     Serial.print("Mq135=");
   Serial.println(Value135);
   volt1 = Value135/4095*3.3;
   RS_gas1 = (3.3-volt1)/volt1;
   ratio1 = RS_gas1/RO; //Replace R0 with the value found using the sketch above
   float x1 = 1538.46 * ratio1;
   float ppm1 = pow(x1,-1.709);
   ppm1=ppm1*100;
   Serial.print("PPM: ");
   Serial.println(ppm1);
   delay(1000);

  Serial.print("v4.val=");              //We print the variable we want to cahnge on the screen
  Serial.print((int)ppm1);                        //Print the value we want to be displayed
  Serial.write(0xff);                         //Always add 3 full bytes after...       
  Serial.write(0xff);
  Serial.write(0xff);
//////////////////////////////////////////////////////////////
 
/////////////////////MQ6/////////////////////////////////////
   
  Value6 = analogRead(32);
  Voltage6 = Value6/4095*3.3;
  Serial.print("sensor voltage = ");
  Serial.print(Voltage6);
  Serial.println(" V");
  delay(1000);
//////////////////////////////////////////////////////////////
  
/////////////////////////DHT  SENDING/////////////////////////////////    
  Serial.print(F("\nSending dht val "));
  Serial.print(x);
  Serial.print("...");
  if (! sen1.publish(temp++)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  if (! sen2.publish(hum++)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

 delay(4000);

 //////////////////////////////////////////////////////////////
  
/////////////////////////MQ6  SENDING/////////////////////////////////    

    Serial.print(F("\nSending mq6 val "));

if(Voltage6>1)
if (! sen3.publish("Flammable Gas Detected")) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  else
  if (! sen3.publish("No Flammable Gas")) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

 delay(2000);
//////////////////////////////////////////////////////////////
  
/////////////////////////MQ7  SENDING/////////////////////////////////    

  Serial.print(F("\nSending mq7 val "));

  if (! sen4.publish(ppm++)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

delay(2000);
//////////////////////////////////////////////////////////////
  
/////////////////////////MQ7  SENDING/////////////////////////////////    

  Serial.print(F("\nSending mq135 val "));

 if (! sen5.publish(ppm1++)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

   delay(2000);
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
//////////////////////////////////////////////////////////////
  
/////////////////////////SENSIRION/////////////////////////////////    

  read_all();
  delay(3000);
}///LOOP ENDING

void GetDeviceInfo()
{
  char buf[32];
  uint8_t ret;

  //try to read serial number
  ret = sps30.GetSerialNumber(buf, 32);
  if (ret == ERR_OK) {
    Serial.print(F("Serial number : "));
    if(strlen(buf) > 0)  Serial.println(buf);
    else Serial.println(F("not available"));
  }
  else
    ErrtoMess("could not get serial number", ret);

  // try to get product name
  ret = sps30.GetProductName(buf, 32);
  if (ret == ERR_OK)  {
    Serial.print(F("Product name  : "));

    if(strlen(buf) > 0)  Serial.println(buf);
    else Serial.println(F("not available"));
  }
  else
    ErrtoMess("could not get product name.", ret);

  // try to get article code
  ret = sps30.GetArticleCode(buf, 32);
  if (ret == ERR_OK)  {
    Serial.print(F("Article code  : "));

    if(strlen(buf) > 0)  Serial.println(buf);
    else Serial.println(F("not available"));
  }
  else
    ErrtoMess("could not get Article code .", ret);
}

/**
 * @brief : read and display all values
 */
bool read_all()
{
  static bool header = true;
  uint8_t ret, error_cnt = 0;
  struct sps_values val;

  // loop to get data
  do {

    ret = sps30.GetValues(&val);

    // data might not have been ready
    if (ret == ERR_DATALENGTH){

        if (error_cnt++ > 3) {
          ErrtoMess("Error during reading values: ",ret);
          return(false);
        }
        delay(1000);
    }

    // if other error
    else if(ret != ERR_OK) {
      ErrtoMess("Error during reading values: ",ret);
      return(false);
    }

  } while (ret != ERR_OK);

  // only print header first time
  /*if (header) {
    Serial.println(F("-------------Mass -----------    ------------- Number --------------   -Average-"));
    Serial.println(F("     Concentration [μg/m3]             Concentration [#/cm3]             [μm]"));
    Serial.println(F("P1.0\tP2.5\tP4.0\tP10\tP0.5\tP1.0\tP2.5\tP4.0\tP10\tPartSize\n"));
    header = false;
  }*/

  Serial.print(val.MassPM1);
  Serial.print(F("\t"));

  Serial.print("n0.val=");              //We print the variable we want to cahnge on the screen
  Serial.print((int)val.MassPM1++);                        //Print the value we want to be displayed
  Serial.write(0xff);                         //Always add 3 full bytes after...       
  Serial.write(0xff);
  Serial.write(0xff);
  
  Serial.print(val.MassPM2);
  Serial.print(F("\t"));

  Serial.print("n1.val=");              //We print the variable we want to cahnge on the screen
  Serial.print((int)val.MassPM2++);                        //Print the value we want to be displayed
  Serial.write(0xff);                         //Always add 3 full bytes after...       
  Serial.write(0xff);
  Serial.write(0xff);
  
  //Serial.print(val.MassPM4);
  //Serial.print(F("\t"));
  Serial.print(val.MassPM10++);
  Serial.print(F("\t"));

  Serial.print("n2.val=");              //We print the variable we want to cahnge on the screen
  Serial.print((int)val.MassPM10);                        //Print the value we want to be displayed
  Serial.write(0xff);                         //Always add 3 full bytes after...       
  Serial.write(0xff);
  Serial.write(0xff);
  
 // Serial.print(val.NumPM0);
 // Serial.print(F("\t"));
 // Serial.print(val.NumPM1);
 // Serial.print(F("\t"));
 // Serial.print(val.NumPM2);
 // Serial.print(F("\t"));
 // Serial.print(val.NumPM4);
 // Serial.print(F("\t"));
 // Serial.print(val.NumPM10);
 // Serial.print(F("\t"));
 // Serial.print(val.PartSize);
 // Serial.print(F("\n"));
//////////////////////////////////////////////////////////////
  
/////////////////////////PM1  SENDING/////////////////////////////////    

 Serial.print(F("\nSending PM 1 val "));

  if (! sen6.publish(val.MassPM1++)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
//////////////////////////////////////////////////////////////
  
/////////////////////////PM2.5 SENDING/////////////////////////////////    

Serial.print(F("\nSending PM 2.5 val "));

  if (! sen7.publish(val.MassPM2++)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
//////////////////////////////////////////////////////////////
  
/////////////////////////PM10  SENDING/////////////////////////////////    

Serial.print(F("\nSending PM 10 val "));

  if (! sen8.publish(val.MassPM10++)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }



  
}

/**
 *  @brief : continued loop after fatal error
 *  @param mess : message to display
 *  @param r : error code
 *
 *  if r is zero, it will only display the message
 */
void Errorloop(char *mess, uint8_t r)
{
  if (r) ErrtoMess(mess, r);
  else Serial.println(mess);
  Serial.println(F("Program on hold"));
  for(;;) delay(100000);
}

/**
 *  @brief : display error message
 *  @param mess : message to display
 *  @param r : error code
 *
 */
void ErrtoMess(char *mess, uint8_t r)
{
  char buf[80];

  Serial.print(mess);

  sps30.GetErrDescription(r, buf, 80);
  Serial.println(buf);
}

/**
 * serialTrigger prints repeated message, then waits for enter
 * to come in from the serial port.
 */
void serialTrigger(char * mess)
{
  Serial.println();

  while (!Serial.available()) {
    Serial.println(mess);
    delay(2000);
  }

  while (Serial.available())
    Serial.read();
}

///////////////////////////////////////////////////////////////////////////////////////

  


// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
