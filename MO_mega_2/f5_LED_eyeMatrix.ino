/*
 *  Base functionality to run a mini-OLED screen, later to be replaced by full sized LED matrix
 *  
 *  The tiny OLED is 128 x 32.
 *  The full size on will be 64 x 32 (but only using the bottom 64 x 26). All dimensions here are x2.
 *  
 *  3-bit matrix.Color333(5, 4, 0)) is roughly equivalent to 4-bit matrix.Color444(10, 8, 0))
 */

void updateEyes() {
  // will read String 'eyeState', perform that animation, and return eyeState to 'none'


  if (isAwake && eyeState == "none" && ((millis() - previousEyeMillis) > (nextRandomEyeMove * 1000))) {
    // this means he's awake, not doing an expression, and it's been a random delay since the last automatic expression
        // now we should generate another random number, to see if he move eye-center, re-centers, or half-blinks 
    eyeState = "random";
  }
  
  if (eyeState != "none") {                     // This works!
    if (eyeState == "waking") {
      wakingEyes();
    } else if (eyeState == "sleeping") {
      sleepingEyes();
    } else if (eyeState == "blinking") {
      blinkingEyes();
    } else if (eyeState == "skewed") {          // IR button 3 = track 6 = "yip"
      skewedPeek();
    } else if (eyeState == "huh") {             // IR button 8 = track 11 = 'huh'
      huhReaction();
    } else if (eyeState == "random") {
      randomFlinch();                              // this is a random-interval 'bored' reaction. would be nice if there were random reactions to pull from.
    }
    
    eyeState = "none";
    previousEyeMillis = millis();
    nextRandomEyeMove = (int)random(4,12);
  }

//  matrix.fillRect(18, 20, 10, 6, matrix.Color444(10, 8, 0));
//  matrix.fillRect(36, 20, 10, 6, matrix.Color444(10, 8, 0));

      // could also try matrix.ColorHSV(hue, sat, val, gammaCorrected)
}

void randomFlinch() {
  // this function called as random-interval 'bored' reaction
  //    making him either randomly half-blink, move or re-center the eye center, or other future reactions
  
  halfBlink();    // temp filler reaction, until we get the random/switch functioning

  /*
   * generate a random int
    switch(number)
    {
      case 0 ... 4:       // a range makes this reaction more common
        break;
      case 5:
      default:
    }
   */

   // cent_x is the value that we want to manipulate. 32 is center of the 64x32 matrix.
   // we want to make sure that we always return cent_x back to 32.
}

// ******** functions:
void drawEyes(struct Expression look, int x, int y) {
    // expression is an eye shape
    // x,y are offsets from center of eyes (y=-4 is actually center vertically)

    matrix.fillScreen(matrix.Color333(0, 0, 0));      // matrix does not have a .clearDisplay() method
    // on OLED, fillRouchRect() wiht corner radius of 2 looked really good
    // on LED matrix, I've overlayed a rounded rect over a dimmer 0 radius rect for a little aliasing
    matrix.fillRect(
        cent_x + x - look.eye_sep - look.eye_width/2,
        cent_y + y - (look.eye_height / 2),
        look.eye_width,
        look.eye_height,
        matrix.Color444(2, 2, 0)
        );
      matrix.fillRoundRect(
        cent_x + x - look.eye_sep - look.eye_width/2,
        cent_y + y - (look.eye_height / 2),
        look.eye_width,
        look.eye_height,
        1,                                  // corner radius
        matrix.Color444(10, 8, 0)
        );
      matrix.fillRect(
        cent_x + x + look.eye_sep - look.eye_width/2,
        cent_y + y - (look.eye_height / 2),
        look.eye_width,
        look.eye_height,
        matrix.Color444(2, 2, 0)
        );
      matrix.fillRoundRect(
        cent_x + x + look.eye_sep - look.eye_width/2,
        cent_y + y - (look.eye_height / 2),
        look.eye_width,
        look.eye_height,
        1,
        matrix.Color444(10, 8, 0)
        );
//
//    display.clearDisplay();                 // clears the buffer
//    display.fillRoundRect(
//        (cent_x + x - look.eye_sep - look.eye_width/2) * 2,
//        cent_y + y - (look.eye_height / 2),
//        look.eye_width * 2,
//        look.eye_height * 2,
//        2,                                  // corner radius
//        SSD1306_WHITE);
//    display.fillRoundRect(
//        cent_x + x + look.eye_sep - look.eye_width/2,
//        cent_y + y - (look.eye_height / 2),
//        look.eye_width,
//        look.eye_height,
//        2,
//        SSD1306_WHITE);
//    display.display();

    
    
    delay(60);
}

// TODO - might want to add a height/thickness value?
void drawLineWithOffset(int y) {
//  display.clearDisplay();
//  display.fillRect(0, (cent_y + y) * 2, display.width(), 4, SSD1306_WHITE);
//  display.display();

  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.fillRect(0, cent_y + y, matrix.width(), 2, matrix.Color444(10, 8, 0));
  
  delay(60);
}

// retained old function, for times when you want to manually code a single frame expression
void drawEyes(int width, int height, int x, int y, int separation) {
    // x is the center of eyes
//  display.clearDisplay();
  // 0,0 is top left
  // x, y, w, h, radius, color  
//  display.fillRoundRect(cent_x + x - separation - width/2, cent_y + y - (height / 2), width, height, 2, SSD1306_WHITE);
//  display.fillRoundRect(cent_x + x + separation - width/2, cent_y + y - (height / 2), width, height, 2, SSD1306_WHITE);
//  display.display();

  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.fillRoundRect(cent_x + x - separation - width/2, cent_y + y - (height / 2), width, height, 1, matrix.Color444(10, 8, 0));
  matrix.fillRoundRect(cent_x + x + separation - width/2, cent_y + y - (height / 2), width, height, 1, matrix.Color444(10, 8, 0));
  
}


// for drawing two different eyes shapes. Note L & R are from viewer's perspective, not MO's.
void drawSkewedEyes(int widthL, int heightL, int widthR, int heightR, int x, int y1, int y2, int separation) {
//  display.clearDisplay();
//  display.fillRoundRect(cent_x + x - separation - widthL/2, cent_y + y1 - (heightL / 2), widthL, heightL, 1, SSD1306_WHITE);
//  display.fillRoundRect(cent_x + x + separation - widthR/2, cent_y + y2 - (heightR / 2), widthR, heightR, 1, SSD1306_WHITE);
//  display.display();

  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.fillRoundRect(cent_x + x - separation - widthL/2, cent_y + y1 - (heightL / 2), widthL, heightL, 1, matrix.Color444(10, 8, 0));
  matrix.fillRoundRect(cent_x + x + separation - widthR/2, cent_y + y2 - (heightR / 2), widthR, heightR, 1, matrix.Color444(10, 8, 0));
}


// ********** eye animation sequences:
void blankScreen() {
//  display.clearDisplay();
//  display.display();

  matrix.fillScreen(matrix.Color333(0, 0, 0));
}

void sleepingEyes() {
    drawEyes(squintyEyes, 0, 0);
    
    blankScreen();
}

void wakingEyes() {
  // narrow squint
  drawEyes(sleepyEyes, 0, 2);

  // line
  drawLineWithOffset(0);

  // big
  drawEyes(bigEyes, 0, 0);

  // slightly close together
  drawEyes(narrowedBaseEyes, 0, 0);

  // normal
  drawEyes(baseEyes, 0, 0);
}

void blinkingEyes() {                               // currently used as generic reaction to play commands
  //line
  drawLineWithOffset(0);

  //eyes
  drawEyes(baseEyes, 0, 0);
}

void halfBlink() {                                  // aka flinch, or RandomEyeMove
  drawEyes(squintyEyes, 0, 0);
  drawEyes(baseEyes, 0, 0);
}

void huhReaction() {
  drawEyes(wowEyes, 0, 0);
  delay(800);
  drawEyes(23,4,0,-4,16);
  blankScreen();
  delay(100);
  drawEyes(baseEyes, 0, 0);
}

void skewedPeek() {
  drawEyes(sleepyEyes, 0, 2);
  delay(60);
  drawLineWithOffset(0);
  drawSkewedEyes(13, 13, 13, 4, 0, 0, 2, 8);
  delay(500);
  drawEyes(wowEyes, 0, -4);
  drawEyes(baseEyes, 0, 0);
}

// TODO - might swap names blinking and squinting

// TODO - might be funny, based on how long he's been asleep, to randomly trigger peak
//    where he opens eyes from sleep, glances left and right, and then closes his eyes again.

 
