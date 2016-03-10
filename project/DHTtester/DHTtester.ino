// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!

// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

#include "DHT.h"
//#include <LiquidCrystal.h>

#define DHTPIN 7     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int ledState = LOW;

  
void setup() 
{

//  pinMode(13, OUTPUT);
  
  Serial.begin(9600);

  // Start to grab T* & H%
  dht.begin();

  // set up the LCD's number of columns and rows:
  //lcd.begin(16, 2);

  // Wait for init.
  delay(500);

}

void loop() 
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float humidity = dht.readHumidity();

  // Read temperature as Celsius (the default)
  float temp = dht.readTemperature();
  
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temp)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    //delay(1000);
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(temp, humidity, false);
/*
  lcd.setCursor(0, 0);  //col / Row
  
  lcd.print("Temp    : ");
  lcd.print(temp);
  lcd.print("c");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%");
*/
  blinkLight();
    
  Serial.print(humidity);
  Serial.print(";");
  Serial.print(temp);
  Serial.print("\n");

  // Wait a few seconds between measurements.
  delay(30000);

}

void blinkLight()
{
  for (int i = 0; i < 8; i++)
  {
    if (ledState == LOW)
    { 
      digitalWrite(13, HIGH);
      ledState = HIGH;
      delay(300);
    }
    else
    {
      digitalWrite(13, LOW);
      ledState = LOW;
      delay(300);

    }
  }
}

