
// import statements
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
//#include "SoftwareSerial.h"                 // when we switch from Uno to Mega, then this might switch to Serial1
#include <Wire.h>                           // built in Arduino library for I2C

// OLED code from Adafruit, store in my workingOLED128x32_i2c sketch
//#include <SPI.h>
#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>

// for Adafruit 3mmpitch 64x32 LED matrix panel
#include <RGBmatrixPanel.h>

#define OLED_WIDTH 128 // OLED display width, in pixels
#define OLED_HEIGHT 32 // OLED display height, in pixels

#define LED_WIDTH 64
#define LED_HEIGHT 32

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
//Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

//_____________________________________________
// declare constants for pin numbers

// 9 through 11 will be reserved for Mega to LED screen
// as well as AO through A4

//const int MP3_RX = 5;
//const int MP3_TX = 6;
const int LED_OE = 9;
const int LED_LAT = 10;
const int LED_CLK = 11;

#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

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
  unsigned long previousEyeMillis = 0;
  unsigned long lastEyeCenterMillis = 0;

//  float dt = 0;
  int frame = 0;                                  // used to track frame in blink pattern       
  
  unsigned long nextSleepEmote = 0;     

//unsigned long timer = 0;                    // was used to play audio in 3 second intervals.
                                            // don't believe it's used anymore
                                            
String message;                             // for received I2C message

  // eyes:
  int cent_x, cent_y;
  //  eye_width, eye_height, eye_sep;

  int random_glance = 0;
  int look_direction = 0;

  // my eye expression struct
  struct Expression {
    int eye_width;
    int eye_height;
    int eye_sep;                                  // note: separation is distance from screen center to eye center, not eye-to-eye
  };
//  typedef struct expression Expression;

  // TODO - remeasure these eyes in Photoshop, with proper understanding of sep value
  // declaring of eye expressions/frames
  Expression sleepyEyes = {7, 1, 6};        // width, height, sep
  Expression baseEyes = {13, 5, 11};
  Expression narrowedBaseEyes = {13, 5, 10};
  Expression bigEyes = {9, 9, 11};
  Expression squintyEyes = {15, 3, 11};             // for random 'half-blink'
  Expression wowEyes = {15, 16, 12};

  int nextRandomEyeMove = 2;

  // TODO - might develop a list of all the sound files, i.e.
  int startUpChime = 1;
  int chargedStartUp = 2;
  int sleepSnore = 3; 
  int moName = 4;
  int yips6 = 5;
  int yipPeek = 6;
  int whatIsThat = 7;
  int yips2 = 8;
  int thereDone = 9;
  int notAgain = 10;
  int huh = 11;
  int whoa6 = 12;
  
  

//_____________________________________________
// declare objects
//SoftwareSerial mySoftwareSerial(MP3_RX, MP3_TX);    // RX, TX
                                            // when we switch from Uno to Mega, then this might switch to Serial1
DFRobotDFPlayerMini myDFPlayer;



//_____________________ SETUP ( runs once )________________________
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
//  mySoftwareSerial.begin(9600);                           // for audio player                 *3
  Serial1.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);                           // for internal LED blink           *2

  Wire.begin(I2C_ADDRESS);                                    // for I2C communication on SDA and SCL pins
  Wire.onReceive(receiveEvent);

  // setup code for temporary mini-OLED:
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
//  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {      // Address 0x3C for 128x32
//    Serial.println(F("SSD1306 allocation failed"));
//    for(;;);                                            // Don't proceed, loop forever
//  }
//  display.clearDisplay();
//  display.display();
  
//  cent_x = display.width()/2;                 // 64
//  cent_y = display.height()/2 + 4;           // 16 + 5 = 20
  // all int values for eyes are x2, as OLED is 128x32, but fullsize will be 64x32(26).
  //      i.e. base eyes on full size will be 13 x 5 with sep 24
  //                  vs OLED eyes as 26,10,22

  // this is the center of the screen. The center of his direction of vision. If we adjust this value, he looks left/right, up/down.
  cent_x = 32;
  cent_y = 20;

  // the actual 64x32 LED matrix
  matrix.begin();
  // fill the screen with 'black'
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(Serial1)) {  //Use softwareSerial to communicate with mp3.
      // "mySoftwareSerial" replaced with Serial1 when moving from Uno to Mega.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(20);          //Set volume value. From 0 to 30
                                  // I found anecdotes stating the .volume() method in the loop can cause up to 100ms delay.
                                  // https://reprage.com/post/dfplayer-mini-cheat-sheet
  myDFPlayer.play(startUpChime);             //Play the first mp3. This tells me Mega-2 has started up.

                                  // when Mega-1 is ready, it will tell Mega-2 to play Wall-e startup chime.

        // whenever you want to play an mp3, just call     myDFPlayer.play(track#)  track 3 is "MO"
        // should probably be in a millis() statement so that previous play has time to finish

//  display.clearDisplay();
//  display.display();

  Serial.println("\n    MO-2 SETUP ROUTINE COMPLETE\n");

  // TODO - if Mo-2 is restarted after Mo-1 is running, Mo-2 will not know true state of isReady and isAwake.
  //    We really need to get that information somehow, sometime.
  
} // end SETUP


//____________________ MAIN LOOP ( runs continuously )_________________________
void loop() {

    // f1
    blinkRunningLED();

    // f5
    updateEyes();

    if (isReady && !isAwake) {
      // sleeping routine in f2
      runSleepingBehaviors();
    }

    // building a feature to make eye center vary
    currentMillis = millis();
    if (currentMillis - lastEyeCenterMillis >= 1000) {
//      Serial.println("Time to check glance");
      lastEyeCenterMillis = currentMillis;

      random_glance = (int)(random(1,10));
//      Serial.print("Random glance = ");
//      Serial.println(random_glance);
    }


    /*
     * Re-read about State Machine. 
     *    Do we have a didStateChange bool that gets changed by I2C?
     *    Then we make the changes and set didStateChange back to false?
     */
    
    // TODO - I think here, we will have a switch statement, based on an int or char that represents state (mood)
    //      ... and then calls the appropriate 'behavior' function, like sleepingRoutine() or wakingUp().
    //  These behaviors might themselves finish by changing the state, resulting in followup behaviors.

} // end main LOOP
