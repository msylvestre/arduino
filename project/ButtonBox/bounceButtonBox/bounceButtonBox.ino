#include <Bounce.h>
#include <elapsedMillis.h>
#include <Rotary.h>
#include <Joystick.h>

Bounce key_on = Bounce(0, 10);
Bounce key_start = Bounce(1, 10);
Bounce shift_up = Bounce(8,10);
Bounce shift_down = Bounce(9,10);
Bounce pbrake = Bounce(10,10);

Bounce r_button_1 = Bounce(11,10);
Bounce r_button_2 = Bounce(12,10);
Bounce r_button_3 = Bounce(26,10);
Bounce r_button_4 = Bounce(13,10);
Bounce r_button_5 = Bounce(14,10);
Bounce r_button_6 = Bounce(15,10);
Bounce r_button_7 = Bounce(16,10);

Bounce l_switch_1 = Bounce(17,10);
Bounce l_switch_2 = Bounce(18,10);
Bounce l_switch_3 = Bounce(19,10);
Bounce l_switch_4 = Bounce(20,10);

Bounce enc_button_1 = Bounce(21,10);
Bounce enc_button_2 = Bounce(22,10);
Bounce enc_button_3 = Bounce(23,10);

elapsedMillis timeElapsed;
unsigned int t_interval = 200;   //interval to hold buttons on for when needed
unsigned int enc_interval = 20;  //interval for encoders

bool key_start_on = false;

Rotary rotary1 = Rotary(6, 7);
Rotary rotary2 = Rotary(2, 3);
Rotary rotary3 = Rotary(4, 5);

Joystick Joystick(A0, A1, 13);

void setup() {

  attachInterrupt(6, rotate, CHANGE);
  attachInterrupt(7, rotate, CHANGE);
  attachInterrupt(2, rotate, CHANGE);
  attachInterrupt(3, rotate, CHANGE);
  attachInterrupt(4, rotate, CHANGE);
  attachInterrupt(5, rotate, CHANGE);
  
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  pinMode(20, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);
  pinMode(23, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  
}

void loop() {

//UPDATE

  key_on.update();
  key_start.update();

  shift_up.update();
  shift_down.update();

  pbrake.update();

  r_button_1.update();
  r_button_2.update();
  r_button_3.update();
  r_button_4.update();
  r_button_5.update();
  r_button_6.update();
  r_button_7.update();

  l_switch_1.update();
  l_switch_2.update();
  l_switch_3.update();
  l_switch_4.update();

  enc_button_1.update();
  enc_button_2.update();
  enc_button_3.update();

//HANDLE KEY STARTER
  
  if (key_on.fallingEdge()) {       //Detects when key moves from OFF to ON
    Joystick.button(1, 1);
    timeElapsed = 0;
  }
  if (key_start.fallingEdge()) {    //Detects when key moves from ON to START
    key_start_on = true;
    Joystick.button(1, 1);
  }

  if (key_start.risingEdge()) {     //Detects when key moves from START to ON
    key_start_on = false;
    Joystick.button(1, 0);
  }
  if (key_on.risingEdge()) {        //Detects when key moves from ON to OFF
    timeElapsed = 0;
    Joystick.button(1, 1);
  }

  if (timeElapsed > t_interval && key_start_on == false) {
     Joystick.button(1,0);
  }

//HANDLE SHIFTER

  if (shift_up.fallingEdge()) {   //Detects when the switch moves up
    timeElapsed = 0;
    Joystick.button(2,1);
  }

  if (shift_down.fallingEdge()) {   //Detects when the switch moves down
    timeElapsed = 0;
    Joystick.button(3,1);
  }

  if (shift_down.risingEdge() || shift_up.risingEdge()) {   //Detects when the switch moves to the center from either direction
    timeElapsed = 0;
    Joystick.button(4,1);
  }

  if (timeElapsed > t_interval) {
     Joystick.button(2,0);
     Joystick.button(3,0);
     Joystick.button(4,0);
  }

//HANDLE PARKING BRAKE SWITCH

  if (pbrake.fallingEdge()) {
    Joystick.button(5,1);
    timeElapsed = 0;
  }

  if (pbrake.risingEdge()) {
    Joystick.button(5,1);
    timeElapsed = 0;
  }

  if (timeElapsed > t_interval) {
     Joystick.button(5,0);
  }

//HANDLE BUTTONS

  if (r_button_1.fallingEdge()) {
    Joystick.button(6,1);
  }
  if (r_button_1.risingEdge()) {
    Joystick.button(6,0);
  }

  if (r_button_2.fallingEdge()) {
    Joystick.button(7,1);
  }
  if (r_button_2.risingEdge()) {
    Joystick.button(7,0);
  }

  if (r_button_3.fallingEdge()) {
    Joystick.button(8,1);
  }
  if (r_button_3.risingEdge()) {
    Joystick.button(8,0);
  }

  if (r_button_4.fallingEdge()) {
    Joystick.button(9,1);
  }
  if (r_button_4.risingEdge()) {
    Joystick.button(9,0);
  }

  if (r_button_5.fallingEdge()) {
    Joystick.button(10,1);
  }
  if (r_button_5.risingEdge()) {
    Joystick.button(10,0);
  }

  if (r_button_6.fallingEdge()) {
    Joystick.button(11,1);
  }
  if (r_button_6.risingEdge()) {
    Joystick.button(11,0);
  }

  if (r_button_7.fallingEdge()) {
    Joystick.button(12,1);
  }
  if (r_button_7.risingEdge()) {
    Joystick.button(12,0);
  }

//HANDLE LED SWITCHES

  if (l_switch_1.fallingEdge()) {
    Joystick.button(13,1);
    timeElapsed = 0;
  }
  if (l_switch_1.risingEdge()) {
    Joystick.button(13,1);
    timeElapsed = 0;
  }

  if (l_switch_2.fallingEdge()) {
    Joystick.button(14,1);
    timeElapsed = 0;
  }
  if (l_switch_2.risingEdge()) {
    Joystick.button(14,1);
    timeElapsed = 0;
  }

  if (l_switch_3.fallingEdge()) {
    Joystick.button(15,1);
    timeElapsed = 0;
  }
  if (l_switch_3.risingEdge()) {
    Joystick.button(15,1);
    timeElapsed = 0;
  }

  if (l_switch_4.fallingEdge()) {
    Joystick.button(16,1);
    timeElapsed = 0;
  }
  if (l_switch_4.risingEdge()) {
    Joystick.button(16,1);
    timeElapsed = 0;
  }

  if (timeElapsed > t_interval) {
     Joystick.button(13,0);
     Joystick.button(14,0);
     Joystick.button(15,0);
     Joystick.button(16,0);
  }  

  if (timeElapsed > enc_interval) {
     Joystick.button(20,0);
     Joystick.button(21,0);
     Joystick.button(22,0);
     Joystick.button(23,0);
     Joystick.button(24,0);
     Joystick.button(25,0);
  }

//HANDLE ENCODER BUTTONS

  if (enc_button_1.fallingEdge()) {
    Joystick.button(17,1);
  }
  if (enc_button_1.risingEdge()) {
    Joystick.button(17,0);
  }

  if (enc_button_2.fallingEdge()) {
    Joystick.button(18,1);
  }
  if (enc_button_2.risingEdge()) {
    Joystick.button(18,0);
  }

  if (enc_button_3.fallingEdge()) {
    Joystick.button(19,1);
  }
  if (enc_button_3.risingEdge()) {
    Joystick.button(19,0);
  }

}

void rotate() {

// HANDLE ENCODER 1
  unsigned char result1 = rotary1.process();
  if (result1 == DIR_CW) {
    timeElapsed = 0;
    Joystick.button(20,1);
    Joystick.button(21,0);
  } else if (result1 == DIR_CCW) {
    timeElapsed = 0;
    Joystick.button(21,1);
    Joystick.button(20,0);
  }

// HANDLE ENCODER 2
  unsigned char result2 = rotary2.process();
  if (result2 == DIR_CW) {
    timeElapsed = 0;
    Joystick.button(22,1);
    Joystick.button(23,0);
  } else if (result2 == DIR_CCW) {
    timeElapsed = 0;
    Joystick.button(23,1);
    Joystick.button(22,0);
  }   
    
// HANDLE ENCODER 3
  unsigned char result3 = rotary3.process();
  if (result3 == DIR_CW) {
    timeElapsed = 0;
    Joystick.button(24,1);
    Joystick.button(25,0);
  } else if (result3 == DIR_CCW) {
    timeElapsed = 0;
    Joystick.button(25,1);
    Joystick.button(24,0);
  }    
     
}
