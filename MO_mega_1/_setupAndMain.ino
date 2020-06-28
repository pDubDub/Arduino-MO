// import statements
#include "Arduino.h"
#include "IRremote.h"                           // *1A
#include <LiquidCrystal.h>                      // *4C
#include <Servo.h>                              // *6
#include <Wire.h>                               // 3 - for I2C communication to slave Arduinos

//_____________________________________________
// declare constants for pin numbers
  // pin 2 reserved for INT on MPU-6050 IMU                                                     *5
  
  const int IR_RECEIVER = 3;            // Signal Pin of IR receiver to Arduino Digital Pin 3   *1A
  const int RED_LAMP_LED = 4;           // red LED for the contaminant siren lamp               *6A
  const int SIREN_LIFT_SERVO = 5;                                                       //      *6A
  const int SIREN_SPIN_CSERVO = 6;                                                      //      *6A
  
  const int RGB_LED_RED = 7;               // define the RGB LED pins                              *4B
  const int RGB_LED_GREEN = 8;
  const int RGB_LED_BLUE = 11;

  const int A_SERVO = 44;               // this is a test servo, trying to control from iOS

    // SOMETHING in this code causes a crash when using RGB_RED on pin 10
    // ALSO couldn't make things work on pin 9
    // pins are fine in debugging_RGB_LED sketch

//_____________________________________________
// declare other constants
const long BLINK_INTERVAL = 100;            // interval at which to blink (milliseconds)            *4A
                                            // making this smaller, makes blink pattern faster

  // these values determine the speed of sine wave LED cycle                                            *3
  // used to use a constant increment of 0.001 for each loop cycle
  // now we compute based on dt interval since last loop cycle
float PULSE_WAVELENGTH = 6.283;       // sine wave cycle (6.283 radians is pi*2, or one full cycle)
float PULSE_PERIOD = 1.50;            // 1.5 seconds is chosen speed  

//_____________________________________________
// declare variables

  int x = 0;        // for test servo
  bool positiveDirection = true;
  
  // 1B - Bluetooth
  String lastCommand = "";
  String newCommand = "";
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
  unsigned long previousLCDMillis = 0;      // similar function for LCD screen                  *6
  unsigned long i2Ctimer = 0;                       // 3 - millis timer for demonstration i2c functionality
  unsigned long previousTestServoMillis = 0;

//  float dt = 0;
   
  int frame = 0;                            // used to track frame in blink pattern             *2
  int updateLCDframe = -6;
  
  int pulseBrightnessValue = 0;                   // used for RGB LED                             *4B
//  int redBrightnessValue = 0;                   // used for RGB LED                             *4B
//  int greenBrightnessValue = 0;
//  int blueBrightnessValue = 0;
  float pulseMaxBrightness = 50;                      // changes max brightness of green LED pulse
  float wave = 4.712;                                 // used for pulsing green LED
                                                  // 4.712 value starts wave at the bottom
  enum RGBColor { red, green, blue };
  RGBColor desiredColor = green;          // attempting to make *3 a multicolor pulse
  RGBColor pulseColor = green;



  bool isReady = true;
  bool isAwake = false;       // defines whether MO is sleeping or ready for commands
  bool previousIsAwake = false;
  bool sleepStrobeIsOn = true;       // allows strobe to turn off at bottom of pulse
  bool sirenOn = false;                     //             *4
  bool sirenDidChange = true;

  // 4C - used in LCD screen functionality
  int seconds, minutes;
  int tempReading = 80;                     
  double tempK = 0;
  double tempF = 80;
  char buffer2[16];
  int tempDisp = 800;

//_____________________________________________
// declare objects
IRrecv irrecv(IR_RECEIVER);     // create instance of 'irrecv' for IR remote
decode_results results;      // create instance of 'decode_results' for IR remote
Servo sirenLiftServo;  // create servo object to control a servo                                       *4 
Servo sirenSpinServo; // continous servo                                                                *4

MoServo servo1 = MoServo(44, 500, 2500, 1420);   // in microseconds

//                BS  E  D4 D5  D6 D7
LiquidCrystal lcd(48, 49, 50, 51, 52, 53);                                             //          *6

//_____________________________________________
// SETUP          ( runs once )
void setup() {
// put your setup code here, to run once:
  Serial.begin(9600);         // this used to be 11520. don't know why. changed it to 9600

  Serial.println("\nMICROBE OBLITERATOR STARTUP ROUTINE INITIATED");
  Serial.println("IR Receiver Button Decode");            // for IRremote                     *1
  irrecv.enableIRIn(); // Start the receiver              // for IRremote                     *

  // 1B - Bluetooth
  Serial1.begin(9600);
  sendBTCommand("AT");                  // calls sendBTCommand function to setup the BlueTooth module from Arduino
  sendBTCommand("AT+ROLE0");            // makes AT-09 BT module a peripheral
  sendBTCommand("AT+UUID0xFFE0");
  sendBTCommand("AT+CHAR0xFFE1");
  sendBTCommand("AT+NAMEM-O");

  pinMode(LED_BUILTIN, OUTPUT);                           // for internal LED blink           *2

  Wire.begin(0);                                          // 3 - starts I2C communication as Master to slaves
    // I think I could put the Wire.onReceive(receiveEvent) here, and then slaves can message the master too
    
  sendMessageToAllListeners("ready:0");
    // fixes a condition where if Mega-2 is already running (thinking isReady) and then Mega-1 reboots, 
    //      it snores below at 'awake:0' before it gets 'ready:1' and plays wakeup chime.

  pinMode(RGB_LED_RED, OUTPUT);                                   // for RGB LED                      *4B
  pinMode(RGB_LED_GREEN, OUTPUT);
  pinMode(RGB_LED_BLUE, OUTPUT);
  pinMode(RED_LAMP_LED, OUTPUT);                             //                                 *4
  
  digitalWrite(RGB_LED_RED, LOW);
  digitalWrite(RGB_LED_GREEN, LOW);
  digitalWrite(RGB_LED_BLUE, LOW);
  digitalWrite(RED_LAMP_LED, LOW);

  sirenLiftServo.attach(SIREN_LIFT_SERVO);              // attaches the servo on pin x to the servo object
  sirenSpinServo.attach(SIREN_SPIN_CSERVO);             // attach continuous servo on pin x to the servo object

  sirenLiftServo.write(0);                              // makes servo start at 0

    // this is a test of MoServo:
  servo1.servo.attach(servo1.pin);                              // TODO - can this be done in constructor?
  servo1.start();
//  a_Servo.attach(A_SERVO);      // test servo, for control from iOS
  // TODO - should there be a beginning position command here?

  lcd.begin(16, 2);         // declares how many columns and rows on LCD screen                 *6
                  

  /* startup sequence:
  *     • should send slow move commands to lower head, torso and foot in case they're not down (as after a fall)
  *               (actually maybe all movements back to default)
  *     • after timer to match setup movements, set booleans isCompact and isFootDown to true, and also isAwake to false?
  *     • await time for IMU readings to stabilize
  *     • confirm that i2C communication to other megas is working
  *     • set bool isReady to true, play startup chime and start pulsing LED
  *       (don't except IR or BT commands if !isReady ?)
  *       
  */

  isAwake = false;
  sendMessageToAllListeners("awake:0");
  
  delay(1000);                                    // simulating IMU stabilization time

                                                  // confirm I2C communication

  // TODO - I guess in the future, we don't set isReady = true here. We set it above. 
  //            And if any setup steps fail, we set it to false.
  //
  //        Then we check IF(isReady) true before sending isReady to other listeners?
  
  isReady = true;                                 // set isReady state and send to listeners
//  lcd.setCursor(0, 0);                      // moved from lcd.begin line above
//  lcd.print("M-O Bot: on-line");           // sends initial message to LCD screen               *6   

  sendMessageToAllListeners("ready:1");
  Serial.println("M-O STARTUP ROUTINE COMPLETE - State: isReady\n");

} // end SETUP


//____________________ MAIN LOOP ( runs continuously ) _________________________
void loop() {

  // *1A - IR Remote control
  readFromIRRemote();

  // 1B - Bluetooth control
  readFromBluetooth();
  
  // *4A - blink pattern on internal and external LED
  blinkRunningLED();
  
  // *4B --  causes the RGB LED to pulse green only if bot is in sleep mode
  if (sleepStrobeIsOn) {
      updatePulsingLED();
  }

// this was a millis function to send random int 0-180 every 2 seconds for MoServo demonstration
//  currentMillis = millis();
//  if (currentMillis - previousTestServoMillis >= 2000) {
//    previousTestServoMillis = currentMillis;
//    int randomDegree = (int)random(0,181);
//    Serial.print("New random degree = ");
//    Serial.println(randomDegree);
//    servo1.commandTo(randomDegree);
//  }
// un-needed now that bluetooth now receives a command from iOS and sends it to servo A1  

  // update servo1, which is currently receiving commands from iOS slider!
  servo1.updateServo();

  // TODO - Testing: It does sort of feel like iOS app can't do anything while the servo is still moving.
  //    This may be because of the delay built into the iOS app when it comes to the slider, so that app doesn't flood
  //    bluetooth to Arduino.
  
  /* BUG - something causes isAwake or the LCD to go to 'sleeping' after about 20 seconds
   *  
   *  moved 4B above into an IF that solves it for the strobe, but LCD continued.
   *  
   *  the mere presence of Serial.print(isAwake) made the problem go away.
   *  and deleting Serial.println(isAwake) made it return.
   *  
   *  the remaining change detection statement here still keeps bug away
  */
  
  if (isAwake != previousIsAwake) {
//    Serial.print("isAwake state changed to: ");
//    Serial.println(isAwake);
    previousIsAwake = isAwake;
  }

// 4C -- rear panel operation message LED screen
updateRearLEDscreen();

// 6A --  the Foreign Contaminent red siren light on M-O's head, currently set to trigger via #0 on the IR remote
updateSirenLamp();
                            
} // end main LOOP



// FUNCTIONS:

// TODO - need to go through logic. There are still instances where an IR 'power' command or BT 'sleep/wake' don't actually toggle, but just repeat the previous state

 void toggleAwakeState() {
  isAwake = !isAwake;

  Serial.println("");
  Serial.print("M-O isAwake toggled to ");
  Serial.println(isAwake ? "Awake" : "Asleep");
  
  String message2send = "";
  if (isAwake) { 
    message2send = "awake:1";
  } else { 
    message2send = "awake:0";
    sleepStrobeIsOn = true;
  }
  sendToI2CSlave(message2send, 1);
    // FUTURE - also send to slave 2
  sendToI2CSlave(message2send, 2);

  // TODO - Isn't it more efficent to use sendMessageToAllListeners() function?
  
  Serial1.println(message2send);                                    // also send to app over BT
  Serial.println("\"" + message2send + "\" also sent to BT app");

  lcd.setCursor(0, 0);
  lcd.print(isAwake ? "M-O Awake       " : "zzz Sleep Mode  ");
    // TODO - this line appears twice: once here and also in LED display
  
  if (!isAwake && sirenOn) {            // if now asleep but sirenOn then automatically turn off siren lamp 
    sirenOn = !sirenOn;
    sirenDidChange = true; 
  }
 }

 void toggleSirenState() {
  if (isAwake) {                        // added conditional to only activate siren lamp when "Awake"
    sirenOn = !sirenOn;
    Serial.print("IR remote: 0 = Siren Lamp ");
    Serial.println(sirenOn ? "ON" : "OFF");
    lcd.setCursor(0,0);
    lcd.print(sirenOn ? "Contaminant " : "All Clean   ");
    sirenDidChange = true;
    if (sirenOn) {
      // tell Mega-2 to play sound that goes with red light
      // Note: in future, should really only send state to Mega-2, rather than play command.
      //  There might be times when we want to turn on siren apart from the Foreign Contaminent scene.
      sendToI2CSlave("play-12", 1);
      // TODO - send siren state message to listeners
    }
  } else {
    Serial.println("0 = Siren Lamp, but it's currently disabled by SLEEP MODE");    
  }
}

void sendMessageToAllListeners(String message) {
  // this function will send a String to all places at once

  Serial.println((String)" -> Sending message \"" + message + "\" to Mega-2 (slave 1) over I2C");
  sendToI2CSlave(message, 1);

  Serial.println((String)" -> Sending message \"" + message + "\" to Mega-3 (slave 2) over I2C");
  sendToI2CSlave(message, 2);

  Serial.println((String)" -> Sending message \"" + message + "\" to iOS app over BT");
  Serial1.print(message);
}

// TODO - build an i2C receiver, so that other Megas can send 'query' and Mega-1 will send out important states?
 //     Does iOS app need same? Or does it already have it below in BTListener?


 // TODO - Fix - Often, the first IR receive 'power' triggers isAwake
 //       But then the 2nd IR 'power' command sends 'wake' again

// *1A - the IR Remote
void readFromIRRemote() {
  if (irrecv.decode(&results)) {                                // have we received an IR signal?
    translateIR(); 
    irrecv.resume(); // receive the next value
  }  
}

void translateIR() {                        
      // takes action based on IR code received
      // this function was taken from tutorial for IR Remote

  // TODO - IR signals should change a botState variable, and behaviors like LCD or siren servos should be handled elsewhere based on botState
      
  currentMillis = millis();                 // gets current time
  if (currentMillis - previousIRMillis >= 500) {         // only runs loop 0.5 seconds after last time, replacing old delay(500) statement
    previousIRMillis = currentMillis;                  
                                                          // could test at 0. using Serial.print
                                                          // might not need millis if statement
                                                          // just doing it because tutorial used delay(500)
    switch(results.value)
    // describing Remote IR codes
    {
      case 0xFFA25D:                                    // "POWER" button
        toggleAwakeState();
        break;
      case 0xFFE21D: Serial.println("FUNC/STOP"); break;
      case 0xFF629D: Serial.println("VOL+"); break;
      case 0xFF22DD: Serial.println("FAST BACK");    break;
      case 0xFF02FD: Serial.println("PAUSE");    break;
      case 0xFFC23D: Serial.println("FAST FORWARD");   break;
      case 0xFFE01F: Serial.println("DOWN");    break;
      case 0xFFA857: Serial.println("VOL-");    break;
      case 0xFF906F: Serial.println("UP");    break;
      case 0xFF9867: Serial.println("EQ");    break;
      case 0xFFB04F: Serial.println("ST/REPT");    break;
      case 0xFF6897:                                      // button "0" = siren toggle
        toggleSirenState();
        break;
      case 0xFF30CF: Serial.println("1");
        // Note: "1" plays track 2, skipping the Startup Chime which actually lives at track 1
        sendToI2CSlave("play-4", 1);   // send command to slave number 1
        break;
      case 0xFF18E7: Serial.println("2");
        sendToI2CSlave("play-5", 1);        // send command to slave number 1
        break;
      case 0xFF7A85: Serial.println("3");    
          sendToI2CSlave("play-6", 1);      // send command to slave number 1
          break;
      case 0xFF10EF: Serial.println("4");    
          sendToI2CSlave("play-7", 1);      // send command to slave number 1
          break;
      case 0xFF38C7: Serial.println("5");    
          sendToI2CSlave("play-8", 1);      // send command to slave number 1
          break;
      case 0xFF5AA5: Serial.println("6");    
          sendToI2CSlave("play-9", 1);      // send command to slave number 1
          break;
      case 0xFF42BD: Serial.println("7");    
            sendToI2CSlave("play-10", 1);   // send command to slave number 1
            break;
      case 0xFF4AB5: Serial.println("8");    
            sendToI2CSlave("play-11", 1);   // send command to slave number 1
            break;
      case 0xFF52AD: Serial.println("9");    
            sendToI2CSlave("play-12", 1);   // send command to slave number 1
            break;
      case 0xFFFFFFFF: Serial.println(" REPEAT");break;  
    
      default: 
        Serial.println(" other IT signal : ");
        Serial.println(results.value);
  
    }// End SWITCH statement
  }
} //END translateIR function
     
// 1B - Bluetooth (4 functions from tutorial)
// TODO - don't love the presence of delay()'s
void sendBTCommand(const char * command) {
  Serial.println((String)"Command \"" + command + "\" sent to iOS app over BT");
  Serial1.println(command);
  
  delay(100);                         //wait some time
  
  char reply[100];
  int i = 0;
  while (Serial1.available()) {
    reply[i] = Serial1.read();
    i += 1;
  }
  reply[i] = '\0';   //end the string
  Serial.println((String)"Message \"" + reply + "\" received from AT-09");
//  Serial.println(reply);
  // Serial.println("Reply: successful");

  delay(50);                          // wait some time
}

String readSerial(){
  char reply[50];
  int i = 0;
  while (Serial1.available()) {
    reply[i] = Serial1.read();
    i += 1;
  }
  reply[i] = '\0';   //end the string
  if(strlen(reply) > 0){
    Serial.print("readSerial() BT Receeived: ");
    Serial.println(reply);
    // Serial.println("Houston we have a signal!");
  }
  String command(reply);
  return command;
}

// this is from tutorial and doesn't appear to be used.
void writeSerialToBLE(int value) {
  Serial1.println(value);
}

// this is also from the tutorial, and may be superfluous, as below I was able to write to Serial1 with one command
void writeToBLE(char value) {
  Serial.print("Writing hex :");
  Serial.println(value, HEX);
  Serial1.write(value);
}

void readFromBluetooth() {
  currentMillis = millis();                               // gets current time
  if (currentMillis - previousBTMillis >= 500) {         // only runs loop 0.5 seconds after last time - no delay()
    previousBTMillis = currentMillis;
                 
    // reacting to various BT message from iOS app:
    newCommand = readSerial();                // reads message from BTE
    if(newCommand.length() > 1) {             // and if longer than 0 characters…
      Serial.println((String)" readFromBluetooth() New BT Command: " + newCommand);       // TODO - this line is redundant, as readSerial() also prints
//      Serial.println(newCommand);
      // lastCommand = newCommand;
      // lastCommand variable is for detecting change, so we would only scan switch if it's a new command
      if(newCommand == "Hello World!") {
        Serial.println(" Replying \"Hello\" to iOS App to confirm comms functionality.");
        Serial1.write("M-O says Hello");        // send text back to M-O BLE app on iPhone
      } else if (newCommand == "M-O") {
        Serial1.write("M-O says Mo");
        sendToI2CSlave("play-4", 1);
      } else if (newCommand == "Yip") {
        Serial1.write("M-O says Yip");
        sendToI2CSlave("play-6", 1);
      } else if (newCommand == "speak") {
        sendToI2CSlave("speak", 1);
      } else if (newCommand == "sleep") {
        toggleAwakeState();                    // TODO - does this work, just toggling, or can I reach a state where sleep on app is triggering wake here?
      } else if (newCommand == "wake") {
        toggleAwakeState();
      } else if (newCommand == "iOSOK") {
        Serial.println(" iOS app has connected via Bluetooth");
        Serial.println((String)" -? Sending isAwake state and \"M-O Connected\" as confirmation back to iOS app");
        Serial1.write(isAwake ? "awake:1" : "awake:0");                       // sending isAwake state… first?  
            delay(50);                                                        // don't really know if this is neccesary, but felt safe
        Serial1.write("M-O Connected");                                       // confirmation to iOS app
            delay(1000);
        Serial1.write("ready:1");                                             // makes iOS app read "Microbe Obliterator Ready"

        // TODO - noticed that I was originally using Serial1.write() to Bluetoth but have carelessly started using
        //          Serial1.print() …and it doesn't seem to make a functional difference.
      } else if (newCommand.startsWith("A1")) {
        newCommand.remove(0,3);                                               // removes first 3 characters
        servo1.commandTo(newCommand);
      }
      
    } 
   
  /* how about converting reply to string, 
   *   and if string is something
   *   then writeToBLE( a reply );
   */

  }
}
