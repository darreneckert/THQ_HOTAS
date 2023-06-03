// Program to setup an Arduino Pro Micro to provide additional 
// controls (ministick, hats, buttons and switches) to a
// VKB Throttle Quadrant (THQ) module, making use of the 3D
// printed left and right throttle bodies that have been designed 
// by Marco (@Marco_596178) at Printerbale.com which can be found 
// here - https://www.printables.com/model/405514-vkb-thq-hotas-throttle
//
// This sketch is for the left throttle controls 
//
// Darren Eckert
// 2023-06-03
// Version 0.1

#include <Joystick.h>

int i;                                // Generic counter
const int HID_ID = 0x27;              // Unique HID Device ID for this arduino
const int uBtns  = 4;                 // Number of buttons on this device
const int tDelay = 50;                // Time delay for loop() in ms, may need adjusting

// Defive digital pins used for each button
int sBtn[uBtns] = {2,                // Push button 1          HID button 0
                    3,                // Push button 2          HID button 1
                    4, 5};            // Push buttons 3 and 4   HID buttons 2 and 3

int sBtn_CurrState;
int sBtn_LastState[uBtns] = {0, 0, 0, 0};

// Joystick (HID ID, Type, Buttons, Hats, X-Aix, Y-Axis, Z-Axis, Rx-Axis, Ry-Axis, Rz-Axis, Rudder, Throttle, Accelerator, Brake, Steering);
Joystick_ THQ(HID_ID, JOYSTICK_TYPE_JOYSTICK, uBtns, 0, false, false, false, false, false, false, false, false, false, false, false);
const bool initAutoSendState = true;

void setup() {
  // Initialise digital pin modes and enable pull-up resistors
  for (i=0; i<uBtns; i++) {
    pinMode(sBtn[i], INPUT);
    digitalWrite (sBtn[i], HIGH);

    THQ.begin(initAutoSendState);
  }
}

void loop() {
  // Check for change in state of each button and if it has changed send new state to HID
  for (i=0; i<uBtns; i++) {
    sBtn_CurrState = digitalRead(sBtn[i]);
    if (sBtn_CurrState != sBtn_LastState[i]) {
      THQ.setButton(i, sBtn_CurrState);
      sBtn_LastState[i] = sBtn_CurrState;    
    }
  }

  delay (tDelay);

}
