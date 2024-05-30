#include<Wire.h>
#include <Joystick.h>

#define DEBUGTHQ TRUE
#define HID_ID    0x26     // Unique HID Device ID, randomly chosen

// Slew sensor definitions
#define SLEW_ADR  0x41     // I2C Addrewss of the warthog slew sensor
#define CTR_REG1  0x0F     // Control Register 1
#define CTR_REGT  0x2D     // Timing Control Register
#define CTR_REG2  0x2E     // Control Register 2
#define ID_CODE   0x0C     // 
#define X_REG     0x10     // X Register
#define Y_REG     0x11     // Y Register
#define XP_REG    0x12
#define XN_REG    0x13
#define YP_REG    0x14
#define YN_REG    0x15

const int uBtns    = 8;     // Number of buttons on the device
const int hBtns    = 4;     // Hat switches (U, D, L, R)
const int dTime    = 50;    // Delay time between loops
int i;                      // Generic counter variable

int xSlew = 0;              // Slew X-Axis value;
int ySlew = 0;              // Slew Y-Axis value;
int sBtn_CurrState;         // Current state of the button
int sHat_CurrState;         // or hat direction being checked
int sBtn_LastState[uBtns];  // The last recorded state of each button
int sHat_LastState[hBtns];  // and hat directions
int sBtn[uBtns] = {};       // Digital pins assigned to each button
int sHat[hBtns] = {};       // and hat direction
bool hVal_Changed = false;  
byte slewRead;

// Joystick (HID ID, Type, Buttons, Hats, X-Aix, Y-Axis, Z-Axis, Rx-Axis, Ry-Axis, Rz-Axis, Rudder, Throttle, Accelerator, Brake, Steering);
Joystick_ THQ(HID_ID, JOYSTICK_TYPE_JOYSTICK, uBtns, 1, false, false, false, false, false, false, false, false, false, false, false);
const bool initAutoSendState = true;


char xZero = 0;
char yZero = 0;

void setup() {
  // Initialise the I2C connection
  #ifdef DEBUGTHQ 
    Serial.begin(9600);
  #endif
  Wire.begin();

  SlewInit();
  SlewSetup(xZero, yZero, 25, 25);

  // Initialise the pin mode, pullup resistors and the last known state of each button and hat direction
  for (i=0; i<uBtns; i++) {
    pinMode(sBtn[i], INPUT);
    digitalWrite(sBtn[i], HIGH);
    sBtn_LastState[i] = 0;
  }
  for (i=0; i< hBtns; i++) {
    pinMode(sHat[i], INPUT);
    digitalWrite(sHat[i], HIGH);
    sHat_LastState[i] = 0;
  }

  THQ.begin(initAutoSendState);

}

void loop() {
  // Read X-Axis and Y-Axis values from slew via I2C


  // Check for change in state of each hat direction and if it has changed send new state to HID
  for (i=0; i<hBtns; i++)  {
    sHat_CurrState = !digitalRead(sHat[i]);
    if (sHat_CurrState != sHat_LastState[i]) {
      hVal_Changed = true;
      sHat_LastState[i] = sHat_CurrState;
    }
  }

  // Something has changed since last check
  if (hVal_Changed) {
    // No switch is currently active, release hat switch
    if ((sHat_LastState[0] == 0) && 
        (sHat_LastState[1] == 0) &&
        (sHat_LastState[2] == 0) &&
        (sHat_LastState[3] == 0)) {
          THQ.setHatSwitch(0, -1);
        }

    // Up Switch pressed
    if (sHat_LastState[0] == 1) {
      THQ.setHatSwitch(0, 0);
      #ifdef DEBUGTHQ 
        Serial.println("POV Up pressed!"); 
      #endif
    }
    // Right Switch pressed
    if (sHat_LastState[1] == 1) {
      THQ.setHatSwitch(0, 90);
       #ifdef DEBUGTHQ 
        Serial.println("POV Right pressed!"); 
      #endif
    }
    // Down Switch pressed
    if (sHat_LastState[2] == 1) {
      THQ.setHatSwitch(0, 180);
       #ifdef DEBUGTHQ 
        Serial.println("POV Down pressed!"); 
      #endif
    }
    // Left Switch pressed
    if (sHat_LastState[3] == 1) {
      THQ.setHatSwitch(0, 270);
       #ifdef DEBUGTHQ 
        Serial.println("POV Left pressed!"); 
      #endif
    }

  }

  // Check for change in state of each button and if it has changed send new state to HID
  for (i=0; i<uBtns; i++) {
    sBtn_CurrState = !digitalRead(sBtn[i]);
    if (sBtn_CurrState != sBtn_LastState[i]) {
      THQ.setButton(i, sBtn_CurrState);
      sBtn_LastState[i] = sBtn_CurrState;  
      #ifdef DEBUGTHQ
        if (sBtn_CurrState == HIGH) {
          Serial.print("Button ");
          Serial.print(i);
          Serial.println(" pressed");
        }
        if (sBtn_CurrState == LOW) {
          Serial.print("Button ");
          Serial.print(i);
          Serial.println(" released");
        }
      #endif
    }
  }

  delay (dTime);
}

void SlewInit() {
  delay(100);
  char Reset_Status = 0;
  while (Reset_Status != 0xF0) {
    Reset_Status = ReadI2CReg(SLEW_ADR, CTR_REG1);
  }

  WriteI2CReg(SLEW_ADR, CTR_REGT, 0x06);

  char X, Y;                
  char xTemp=0, yTemp=0;
  ReadXAxis();  
  ReadYAxis(); 
  delay(5);
  for (byte i = 0; i<4; i++)
  {
    xTemp += ReadXAxis();
    yTemp += ReadYAxis();
    delay(5);
  }
  xZero = xTemp/4;
  yZero = yTemp/4;
  if (ReadI2CReg(SLEW_ADR, ID_CODE) != 0x0C) Serial.println("Slew sensor failed to respond.");
}

void SlewSetup(char xNull, char yNull, byte xDelta, byte yDelta) {
  char xp, xn, yp, yn;
  xp = xNull + xDelta;
  xn = xNull - xDelta;
  yp = yNull + yDelta;
  yn = yNull - yDelta;
  WriteI2CReg(SLEW_ADR, XP_REG, xp);
  WriteI2CReg(SLEW_ADR, XN_REG, xn);
  WriteI2CReg(SLEW_ADR, YP_REG, yp);
  WriteI2CReg(SLEW_ADR, YN_REG, yn);

  byte temp = ReadI2CReg(SLEW_ADR, CTR_REG1);
  temp = temp & 0b11110111;
  temp = temp & 0b01111111;
  temp = temp & 0b10001111;
  temp = temp | 0b01010000;
  temp = temp | 0b00000100;
  WriteI2CReg(SLEW_ADR, CTR_REG1, temp);
}

char ReadI2CReg(byte slave_adr, byte reg) {
  Wire.beginTransmission(slave_adr);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(slave_adr, (byte)1);return Wire.read();
}

void WriteI2CReg(byte slave_adr, byte reg, byte buf) {
  Wire.beginTransmission(slave_adr);
  Wire.write(reg);
  Wire.write(buf);
  Wire.endTransmission();
}

char ReadXAxis(){
  return ReadI2CReg(SLEW_ADR, X_REG);
}

char ReadYAxis(){
  return ReadI2CReg(SLEW_ADR, Y_REG);
}