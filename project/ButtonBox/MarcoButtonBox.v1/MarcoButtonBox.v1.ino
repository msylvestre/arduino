// Marco's Button Box
// Written : around Jan 16th 2021
//--------------------------------------------------------------------

#include <Joystick.h>
#include <elapsedMillis.h>
#include <Rotary.h>

//======================================================================
// VARIABLES DECLARATION

// Libraries
Joystick_ Joystick;                // Used to press the cutton on the joystick 
elapsedMillis timeElapsedIgn;      // Used to calucale easily the time elapse between 2 event
elapsedMillis timeElapsedBrkBias;  // Used to calucale easily the time elapse between 2 event

// Pins setting
const int START_ENGINE_PIN = 0; // Windows Button 1, Joystick 0
const int IGNITION_PIN = 12;     // Windows Button 2, Joystick 1
const int LED1_PIN = 13;         // Ignition Led
const int BRAKE_BIAS_BACKWARD_PIN = 15;
const int BRAKE_BIAS_FORWARD_PIN = 14;

// List of Windows joystick button used.
const int IGNITION_BUTTON = 0;
const int START_ENGINE_BUTTON = 1;
const int BRAKE_BIAS_FORWARD_BUTTON = 3;
const int BRAKE_BIAS_BACKWARD_BUTTON = 2;

//Rotary encoders
Rotary rotary = Rotary(BRAKE_BIAS_BACKWARD_PIN, BRAKE_BIAS_FORWARD_PIN);


// AMount of time we keep the button pushed
const int INTERVAL = 300; //milliseconds

// Use to trigger the release of the button after the TRIGGER_INTERVAL
bool checkToReleaseIgnButtonFlag = false;
bool checkToReleaseBrkBiasButtonFlag = false;

// Last state of the button
int lastStartButtonState = 0;
int lastIgnitionSwitchState = 0;

int counter = 0;
String rotarySide = "";

//======================================================================
// SETUP & MAIN LOOP

void setup() 
{
  // Initialize Pins
  pinMode(START_ENGINE_PIN, INPUT_PULLUP); 
  pinMode(IGNITION_PIN, INPUT_PULLUP);
  pinMode(LED1_PIN, OUTPUT);
   
  // Initialize Joystick Library
  Joystick.begin();

  // DEBUG
  Serial.begin(57600);  // Enable the serial pour to print to the console. CTRL+SHIFT+M to open the serial monitor
}
  

void loop() 
{
  // Act when the Ignition is turned on/off
  UpdateIgnitionSwitchState();

  // Act when the Start button is pressed
  UpdateStartButtonState();

  UpdateBrakeBiasState();
}


//======================================================================
// FUNCTIONS


// IGNITION SWITCH

void UpdateIgnitionSwitchState()
{
  // Read the state of the Ignition 
  int currentIgnitionSwitchState = !digitalRead(IGNITION_PIN);

  // If the switch is flipped
  if (currentIgnitionSwitchState != lastIgnitionSwitchState)
  {
    // DEBUG
    Serial.println("change of status");

    // Press the button
    Joystick.setButton(IGNITION_BUTTON, 1);

    // Turn on/off the led based on the ignitition button switch
    UpdateIgnitionLedState(currentIgnitionSwitchState);

    // Update the last status, so we won't update the status every loop
    lastIgnitionSwitchState = currentIgnitionSwitchState;
    
    // Reset the timer to release the trigger
    timeElapsedIgn = 0;
    checkToReleaseIgnButtonFlag = true;
  }

  // Check if the button need to be released after a status change
  bool r = checkToReleaseTheButton(IGNITION_BUTTON, timeElapsedIgn, checkToReleaseIgnButtonFlag);

  if (r == true)
  {
    checkToReleaseIgnButtonFlag = false;
  }

}


// IGNITION LED

void UpdateIgnitionLedState(int currentIgnitionSwitchState)
{
    if (currentIgnitionSwitchState == HIGH)
    {
      digitalWrite(LED1_PIN, HIGH);
    }
    if (currentIgnitionSwitchState == LOW)
    {
      digitalWrite(LED1_PIN, LOW);
    }
}


// START ENGINE BUTTON

void UpdateStartButtonState()
{
  // Start Engine
  int currentButtonState = !digitalRead(START_ENGINE_PIN);
  
  if (currentButtonState != lastStartButtonState)
  {
      Joystick.setButton(START_ENGINE_BUTTON, currentButtonState);  // 0 is Button 1, which is pin 8
      lastStartButtonState = currentButtonState;
  }
}


// BRAKE BIAS

void UpdateBrakeBiasState()
{  
  bool isButtonReleased = false;
  unsigned char result = rotary.process();

  //Serial.println("rotary: " + result);
  
  if (result == DIR_CW) 
  {
    counter++;
    Serial.println(counter);
    
    // Press the button
    Joystick.setButton(BRAKE_BIAS_FORWARD_BUTTON, 1);
    timeElapsedBrkBias = 0;
    checkToReleaseBrkBiasButtonFlag = true;
    rotarySide = "CW";
  } 
  else if (result == DIR_CCW) 
  {
    counter--;
    Serial.println(counter);

    // Press the button
    Joystick.setButton(BRAKE_BIAS_BACKWARD_BUTTON, 1);
    timeElapsedBrkBias = 0;
    checkToReleaseBrkBiasButtonFlag = true;
    rotarySide = "CCW";
  }

  if (rotarySide == "CW")
  {
    isButtonReleased = checkToReleaseTheButton(BRAKE_BIAS_FORWARD_BUTTON, timeElapsedBrkBias, checkToReleaseBrkBiasButtonFlag);
  }
  else if (rotarySide == "CCW")
  {
    isButtonReleased = checkToReleaseTheButton(BRAKE_BIAS_BACKWARD_BUTTON, timeElapsedBrkBias, checkToReleaseBrkBiasButtonFlag);
  }
  /*else
  {
    Joystick.setButton(BRAKE_BIAS_FORWARD_BUTTON, 0);
    Joystick.setButton(BRAKE_BIAS_BACKWARD_BUTTON, 0);
  }*/

  //Serial.println("result: " + result);

  //Serial.println("rotarySide: " + rotarySide);
  
  if (isButtonReleased == true)
  {
    checkToReleaseBrkBiasButtonFlag = false;
  }
}



// HELPER

bool checkToReleaseTheButton(int button, elapsedMillis timeElapsed, bool checkToReleaseFlag)
{
  bool result;
  
  // After the INTERVAL in milliseconds, release the joystick button
  if ((timeElapsed > INTERVAL) && checkToReleaseFlag == true) 
  {
    // DEBUG
    Serial.println("time ups");  
    
    // Release the button
    Joystick.setButton(button, 0);
    
    result = true;
  }
  else
  {
    result = false;
  }

  return result;
}
