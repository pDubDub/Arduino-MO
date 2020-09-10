
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
const int rpm = 1;          // Adjustable range of 28BYJ-48 stepper is 0~17 rpm

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

  const int mpu9250Address = 0x69;

  // for 9250 PID
  float target = 0;       // target angle for PID
    /*
     * Eventually, we would have multiple targets, on for pitch, roll and yaw.
     * And adjusting the target would result in movement.
     * 
     * 
     */
  unsigned long nextTime = 0;

  // PID value will need to be tuned under actual operational testing:
  float Kp = 7;          // (P)roportional Tuning Parameter
  float Ki = 6;          // (I)ntegral Tuning Parameter        
  float Kd = 3;          // (D)erivative Tuning Parameter       
  float iTerm = 0;       // Used to accumulate error (integral)
  float lastTime = 0;    // Records the last time function was called
  float maxPID = 255;    // The maximum value that can be output
  float oldValue = 0;    // The last sensor value

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

  // TODO - we appear to be calling .setSpeed() both in setup() and in updateSteppers()

  myStepperA.setSpeed(rpm);       // front
  myStepperB.setSpeed(rpm);       // right
  myStepperC.setSpeed(rpm);       // back
  myStepperD.setSpeed(rpm);       // left

  Serial.println("\n    MO-3 SETUP ROUTINE COMPLETE\n");

  // TODO - I'm really starting to think at the end of this setup, Mega-3 should query Mega-1 for ready and awake states.
  
} // end SETUP


//____________________ MAIN LOOP ( runs continuously )_________________________
void loop() {


  
  // f4 - running LED 
  blinkRunningLED();

  // Only run the controller once the time interval has passed
  if (nextTime < millis()) {
    nextTime = millis() + 10;           // runs loop every 10 ms (10ms = 100Hz)
    // pseudocode:
    //    angle = getAngle();           // angle is the current angle, so getAngle() would be to query 9250
    //    motorOutput = PID(target, angle);
    //    moveMotors(motorOutput);

    
  }

  
  // f5 - stepper motors
  if (isAwake) {
      updateSteppers();
  } else {
    // DONE - Yes! This will stop power draw by stepper controllers when we want the motors to stop.
    //    Otherwise, motors continued to draw 0.3A/ea, even when not moving.

    for (int i = 22; i <44; i++) {
      digitalWrite(i, LOW);
    }
  }

  // TODO - for some reason, current config with 3 Megas, 6050 AND the 9250 (unused), Mega-1 will freeze.
  //    Removing 9250 from i2c circuit appears to fix the problem for now.

} // end main LOOP
