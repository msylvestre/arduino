/* 
*  Test for the DHT11 sensor
*  Part of the code is based on the work done by Adafruit for the DHT11 sensor
*  Writtent by Marco Schwartz for Open Home Automation
*/

// Include required libraries
#include <SPI.h>
#include <string.h>
#include "DHT.h"

// DHT11 sensor pins
#define DHTPIN 7 
#define DHTTYPE DHT22

// DHT instance
DHT dht(DHTPIN, DHTTYPE);
                                         
void setup(void)
{
 
  // Initialize DHT sensor
  dht.begin();
  
  Serial.begin(115200);
  
}
  
void loop(void)
{
  
    // Measure the humidity & temperature
    float h = dht.readHumidity();
    float t = dht.readTemperature();
   
    // Transform to String
    String temp = String((float) t);
    String hum = String((float) h);
    
    Serial.print("Temperature: ");
    Serial.println(temp);
    Serial.print("Humidity: ");
    Serial.println(hum);
    Serial.println("");
    delay(5000);
  
}
