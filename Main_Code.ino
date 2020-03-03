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



AsyncWebServer server(80);
DNSServer dns;


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

}

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
//dht
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
  }
  
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
    float Voltage6;
  float Value6;
 
  Value6 = analogRead(32);

     
  Voltage6 = Value6/4095*3.3;
 
  Serial.print("sensor voltage = ");
  Serial.print(Voltage6);
  Serial.println(" V");
  delay(1000);


  
    
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


  if (! sen4.publish(ppm++)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

delay(2000);


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
}

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
