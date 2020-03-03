#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>  
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define DHTPIN 4     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value



float RS_gas = 0;
float ratio = 0;
float Value1 = 0;
float volt = 0;
float R0 = 7200.0;

float RS_gas1 = 0;
float ratio1 = 0;
float Value135 = 0;
float volt1 = 0;
float RO = 7200.0;
 
void setup() {
 Serial.begin(9600);
 dht.begin();
}
 
void loop() {
   Value1 = analogRead(27);
   Serial.print("mq7");
   Serial.print(Value1);
   volt = Value1/4095*3.3;
   RS_gas = (3.3-volt)/volt;
   ratio = RS_gas/R0; //Replace R0 with the value found using the sketch above
   float x = 1538.46 * ratio;
   float ppm = pow(x,-1.709);
   Serial.print("PPM: ");
   Serial.println(ppm);
   delay(1000);

   Value135 = analogRead(25);
      Serial.print("mq135");
   Serial.print(Value135);
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
 
  Value6 = analogRead(13);

     
  Voltage6 = Value6/4095*3.3;
 
  Serial.print("sensor voltage = ");
  Serial.print(Voltage6);
  Serial.println(" V");
  delay(1000);

   //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp= dht.readTemperature();
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    delay(2000); //Delay 2 sec.
}
