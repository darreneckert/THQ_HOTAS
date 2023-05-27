// Program to setup an Arduino Pro Micro to provide additional 
// controls (ministick, hats, buttons and switches) to a
// VKB Throttle Quadrant (THQ) module, making use of the 3D
// printed left and right throttle bodies that have been designed 
// by Marco (@Marco_596178) at Printerbale.com which can be found 
// here - https://www.printables.com/model/405514-vkb-thq-hotas-throttle
//
// Darren Eckert
// 2023-05-18
// Version 0.1

#include <Joystick.h>

int i;                                // Generic counter variable
const int HID_ID = 0x26;              // Unique HID Device ID, randomly chosen
const int uBtns = 5;                  // Number of buttons on the device
const int tDelay = 25;                    // Time delay for loop() in ms

// Define digital pin to be used for each button
// const int sTDC  = 2;               // Digital Pin for TDC push button
int sBtn[uBtns] = {3, 4, 5, 6, 7};    // Digital Pins for buttons/switches

// Define Analog pins for each axis
const int xTDC_Axis  = A0;            // Analog pin for TDC X-Axis
const int yTDC_Axis  = A1;            // Analog pin for TDC Y-Axis
  

// Default states for axes and buttons
int xTDC = 0;
int yTDC = 0;

// int sTDC_CurrState;
// int sTDC_LastState = 0;

int sBtn_CurrState;
int sBtn_LastState[uBtns] = {0, 0, 0, 0, 0};

// Joystick (HID ID, Type, Buttons, Hats, X-Aix, Y-Axis, Z-Axis, Rx-Axis, Ry-Axis, Rz-Axis, Rudder, Throttle, Accelerator, Brake, Steering);
Joystick_ THQ(HID_ID, JOYSTICK_TYPE_JOYSTICK, uBtns, 0, true, true, false, false, false, false, false, false, false, false, false);
const bool initAutoSendState = true;


void setup() {
  // Initialise digital pin modes and enable pull-up resistors
  /* pinMode(sTDC, INPUT);
     digitalWrite(sTDC, HIGH); */

  for (i=0; i<uBtns; i++) {
    pinMode(sBtn[i], INPUT);
    digitalWrite (sBtn[i], HIGH);
  }

  THQ.begin(initAutoSendState);
}

void loop() {
  // Read axes and send to HID
  // X-Axis
  xTDC = analogRead(xTDC_Axis);
  xTDC = map(xTDC, 0, 1023, 0, 255);   // scale resolution to reduce jittering, need to test if necessary
  THQ.setXAxis(xTDC);

  // Y-Axis
  yTDC = analogRead(yTDC_Axis);
  yTDC = map(yTDC, 0, 1023, 0, 255);  // scale resolution to reduce jittering, need to test if necessary
  THQ.setYAxis(yTDC);

  // Read button states and send to HID
  /* sTDC_CurrState = digitalRead(sTDC);
     if (sTDC_CurrState != sTDC_LastState) {
       THQ.setButton (0, sTDC_CurrState);
       sTDC_LastState = sTDC_CurrState;
  } */

  for (i=0; i<uBtns; i++) {
    sBtn_CurrState = digitalRead(sBtn[i]);
    if (sBtn_CurrState != sBtn_LastState[i]) {
      THQ.setButton(i, sBtn_CurrState);
      sBtn_LastState[i] = sBtn_CurrState;    
    }
  }

  delay (tDelay);
}