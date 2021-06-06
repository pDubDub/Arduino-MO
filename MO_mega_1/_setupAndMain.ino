// import statements
#include "Arduino.h"
#include "IRremote.h"                               // *Capability 1A
#include <LiquidCrystal.h>                          // *4C
//#include <Servo.h>                                  // *5
#include <Wire.h>                                   // *1,3 - for I2C communication to slave Arduinos & mpu6050
#include <Adafruit_PWMServoDriver.h>                // f5 - for 16-channel I2C Servo Driver

//____________________ declare constants (for pin numbers) ____________________

  // pin 2 reserved for INT on MPU-6050 IMU                                                     *1
  const int IR_RECEIVER = 3;            // Signal Pin of IR receiver to Arduino Digital Pin 3   *1A
  const int RED_LAMP_LED = 4;           // red LED for the contaminant siren lamp               *5A
  const int SIREN_LIFT_SERVO = 5;                                                       //      *5A       XXXXX
  const int SIREN_SPIN_CSERVO = 6;                                                      //      *5A       XXXXX
  
  const int RGB_LED_RED = 7;               // define the RGB LED pins                           *4B
    // Something in this code causes a crash when using RGB_RED on pin 9 or 10
    // Pins are fine when used in "debugging_RGB_LED" sketch
  const int RGB_LED_GREEN = 8;
  const int RGB_LED_BLUE = 11;
  const int ALERT_TEMPERATURE = 90;

//____________________ declare other constants ____________________

  const long BLINK_INTERVAL = 100;            // interval at which to blink (milliseconds)         *4A
                                            // making this smaller, makes blink pattern faster

  // These values determine the speed of sine wave LED cycle.                                     *4
  // Used to use a constant increment of 0.001 for each loop cycle. Now we compute based on delta-time interval since last loop cycle.
  float PULSE_WAVELENGTH = 6.283;       // sine wave cycle (6.283 radians is pi*2, or one full cycle)
  float PULSE_PERIOD = 1.50;            // 1.5 seconds is chosen speed  

//____________________ declare variables ____________________

//  enum BotState { 
//    unknown,se
//    starting,
//    failed,
//    sleeping,
//    awake
//    };
//    // sleeping would be standby
//  BotState readyState = unknown;
  
  // 1B - Bluetooth
  String lastBTCommand = "";
  String newBTCommand = "";
  String command = "meep";                        // 3 - nessage to I2C slaves
  char message[8];                                // 3 - Wire.write() actually needs char array, not a String

  //   TIMER variables:
  //          previousBlinkLEDMillis use in internal LED blink animated pattern
  //          enerally, you should use "unsigned long" for variables that hold time
  //          The value will quickly become too large for an int to store
  unsigned long currentMillis = 0;
  unsigned long previousIRMillis = 0;
  unsigned long previousBTMillis = 0;
  unsigned long previousBlinkLEDMillis = 0;                               // 4A - last running LED update
  unsigned long previousPulseUpdateMillis = 0;                            // 4B - pulsing LED
  unsigned long previousLCDMillis = 0;      // similar function for LCD screen                  *4
//  unsigned long i2Ctimer = 0;                       // 3 - millis timer for demonstration i2c functionality
  unsigned long previousTestServoMillis = 0;
  unsigned long previousReadyUpdate = 0;
      // I'm using this to periodically send ready state to slaves at present, rather than having them query
  unsigned long previousSync = 0;
      // using this to periodically sync Megas. Could probably combine with ReadyUpdate.

  // mpu6050:
  const int mpu6050Address = 0x68; // MPU6050 I2C address
  float AccX, AccY, AccZ;
  float GyroX, GyroY, GyroZ;
  float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
  float roll, pitch, yaw;
  float mpu6050Temperature;
  float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
  float elapsedTime6050, currentTime6050, previousTime6050, previous6050read;
  int errorSamples = 0;
   
  int frame = 0;                            // used to track frame in blink pattern             *4
  int updateLCDframe = -6;
  
  int pulseBrightnessValue = 0;                   // used for pulsing green RGB LED              *4B
  float pulseMaxBrightness = 50;                  // changes max brightness of green LED pulse
  float wave = 4.712;                             // 4.712 value starts wave at the bottom
  enum RGBPulseColor { red, green, blue, orange, off };
  RGBPulseColor desiredColor = green;                  // used to make *3 a multicolor pulse
  RGBPulseColor pulseColor = green;

  bool batteryIsLow = false;                // placeholder variable (no hardware to set this yet)
  bool startupDidFail;
  bool tempIsHigh;
  bool isReady;                            // MO can be on and functional, yet asleep
  bool isAwake;                           // defines whether MO is sleeping or alert & ready for commands
  bool previousIsAwake = false;
  bool pulsingLEDisOn = true;                    // allows strobe to turn off at bottom of pulse *4
  bool sirenOn = false;                           // *6
  bool sirenDidChange = true;

  // 4C - used in LCD screen functionality
  int seconds, minutes;
  char buffer2[16];

//_____________________________________________
// declare objects
  IRrecv irrecv(IR_RECEIVER);         // create instance of 'irrecv' for IR remote
  decode_results results;             // create instance of 'decode_results' for IR remote
//  Servo sirenLiftServo;               // create servo object to control a servo             *5
//  Servo sirenSpinServo;               // continous servo                                    *5
  
//                  BS  E  D4 D5  D6 D7
  LiquidCrystal lcd(48, 49, 50, 51, 52, 53);                                             // *4

  // direct connected test servo. Pin 12, Min 500, Max 2500, middle 1420 microseconds
//  MoServo servo1 = MoServo(12, 500, 2500, 1420);   // in microseconds
      // TODO this servo used in f1, but is obselete
  
  // BlueTooth mesasges become "servo1.commandTo(newBTCommand, 800)" which tells servo1 MoServo object where to go to.
  // servo1.updateServo() call in loop moves the servo each cycle
 
  
  Adafruit_PWMServoDriver pwmServoBoard_1 = Adafruit_PWMServoDriver(0x40);
  Adafruit_PWMServoDriver pwmServoBoard_2 = Adafruit_PWMServoDriver(0x41);
//  int SERVOMIN = 100;   // right
//  int SERVOMID = 290;   // center
//  int SERVOMAX = 480;   // left

  int MIN_MICRO = 1000;
  int MAX_MICRO = 2000;
  
  int servo1pos = 290;
  int servo2pos =290;
  bool servo1cw = true;

  bool goLow = false;       // what is this?

  int testChannel = 0;      // just a temp var for testing servos in sequence

  // new servo constructor test
    // channel, MIN, MAX, START, CENTER
    // note MIN is actually furthest right/CW, while MAX is actually furthest left/CCW
  MoServo moServo00Louvers = MoServo("headLouver00", 0, 900, 2101, 2100, 2100);              // rear head louvers
      // should I open louvers on sleep?
  MoServo moServo01SirenLift = MoServo("sirenLift01", 1, 700, 2101, 2100, 2100);            // siren lift
  MoServo moServo02SirenSpin = MoServo("sirenSpin02", 2, 1400, 1600, 1500, 1500);           // siren spin
      // Siren Spin servo works like this, with easing, although that servo is really noisy.
  MoServo newTestServo3 = MoServo("testPan3", 3, 900, 2000, 1500, 1500);                    // head pan
  MoServo newTestServo4 = MoServo("testTilt4", 4, 1000, 2000, 1500, 1500);                  // head pitch
  MoServo newTestServo5 = MoServo("testRoll5", 5, 1000, 2000, 1500, 1500);                  // head roll
  MoServo newTestServo6 = MoServo("test6", 6, 1000, 2000, 1500, 1500);                      // neck lift
  MoServo newTestServo7 = MoServo("test7", 7, 1300, 1600, 1420, 1420);                      // neck lean
  
  MoServo newTestServo8 = MoServo("testTrack8", 8, 1000, 2000, 1500, 1500);
      // TODO - altering the last two values seems to crash servo8
  MoServo newTestServo9 = MoServo("testShrug9", 9, 1000, 2000, 1500, 1500);
  MoServo newTestServo10 = MoServo("testLean10", 10, 1300, 1600, 1420, 1420);   // tuned for pretty close center position
          // on neck lean, lower microseconds are more forward

//  int microsec = 0;

//____________________ SETUP ( runs once ) _________________________
void setup() {

  /* startup sequence:
  *     • confirm that i2C communication to other megas is working
  *     • should send slow move commands to lower head, torso and foot in case they're not down (as after a fall)
  *               (actually maybe all movements back to default)
  *     • after timer to match setup movements, set booleans isCompact and isFootDown to true, and also isAwake to false?
  *     • await time for IMU readings to stabilize
  *       
  *     - if any startup task fails, then startupDidFail = true;  
  *     
  *     • set isReady = !startupDidFail
  *       • send isReady to slaves -> Mega-2 plays startup chime
  *     • pulsing LED starts (green if OK, orange if startup did fail)    
  *     • don't accept IR or BT commands is !isReady
  */

  // new:
//  readyState = starting;

  // old/vestigial:
  startupDidFail = false;
  isReady = false;
  isAwake = false;

  // Since these are somewhat exclusive states (can't be awake and not ready), should this really be one enum variable?

  
  Serial.begin(9600);         // this used to be 11520. don't know why. changed it to 9600

  Serial.println("\nMICROBE OBLITERATOR STARTUP ROUTINE INITIATED");

  sendMessageToAllListeners("ready:0");
    // fixes a condition where if Mega-2 is already running (thinking isReady) and then Mega-1 reboots, 
    //      it snores below at 'awake:0' before it gets 'ready:1' and plays wakeup chime.

  // TODO - can I get MO-2 Serial to print a message that "MO STARTUP ROUTINE INITIATED" ??

  
  // for IR Remote *1
  irrecv.enableIRIn();  // Start the receiver
  Serial.println("\n *  IR Receiver Decode enabled.");

  // 1B - Bluetooth
  Serial.println(" *  Configuring Bluetooth communication:");
  Serial1.begin(9600);
  sendBTCommand("AT");                  // calls sendBTCommand function to setup the BlueTooth module from Arduino
  sendBTCommand("AT+ROLE0");            // makes AT-09 BT module a peripheral
  sendBTCommand("AT+UUID0xFFE0");
  sendBTCommand("AT+CHAR0xFFE1");
  sendBTCommand("AT+NAMEM-O");
  Serial.println("    Bluetooth communication enabled.");     // dummy message, eventually needing conditional

  // confirm i2c communication
  Wire.begin(0);                                          // 3 - starts I2C communication as Master to slaves
  //Wire.setClock(1000000);                                 // TRIED setting this higher than default 100000 to see if would
                                                          //   eliminate PWM servo board movement jerks, but did not
                                                          
    // I think I could put the Wire.onReceive(receiveEvent) here, and then slaves can message the master too
  Serial.println(" *  I2C communication enabled.");           // dummy message, eventually needing conditional

  // mpu6050 setup:
  Wire.beginTransmission(mpu6050Address);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                             // Talk to the register 6B
  Wire.write(0x00);                             // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);                   // end the transmission
  Serial.println(" *  MPU 6050 gyro/accelerometer enabled.");
  

  //  isReady = false;
  //  isAwake = false;
  //  isCompact = true;
  //  isFootDown = true;  

  pinMode(LED_BUILTIN, OUTPUT);                           // for internal LED blink           *4

  pinMode(RGB_LED_RED, OUTPUT);                              // for RGB LED                      *4B
  pinMode(RGB_LED_GREEN, OUTPUT);
  pinMode(RGB_LED_BLUE, OUTPUT);
  pinMode(RED_LAMP_LED, OUTPUT);                             // siren                            *5
  
  digitalWrite(RGB_LED_RED, LOW);
  digitalWrite(RGB_LED_GREEN, LOW);
  digitalWrite(RGB_LED_BLUE, LOW);
  digitalWrite(RED_LAMP_LED, LOW);

  lcd.begin(16, 2);         // declares how many columns and rows on LCD screen                 *4

  pwmServoBoard_1.begin();
//  pwmServoBoard_1.setOscillatorFrequency(27000000);
  pwmServoBoard_1.setPWMFreq(50);  // This is the maximum PWM frequency

  pwmServoBoard_2.begin();
//  pwmServoBoard_2.setOscillatorFrequency(27000000);
  pwmServoBoard_2.setPWMFreq(50);  // This is the maximum PWM frequency

//pwmServoBoard_1.setPWM(0,0,SERVOMID); 
//delay(1000);
//
// pwmServoBoard_1.setPWM(0,0,SERVOMIN);
//delay(1000);
//
//pwmServoBoard_1.setPWM(0,0,SERVOMAX);
//delay(1000);
//pwmServoBoard_1.setPWM(0,0,SERVOMID);
//delay(1000);

//  sirenLiftServo.attach(SIREN_LIFT_SERVO);              // attaches the servo on pin x to the servo object
//  sirenSpinServo.attach(SIREN_SPIN_CSERVO);             // attach continuous servo on pin x to the servo object
//
//  sirenLiftServo.write(0);                              // makes servo start at 0
//  sirenSpinServo.writeMicroseconds(1454);               // fine tuned for zero rotation of FS90R c-r servo

    // this is a test of MoServo:
//  servo1.servo.attach(servo1.pin);                      // TODO - can this be done in constructor?
//  servo1.goToStart();
  // TODO - should there be a beginning position command here?


  isAwake = false;
  sendMessageToAllListeners("awake:0");
  // In addition to fixing a bug, this nicely causes him close his eyes before a reboot.
  
  delay(1000);                                    // simulating IMU stabilization time

  // In the future, do we instead set isReady = true above. 
  //        And if any setup steps fail, we set it to false.
  //
  //        Then we check IF(isReady) true before sending isReady to other listeners?

  calculate_IMU_error();                          // moved mpu6050 calibration to end of setup to observe difference.
  delay(20);



  if (startupDidFail) {            // readyState == failed)
    // causeed if a setup failure sets this readystate
    Serial.println("**** STARTUP ROUTINE FAILED ****");

    // TODO - set the pulse to Orange, maybe at a faster rate?
  } else {                        
    // if not failed, then startup must have been successful
//    readyState = sleeping;         // sleeping is standby state

    // these are vestigial old commands
    isReady = true;                                 // set isReady state and send to listeners
    sendMessageToAllListeners("ready:1");
    Serial.println("\nM-O STARTUP ROUTINE COMPLETE - State: isReady\n");

    // Added command for startup chime here.
    // Previously, Mo-2 would play chime when it received "ready:1"
    // Now it only plays chime at the end of Mo-1 setup()
    delay(1000);
    sendToI2CSlave("play-0", 1);                    

    // TODO - set LED pulse to Green

    servosToStart();                                              // this is a temporary function in f5 to center the servos.
  }

  // TODO - I think on startup, the servos need an initial PWM signal.
  //    servosToStart() is definitely not doing anything, because MoServo logic thinks they are already as start
  
} // end SETUP


//____________________ MAIN LOOP ( runs continuously ) _________________________
void loop() {

  // This was one solution to slaves rebooting and thus never hearing the ready:1 command
  // It sends the ready state once per minute.
//  if ((millis() - previousReadyUpdate) > 60000) {
//    if (isReady) {
//      sendMessageToAllListeners("ready:1");
//    } else {
//      sendMessageToAllListeners("ready:0");
//    }
//  }
  // This actually does not work as intended, at present, because Mo-2 plays the startup chime anytime it hears "ready:1"

  // *1A - IR Remote control
  readFromIRRemote();

  // 1B - Bluetooth control
  readFromBluetooth();

  // 1C - read mpu6050 gyro
  if ((millis() - previous6050read) > 100) {
    // This make us only read the 6050 roughly 10 times per second.
    // This appears to solve a problem where reading the 6050 every cycle created too much I2C traffic, 
    //   causing the OLED screen to mess up.
    // This may be adequate solution for head-leveling IMU. Not sure if there will be a problem when Mega-1 is
    //   reading 6050 at the same time as Mega-3 is reading a 9250.
    read6050imu();                                      //commenting out solves OLED problems.
    previous6050read = millis();
  }
  
  
  // *4A - blink pattern on internal and external LED
  blinkRunningLED();
  
  // *4B --  update the pulsing LED only is ON
  if (pulsingLEDisOn) {
      updatePulsingLED();
  }

  // rewrite of the pulsing LED:
  /*
   * Might make two functions in f4:
   *      - one to update the color (every loop)
   *      - a second to update the LED (only when (pulseColor != off) && (pulsingLEDisOn)
   *      
   *      Second func could be called by the first, and maybe pass in a color?
   * 
   * psuedocode:
   *  if (batteryIsLow) {
   *    desiredColor = blue
   *    // should probably trigger a parking command, and perhaps if too low, put him to sleep
   *  } else if (startupDidFail) { 
   *    desiredColor = orange
   *    // possibly even pulsing faster?
   *    // !isReady
   *    // should prevent action, both internal or IR/BT
   *  } else if (tempIsHigh) {
   *    desiredColor = red
   *  } else if (!isAwake) {
   *    desiredColor = blue
   *  } else {  
   *    desiredColor = off
   *    // will cause updatePulsingLED() to update pulsingLEDisOn = false when the current pulse reaches bottom.
   *  }
   *  
   *  if (pulsingLEDisOn) {
   *    updatePulsingLED(desiredColor)
   *  }
   *  
   *  // I need to review my logic that limits the strobe to starting/stopping/changing only at the bottom (dark) position of pulsing
   *  
   *  if (pulseColor != off) && (pulsingLEDisOn) {
   *    updatePulsingLED()
   *  }
   *  
   */

  

  // this was a millis function to send random int every 2 seconds for servo demonstration
  if (isAwake) {
    currentMillis = millis();
    if (currentMillis - previousTestServoMillis >= 1500) {
      previousTestServoMillis = currentMillis;
      int randomDegreeCommand = (int)random(35,145);

      Serial.print("Random Degree Command: "); Serial.print(randomDegreeCommand);
      Serial.print("      microseconds = "); Serial.println(map(randomDegreeCommand, 0, 180, 2500, 500));

      moServo00Louvers.commandTo(randomDegreeCommand);
      // skipping 1 and 2 (siren)
      newTestServo3.commandTo(randomDegreeCommand);
          // TODO - make servos 4 and 5 (pitch and roll) react to 6050 MPU
      newTestServo4.commandTo(randomDegreeCommand);
      newTestServo5.commandTo(randomDegreeCommand);
      newTestServo6.commandTo(randomDegreeCommand);
      newTestServo7.commandTo(randomDegreeCommand);

      newTestServo8.commandTo(randomDegreeCommand);
      newTestServo9.commandTo(randomDegreeCommand);
      
//      Serial.print("Moving servo "); Serial.print(testChannel);
//      Serial.print(" to new random microseconds = "); Serial.println(randomDegree);
  
      // f5 servosToCenter() method will center all test servos. 
      //     Will need similar functionality with servo class, to be called when isAsleep.
      

        /*
         *  We might redo these in the form of
         *    
         *  pwmServoBoard_1.writeMicroseconds( neckLiftServo.channel, randomMicros);
         *  
         *  Or perhaps better would be a moServo object "neckLiftServo"
         *    which can have a method "neckLiftServo.moveTo()"
         *    
         *    and then the object itself can call either "pwmServoBoard_1.writeMicroseconds" or "pwmServoBoard_2.writeMicroseconds"
         *    and go through its own personal limit checks.
         *    
         *    This way, each moServo object can have its own MIN, MAX and CTR values.
         *    
         */

        
    } // end of millis()
  } // end of isAwake

  //    servo1.commandTo(randomDegree);
  
// un-needed now that bluetooth now receives a command from iOS and sends it to servo A1
// but may reuse for later testing

  // update servo1, which is currently receiving commands from iOS slider!
//  servo1.updateServo();

//  newTestServo.updateServo();   // this runs every loop to move the servo just a touch towards destination

//  for (int i = 0; i < 4; i++) {
//    int microsec = 0;
//    Serial.print("Update servo ); Serial.println(i);
//    if (i <=7) {
//      // pwm board 1
//    } else {
//      // pwm board 2
//    }
////  }


// ** UPDATING  the servos each loop
  pwmServoBoard_1.writeMicroseconds(moServo00Louvers.getChannel(), moServo00Louvers.updateServo());
  
  pwmServoBoard_1.writeMicroseconds(moServo01SirenLift.getChannel(), moServo01SirenLift.updateServo());
  pwmServoBoard_1.writeMicroseconds(moServo02SirenSpin.getChannel(), moServo02SirenSpin.updateServo());
    // might put these in a conditional?

  pwmServoBoard_1.writeMicroseconds(newTestServo3.getChannel(), newTestServo3.updateServo());
  pwmServoBoard_1.writeMicroseconds(newTestServo4.getChannel(), newTestServo4.updateServo());
  pwmServoBoard_1.writeMicroseconds(newTestServo5.getChannel(), newTestServo5.updateServo());
  pwmServoBoard_1.writeMicroseconds(newTestServo6.getChannel(), newTestServo6.updateServo());
  pwmServoBoard_1.writeMicroseconds(newTestServo7.getChannel(), newTestServo7.updateServo());

  pwmServoBoard_2.writeMicroseconds(newTestServo8.getChannel(), newTestServo8.updateServo());
  pwmServoBoard_2.writeMicroseconds(newTestServo9.getChannel(), newTestServo9.updateServo());


  // TODO - Testing: It does sort of feel like iOS app can't do anything while the servo is still moving.
  //    This may be because of the delay built into the iOS app when it comes to the slider, so that app doesn't flood
  //    bluetooth to Arduino.

 
  if (isAwake != previousIsAwake) {
    // All this appears to do is set previousIsAwake when there is a change
    
    /* BUG - something causes isAwake or the LCD to go to 'sleeping' after about 20 seconds
     *  
     *  moved 4B above into an IF that solves it for the strobe, but LCD continued.
     *  
     *  the mere presence of Serial.print(isAwake) made the problem go away.
     *  and deleting Serial.println(isAwake) made it return.
     *  
     *  the remaining change detection statement here still keeps bug away
    */
  
  //    Serial.print("isAwake state changed to: "); Serial.println(isAwake);
      previousIsAwake = isAwake;
  }

  // 4C -- rear panel operation message LED screen
  updateRearLEDscreen();
  
  // 5A --  the Foreign Contaminent red siren light on M-O's head, currently set to trigger via #0 on the IR remote
  updateSirenLamp();
                            
} // end main LOOP
