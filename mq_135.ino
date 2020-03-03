#include<DHT.h>
float RS_gas1 = 0;
float ratio1 = 0;
float sensorValue135 = 0;
float sensor_volt1 = 0;
float RO = 7200.0;
 
void setup() {
 Serial.begin(9600);
}
 
void loop() {
   sensorValue135 = analogRead(32);
   float b= analogRead(13);
   float c= analogRead(4);
   float d= analogRead(27);
   sensor_volt1 = sensorValue135/4095*3.3;
   RS_gas1 = (3.3-sensor_volt1)/sensor_volt1;
   ratio1 = RS_gas1/RO; //Replace R0 with the value found using the sketch above
   float x1 = 1538.46 * ratio1;
   float ppm1 = pow(x1,-1.709);
   ppm1=ppm1*100;
   Serial.print("PPM: ");
   Serial.println(ppm1);
   delay(1000);
}
