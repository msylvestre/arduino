/*
 * Tutorial 5: Ultrasonic Range Finder
 * 
 * Prints the distance measured from the
 * range finder over serial. Place the board on your desk
 * and open up the serial monitor. Move an object in front of the
 * ultrasonic sensor and you'll see the distance to the object
 * printed out over serial.
 *
 * The circuit:
 * - 5v, ground connected to Ultrasonic sensor
 * - digital pin 4 conneceted to Ultrasonic sensor echo pin
 * - digital pin 5 conneceted to Ultrasonic sensor trig pin
 *
 * created 3 Nov 2008
 * by David A. Mellis
 * modified 30 Aug 2011
 * by Tom Igoe
 * modified 14 August 2013
 * by Blaise Jarrett
 *
 * This example code is in the public domain.
 *
 * Derivative work from:
 * http://www.arduino.cc/en/Tutorial/Ping
 *
 */

// the pins connected to the Ultrasonic sensor
const int ledCount  = 7;    // the number of LEDs in the bar graph
const int potPin = A0;

// an array of pin numbers to which LEDs are attached
int ledPins[]   = {2, 3, 4, 5, 6, 7, 8};   
int ledLevel    = 0;
int switchState = 0;
int potValue    = 0;
int myDelay     = 0;
int minDelay    = 0;
int maxDelay    = 5;

void setup()
{
  
    Serial.begin(9600);

    // loop over the pin array and set them all to output:
    for (int thisLed = 0; thisLed < ledCount; thisLed++) 
    {
      pinMode(ledPins[thisLed], OUTPUT);
      //delay(10);
      //digitalWrite(ledPins[thisLed], HIGH);
      delay(100);
      digitalWrite(ledPins[thisLed], LOW);
    }

}

void loop()
{

    potValue = analogRead(potPin);
    myDelay = map(potValue,0,1023,minDelay,maxDelay);
    Serial.print("myDelay -> ");
    Serial.println(myDelay);
    
    // loop over the LED array:
    for (int thisLed = 0; thisLed < ledCount; thisLed++) 
    {
      //digitalWrite(ledPins[thisLed], HIGH);
      //delay(150);

      for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) 
      {
        potValue = analogRead(potPin);
        myDelay = map(potValue,0,1023,minDelay,maxDelay);

        // sets the value (range from 0 to 255):
        analogWrite(ledPins[thisLed], fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(myDelay);
      }
    }
      
    // loop over the LED array:
    for (int thisLed = 0; thisLed <= ledCount; thisLed++) 
    {
      for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) 
      {
        potValue = analogRead(potPin);
        myDelay = map(potValue,0,1023,minDelay,maxDelay);

        
        // sets the value (range from 0 to 255):
        analogWrite(ledPins[thisLed], fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(myDelay);
      }
    }   

    delay(200);

    // loop over the LED array:
    for (int thisLed = ledCount; thisLed >= 0; thisLed--) 
    {
      //digitalWrite(ledPins[thisLed], HIGH);
      //delay(150);

      for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) 
      {
        potValue = analogRead(potPin);
        myDelay = map(potValue,0,1023,minDelay,maxDelay);

        // sets the value (range from 0 to 255):
        analogWrite(ledPins[thisLed], fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(myDelay);
      }
    }
      
    // loop over the LED array:
    for (int thisLed = ledCount; thisLed >= 0; thisLed--) 
    {
      for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) 
      {
        potValue = analogRead(potPin);
        myDelay = map(potValue,0,1023,minDelay,maxDelay);

        
        // sets the value (range from 0 to 255):
        analogWrite(ledPins[thisLed], fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(myDelay);
      }
    }   

    delay(500);
}

