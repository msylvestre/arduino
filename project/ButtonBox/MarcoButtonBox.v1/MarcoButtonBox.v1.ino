// Marco's Button Box
// Written : around Jan 16th 2021
//--------------------------------------------------------------------

#include <Joystick.h>
#include <elapsedMillis.h>


//======================================================================
// VARIABLES DECLARATION

// Libraries
Joystick_ Joystick;         // Used to press the cutton on the joystick 
elapsedMillis timeElapsed;  // Used to calucale easily the time elapse between 2 event

// Pins setting
const int START_ENGINE_PIN = 0; // Windows Button 1, Joystick 0
const int IGNITION_PIN = 12;     // Windows Button 2, Joystick 1
const int LED1_PIN = 13;         // Ignition Led

// List of Windows joystick button used.
const int IGNITION_BUTTON = 0;
const int START_ENGINE_BUTTON = 1;

// AMount of time we keep the button pushed
const int INTERVAL = 200; //milliseconds

// Use to trigger the release of the button after the TRIGGER_INTERVAL
bool checkToReleaseTheButtonFlag = false;

// Last state of the button
int lastStartButtonState = 0;
int lastIgnitionSwitchState = 0;


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
  //Serial.begin(9600);  // Enable the serial pour to print to the console. CTRL+SHIFT+M to open the serial monitor
}
  

void loop() 
{
  // Act when the Ignition is turned on/off
  UpdateIgnitionSwitchState();

  // Act when the Start button is pressed
  UpdateStartButtonState();

  delay(50);  //milliseconds
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
    //Serial.println("change of status");

    // Press the button
    Joystick.setButton(IGNITION_BUTTON, 1);

    // Turn on/off the led based on the ignitition button switch
    UpdateIgnitionLedState(currentIgnitionSwitchState);

    // Update the last status, so we won't update the status every loop
    lastIgnitionSwitchState = currentIgnitionSwitchState;
    
    // Reset the timer to release the trigger
    timeElapsed = 0;
    checkToReleaseTheButtonFlag = true;
  }

  // Check if the button need to be released after a status change
  checkToReleaseTheButton();
}


void checkToReleaseTheButton()
{
  // After the INTERVAL in milliseconds, release the joystick button
  if (timeElapsed > INTERVAL && checkToReleaseTheButtonFlag == true) 
  {
    // DEBUG
    //Serial.println("time ups");  
    
    // Release the button
    Joystick.setButton(IGNITION_BUTTON, 0);
    
    // Set the flag so we won't set the button again until the next state change
    checkToReleaseTheButtonFlag = false;
  }
}


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
