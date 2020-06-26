
// import statements
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include "SoftwareSerial.h"                 // when we switch from Uno to Mega, then this might switch to Serial1
#include <Wire.h>                           // built in Arduino library for I2C

// OLED code from Adafruit, store in my workingOLED128x32_i2c sketch
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//_____________________________________________
// declare constants for pin numbers

//_____________________________________________
// declare other constants
const int I2C_ADDRESS = 1;                      // I2C address. Master would be 0, slaves would be 1 and 2
const long BLINK_INTERVAL = 100;                // interval at which to blink (milliseconds)
                                                // making this smaller, makes blink pattern faster

//_____________________________________________
// declare variables
  bool isReady = false;
  bool isAwake = false;                         // defines whether MO is sleeping or ready for commands

  bool stateDidChange = false;
  String eyeState = "none";
  
  unsigned long currentMillis = 0;                // these 4 varaibles used for runningLED
  unsigned long previousBlinkLEDMillis = 0;       // will store last time LED was updated
//  float dt = 0;
  int frame = 0;                                  // used to track frame in blink pattern            

//unsigned long timer = 0;                    // was used to play audio in 3 second intervals.
                                            // don't believe it's used anymore
                                            
String message;                             // for received I2C message


   int16_t cent_x, cent_y, eye_width, eye_height, eye_sep;

//_____________________________________________
// declare objects
SoftwareSerial mySoftwareSerial(10, 11);    // RX, TX
                                            // when we switch from Uno to Mega, then this might switch to Serial1
DFRobotDFPlayerMini myDFPlayer;

//_____________________ SETUP ( runs once )________________________
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySoftwareSerial.begin(9600);                           // for audio player                 *3

  pinMode(LED_BUILTIN, OUTPUT);                           // for internal LED blink           *2

  Wire.begin(I2C_ADDRESS);                                    // for I2C communication on SDA and SCL pins
  Wire.onReceive(receiveEvent);

  // setup code for temporary mini-OLED:
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {      // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);                                            // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();

  cent_x = display.width()/2;                 // 64
  cent_y = display.height()/2 + 5;           // 16

  eye_width = 15;
  eye_height = 20;
  eye_sep = 8;

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(20);          //Set volume value. From 0 to 30
                                  // I found anecdotes stating the .volume() method in the loop can cause up to 100ms delay.
                                  // https://reprage.com/post/dfplayer-mini-cheat-sheet
  myDFPlayer.play(1);             //Play the first mp3. This tells me Mega-2 has started up.

                                  // when Mega-1 is ready, it will tell Mega-2 to play Wall-e startup chime.

        // whenever you want to play an mp3, just call     myDFPlayer.play(track#)  track 3 is "MO"
        // should probably be in a millis() statement so that previous play has time to finish

//  display.clearDisplay();
//  display.display();
} // end SETUP


//____________________ MAIN LOOP ( runs continuously )_________________________
void loop() {

    // f1
    blinkRunningLED();

    // f2
    updateEyes();

    /* pseudocode for random snorePurr:
     *  if !isAwake {
     *      has it been more than 60 * 1000 millis since previousSnoreMillis
     *          if so, call a random # to decide, should we play track-2
    */

    /*
     * Re-read about State Machine. 
     *    Do we have a didStateChange bool that gets changed by I2C?
     *    Then we make the changes and set didStateChange back to false?
     */
    
    // TODO - I think here, we will have a switch statement, based on an int or char that represents state (mood)
    //      ... and then calls the appropriate 'behavior' function, like sleepingRoutine() or wakingUp().
    //  These behaviors might themselves finish by changing the state, resulting in followup behaviors.

} // end main LOOP

void awakeStateTo(bool state) {
  isAwake = state;
  stateDidChange = true;
//  updateEyes();
  if (isAwake) {
    Serial.println("wake eyes");
    eyeState = "waking";
  } else {
    Serial.println("sleep eyes");
//    sleepEyes();
    eyeState = "sleeping";
  }
}
