/*
 * Example using the Rotary library, dumping integers to the serial
 * port. The integers increment or decrement depending on the direction
 * of rotation.
 *
 * This example uses polling rather than interrupts.
 */
#include <Joystick.h>
#include <elapsedMillis.h>
#include <Rotary.h>

Joystick_ Joystick;   
elapsedMillis timeElapsed;

// Rotary encoder is wired with the common to ground and the two
// outputs to pins 5 and 6.
Rotary rotary = Rotary(15, 14);

// AMount of time we keep the button pushed
const int INTERVAL = 300; //milliseconds
const int BRAKE_BIAS_FORWARD_BUTTON = 3;
const int BRAKE_BIAS_BACKWARD_BUTTON = 2;

// Use to trigger the release of the button after the TRIGGER_INTERVAL
bool checkToReleaseTheButtonFlag = false;

// Counter that will be incremented or decremented by rotation.
int counter = 0;
String rotarySide = "";

void setup() 
{
  // Initialize Joystick Library
  Joystick.begin();
  Serial.begin(57600);
}

void loop() 
{
  bool isButtonReleased = false;
  unsigned char result = rotary.process();
  
  if (result == DIR_CW) 
  {
    counter++;
    Serial.println(counter);

    Joystick.setButton(BRAKE_BIAS_FORWARD_BUTTON, 1);
    timeElapsed = 0;
    checkToReleaseTheButtonFlag = true;
    rotarySide = "CW";

  } 
  else if (result == DIR_CCW) 
  {
    counter--;
    Serial.println(counter);

    Joystick.setButton(BRAKE_BIAS_BACKWARD_BUTTON, 1);
    timeElapsed = 0;
    checkToReleaseTheButtonFlag = true;
    rotarySide = "CCW";
  }

  if (rotarySide == "CW")
  {
    isButtonReleased = checkToReleaseTheButton(BRAKE_BIAS_FORWARD_BUTTON);
  }
  else if (rotarySide == "CCW")
  {
    isButtonReleased = checkToReleaseTheButton(BRAKE_BIAS_BACKWARD_BUTTON);
  }

}

bool checkToReleaseTheButton(int button)
{
  bool result;
  
  // After the INTERVAL in milliseconds, release the joystick button
  if (timeElapsed > INTERVAL && checkToReleaseTheButtonFlag == true) 
  {
    // DEBUG
    Serial.println("time ups");  
    
    // Release the button
    Joystick.setButton(button, 0);
    
    // Set the flag so we won't set the button again until the next state change
    checkToReleaseTheButtonFlag = false;

    result = true;
  }
  else
  {
    result = false;
  }

  return result;
}
