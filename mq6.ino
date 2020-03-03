void setup()
{
  Serial.begin(9600);
}
 
void loop()
{
  float Voltage6;
  float Value6;
 
  Value6 = analogRead(35);

     
  Voltage6 = Value6/4095*3.3;
 
  Serial.print("sensor voltage = ");
  Serial.print(Voltage6);
  Serial.println(" V");
  delay(1000);
}
