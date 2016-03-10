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
const int echoPin   = 11;
const int trigPin   = 12;
const int analogPin = A0;   // the pin that the potentiometer is attached to
const int ledCount  = 7;    // the number of LEDs in the bar graph

// an array of pin numbers to which LEDs are attached
int ledPins[] = {2, 3, 4, 5, 6, 7, 8};   

void setup()
{
    // set up serial
    Serial.begin(9600);
    
    // set the pinmode on our ultrasonic echo, and tric pins
    pinMode(echoPin, INPUT);
    pinMode(trigPin, OUTPUT);

    // loop over the pin array and set them all to output:
    for (int thisLed = 0; thisLed < ledCount; thisLed++) 
    {
      pinMode(ledPins[thisLed], OUTPUT);
      delay(10);
      digitalWrite(ledPins[thisLed], HIGH);
      delay(100);
      digitalWrite(ledPins[thisLed], LOW);
    }

}

void loop()
{
    float distanceCentimeters;
    int pulseLenMicroseconds;

    // bit-bang a small square wave
    // on the trig pin to start the range
    // finder
    digitalWrite(trigPin, LOW);
    delayMicroseconds(20);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(100);
    digitalWrite(trigPin, LOW);

    // measure the pulse length from the echo pin
    pulseLenMicroseconds = pulseIn(echoPin, HIGH);

    // calculate the distance using the speed of sound
    distanceCentimeters = pulseLenMicroseconds / 29.387 / 2;

    // print it out over serial
    Serial.println(distanceCentimeters);

    if ((distanceCentimeters > 2) && (distanceCentimeters < 100))
    {
      int ledLevel = map(distanceCentimeters, 60, 10, 0, ledCount);
    
      // loop over the LED array:
      for (int thisLed = 0; thisLed < ledCount; thisLed++) 
      {
        // if the array element's index is less than ledLevel,
        // turn the pin for this element on:
        if (thisLed < ledLevel) {
          digitalWrite(ledPins[thisLed], HIGH);
        }
        // turn off all pins higher than the ledLevel:
        else {
          digitalWrite(ledPins[thisLed], LOW);
        }
      }
    }
    else if (distanceCentimeters > 200)
    {
      for (int thisLed = 0; thisLed < ledCount; thisLed++) 
      {
        digitalWrite(ledPins[thisLed], LOW);
      }
    }
    

    delay(100);
}
