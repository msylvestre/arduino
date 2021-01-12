// Simple example application that shows how to read four Arduino
// digital pins and map them to the USB Joystick library.
//
// Ground digital pins 9, 10, 11, and 12 to press the joystick 
// buttons 0, 1, 2, and 3.
//
// NOTE: This sketch file is for use with Arduino Leonardo and
//       Arduino Micro only.
//
// by Matthew Heironimus
// 2015-11-20
//--------------------------------------------------------------------

#include <Joystick.h>

Joystick_ Joystick;

// Constant that maps the phyical pin to the joystick button.
const int pinToButtonMap = 9;

// Pin used
int ledPin = 2;
int buttonPin = 9;

// Last state of the button
int lastButtonState[4] = {0,0,0,0};


void setup() {
  
  // Initialize Button Pins
  //pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  //pinMode(10, INPUT_PULLUP);
  //pinMode(11, INPUT_PULLUP);
  //pinMode(12, INPUT_PULLUP);

  // Initialize Joystick Library
  Joystick.begin();
}

void loop() {

  int stateButton = digitalRead(buttonPin);

  if(stateButton == 0) 
  { 
    //if is pressed
    digitalWrite(ledPin, HIGH); //write 1 or HIGH to led pin
  } 
  else 
  { 
    //if not pressed
    digitalWrite(ledPin, LOW);  //write 0 or low to led pin
  }


  // Read pin values
  for (int index = 0; index < 4; index++)
  {
    int currentButtonState = !digitalRead(index + pinToButtonMap);
    
    if (currentButtonState != lastButtonState[index])
    {
      Joystick.setButton(index, currentButtonState);
      lastButtonState[index] = currentButtonState;
    }
  }

  delay(50);
}
