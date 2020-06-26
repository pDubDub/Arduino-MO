
// import statements
#include "Arduino.h"
#include <Servo.h>                          // unsure if this will be needed in Mega-3
#include <Stepper.h>
#include <Wire.h>                           // for receiving messages over I2C

//_____________________________________________
// declare constants for pin numbers

//_____________________________________________
// declare other constants
const int I2C_ADDRESS = 2;                  // I2C address. Master would be 0, slaves would be 1 and 2
const long BLINK_INTERVAL = 100;            // interval at which to blink (milliseconds)
                                            // making this smaller, makes blink pattern faster

//const int SERVO_THROW = 160;         // how far does servo 1 move                             *4

const int STEPS_PER_REV = 2048;       // change this to fit the number of steps per revolution  *7
const int rolePerMinute = 1;          // Adjustable range of 28BYJ-48 stepper is 0~17 rpm

//_____________________________________________
// declare variables
  bool isReady = false;
  bool isAwake = false;                         // defines whether MO is sleeping or ready for commands
  
  // previousMillis use in internal LED blink animated pattern
  //    Generally, you should use "unsigned long" for variables that hold time
  //    The value will quickly become too large for an int to store
  unsigned long currentMillis = 0;
  unsigned long previousBlinkLEDMillis = 0;
  
  unsigned long previousMillis = 0;         // will store last time LED was updated             *2 
  int frame = 0;                            // used to track frame in blink pattern             *2

  unsigned long previousMillisStepper = 0; // for stepper motor version
  int stepperFrame = 0;
  
    // booleans

  int stepsMoved = 0;   // *7 - add descripters here *********
  int stepsToMove = 0;

  String message;                           // for received I2C message

//_____________________________________________
// declare objects

  // initialize the stepper library on pins n1 through n2:
  Stepper myStepperA(STEPS_PER_REV, 22, 24, 23, 25);
  Stepper myStepperB(STEPS_PER_REV, 28, 30, 29, 31);
  Stepper myStepperC(STEPS_PER_REV, 34, 36, 35, 37);
  Stepper myStepperD(STEPS_PER_REV, 40, 42, 41, 43);


//_____________________ SETUP ( runs once )________________________
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);                                   // was previously 115200. changed to 9600 to match other Megas.

  Wire.begin(I2C_ADDRESS);                                // for I2C communication on SDA and SCL pins
  Wire.onReceive(receiveEvent);
  
  pinMode(LED_BUILTIN, OUTPUT);                           // for internal LED blink           *2

  myStepperA.setSpeed(rolePerMinute);       // front
  myStepperB.setSpeed(rolePerMinute);       // right
  myStepperC.setSpeed(rolePerMinute);       // back
  myStepperD.setSpeed(rolePerMinute);       // left

//  Serial.println("Mega-3 BBR setup complete.");

  // TODO - I'm really starting to think at the end of this setup, Mega-3 should query Mega-1 for ready and awake states.
  
} // end SETUP


//____________________ MAIN LOOP ( runs continuously )_________________________
void loop() {

  blinkRunningLED();

  if (isAwake) {
      updateSteppers();
  }


} // end main LOOP