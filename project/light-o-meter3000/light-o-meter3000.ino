/*
 *
 * # To get access to the port : sudo chmod a+rw /dev/ttyUSB0
 *
 * LCD
 * =================================================
 * The circuit:
 * - LCD RS pin to digital pin 12
 * - LCD Enable pin to digital pin 11
 * - LCD D4 pin to digital pin 5
 * - LCD D5 pin to digital pin 4
 * - LCD D6 pin to digital pin 3
 * - LCD D7 pin to digital pin 2
 * - LCD R/W pin to ground
 * - 10K potentiometer divider for LCD pin VO:
 * - 330ohm resistor betweenm LCD pin A and 5v
 * - LCD pin K to ground
 *
 *
 */

// include the library
#include <LiquidCrystal.h>

// Debug output
const String DEBUG_OUTPUT = "LCD"  // LCD or SERIAL

// Smoothing
const int numReadings = 5;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total     = 0;              // the running total
int average   = 0;              // the average

// Potentiometer that control the sensivity of the photocell
const int POT_PIN = A0;
const int POT_MIN = 0;
const int POT_MAX = 99;
int potValue      = 0;

// Temperature sensor
const int TEMP_PIN = A1;
float temp         = 0;

// Photocell sensor
const int PHOTOCELL_PIN = A2;
const int PHOTOCELL_MIN = 0;    // sensor minimum, discovered through experiment
const int PHOTOCELL_MAX = 800;  // sensor maximum, discovered through experiment
int photocellValue      = 0;

// Light (Led for development)
const int LIGHT_PIN    = 12;
const bool ON          = true;
const bool OFF         = false;
bool manualLightSwitch = true;

// LCD 
const int lcdRSPin = 7;
const int lcdEPin  = 6;
const int lcdD4Pin = 5;
const int lcdD5Pin = 4;
const int lcdD6Pin = 3;
const int lcdD7Pin = 2;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(lcdRSPin, lcdEPin,
                  lcdD4Pin, lcdD5Pin, lcdD6Pin, lcdD7Pin);


/*====================================================================================*/

void setup()
{
    Serial.begin(9600);
    
    // set up the LCD's number of columns and rows: 
    lcd.begin(16, 2);

    // set the led pin in output mode to open/close the led
    pinMode(LIGHT_PIN, OUTPUT);

    // Show via the LED that the program is starting
    for (int i = 0; i < 2; i++)
    {
        switchLight(LIGHT_PIN, ON);
        delay(300);
        switchLight(LIGHT_PIN, OFF);
        delay(100);
    }   

    // initialize all the readings to 0:
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      readings[thisReading] = 0;
    } 
}


void loop()
{
    // Read Serial Port for dev - later will comme from WiFi
    //manualLightSwitch = readManualLightSwitch();
    Serial.print("manualSwitch: ");
    Serial.println(manualLightSwitch);
     
    // Read Sensor
    potValue       = readPotentiometer();
    photocellValue = readPhotocell();
    //temp           = readTemp();

    manageLight(photocellValue, potValue, manualLightSwitch);    
    printTo(DEBUG_OUTPUT, photocellValue, potValue, temp);

    delay(500);
}




/*==================================================================================*/
/* ------------------------------- INTERNAL FUNCTION ------------------------------ */
/*==================================================================================*/

bool manageLight(int photocellValue, int potValue, bool manualLightSwitch)
{

    if (manualLightSwitch)
    {
      if (photocellValue < potValue)
          switchLight(LIGHT_PIN, ON);
      else
          switchLight(LIGHT_PIN, OFF);
    }
    else
    {
      if (photocellValue > potValue)
          return true;
      else
          return false;      
    }
}


bool readManualLightSwitch()
{
    int value;
    
    if (Serial.available() > 0)
    {
      value = Serial.read();
      
      Serial.print("Value: ");
      Serial.println(value);
      
      if (value == '0')
          return false;
      else
          return true;
    }    
}

void switchLight(int lightPin, bool lightStatus)
{
    if (lightStatus)
        digitalWrite(lightPin, HIGH);
    else
        digitalWrite(lightPin, LOW);
}


float readTemp()
{
    // read our temperature sensor
    int analogValue = analogRead(TEMP_PIN);

    // convert the 10bit analog value to celcius
    float temperature = (float(analogValue) / 1023) * 500;
    
    return temperature;      
}


int readPhotocell()
{
    // subtract the last reading:
    total = total - readings[readIndex];

    // read from the sensor:
    readings[readIndex] = analogRead(PHOTOCELL_PIN);
    
    // add the reading to the total:
    total = total + readings[readIndex];
    
    // advance to the next position in the array:
    readIndex = readIndex + 1;
  
    // if we're at the end of the array...
    if (readIndex >= numReadings) 
      readIndex = 0;
  
    average = total / numReadings;
    
    // send it to the computer as ASCII digits
    //Serial.println(average);

    return map(average, 0, 1023, POT_MIN, POT_MAX);  
}


int readPotentiometer()
{
    int analogValue = analogRead(POT_PIN);
    return map(analogValue, 0, 1023, POT_MAX, POT_MIN);
}


void printTo(String output, int photocell, int potentiometer, float temp)
{

    if (output == "LCD")
    {
        
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0)
        lcd.setCursor(0, 0);
    
        lcd.print("Photocell:");
        lcd.print(photocell);
    
        lcd.setCursor(0, 1);
        lcd.print("Potentiometer:");
    
        if (potentiometer < 10)
        {
             lcd.setCursor(14, 1);
             lcd.print(0);    
             lcd.setCursor(15, 1);
        }
        
        lcd.print(potentiometer);
    
        /*
        lcd.setCursor(0,1);
        lcd.print("Temp: ");
        lcd.print(temp);
        lcd.print("c");   
        */
    }
    else if (output == "SERIAL")
    {
        Serial.print("Photocell: ");
        Serial.println(photocell);
        Serial.print("Potentiometer: ");
        Serial.println(potentiometer);        
    }
}


