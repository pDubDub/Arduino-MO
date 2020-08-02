/*
 *  Base functionality to run a mini-OLED screen, later to be replaced by full sized LED matrix
 *  
 *  The tiny OLED is 128 x 32.
 *  The full size on will be 64 x 32 (but only using the bottom 64 x 26). All dimensions here are x2.
 */

void updateEyes() {
  // will read String 'eyeState', perform that animation, and return eyeState to 'none'


  if (isAwake && eyeState == "none" && ((millis() - previousEyeMillis) > (nextRandomEyeMove * 1000))) {
    eyeState = "random";
  }
  
  if (eyeState != "none") {                     // This works!
    if (eyeState == "waking") {
      wakingEyes();
    } else if (eyeState == "sleeping") {
      sleepingEyes();
    } else if (eyeState == "blinking") {
      blinkingEyes();
    } else if (eyeState == "huh") {             // IR button 8 = track 11 = 'huh'
      huhReaction();
    } else if (eyeState == "random") {
      halfBlink();                              // this is a random-interval 'bored' reaction. would be nice if there were random reactions to pull from.
    } else if (eyeState == "skewed") {          // IR button 3 = track 6 = "yip"
      skewedPeek();
    }
    
    eyeState = "none";
    previousEyeMillis = millis();
    nextRandomEyeMove = (int)random(4,12);
  }
}

// ******** functions:
void drawEyes(struct Expression look, int x, int y) {
    // expression is an eye shape
    // x,y are offsets from center of eyes (y=-4 is actually center vertically)

    display.clearDisplay();                 // clears the buffer
    
    display.fillRoundRect(
        cent_x + x - look.eye_sep - look.eye_width/2,
        cent_y + y - (look.eye_height / 2),
        look.eye_width,
        look.eye_height,
        2,                                  // corner radius
        SSD1306_WHITE);
    display.fillRoundRect(
        cent_x + x + look.eye_sep - look.eye_width/2,
        cent_y + y - (look.eye_height / 2),
        look.eye_width,
        look.eye_height,
        2,
        SSD1306_WHITE);
    display.display();
    
    delay(60);
}

// TODO - might want to add a height/thickness value?
void drawLineWithOffset(int y) {
  display.clearDisplay();
  display.fillRect(0, cent_y + y, display.width(), 4, SSD1306_WHITE);
  display.display();

  delay(60);
}

// retained old function, for times when you want to manually code a single frame expression
void drawEyes(int width, int height, int x, int y, int separation) {
  display.clearDisplay();
  // 0,0 is top left
  // x, y, w, h, radius, color  
  display.fillRoundRect(cent_x + x - separation - width/2, cent_y + y - (height / 2), width, height, 2, SSD1306_WHITE);
  display.fillRoundRect(cent_x + x + separation - width/2, cent_y + y - (height / 2), width, height, 2, SSD1306_WHITE);
  display.display();
}


// for drawing two different eyes shapes. Note L & R are from viewer's perspective, not MO's.
void drawSkewedEyes(int widthL, int heightL, int widthR, int heightR, int x, int y1, int y2, int separation) {
  display.clearDisplay();
  display.fillRoundRect(cent_x + x - separation - widthL/2, cent_y + y1 - (heightL / 2), widthL, heightL, 2, SSD1306_WHITE);
  display.fillRoundRect(cent_x + x + separation - widthR/2, cent_y + y2 - (heightR / 2), widthR, heightR, 2, SSD1306_WHITE);
  display.display();
}


// ********** eye animation sequences:
void blankScreen() {
  display.clearDisplay();
  display.display();
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
  drawEyes(wowEyes, 0, -4);
  delay(800);
  drawEyes(56,4,0,-4,32);
  blankScreen();
  delay(100);
  drawEyes(baseEyes, 0, 0);
}

void skewedPeek() {
  drawEyes(sleepyEyes, 0, 2);
  delay(60);
  drawLineWithOffset(0);
  drawSkewedEyes(26, 26, 26, 8, 0, 0, 4, 18);
  delay(500);
  drawEyes(wowEyes, 0, -4);
  drawEyes(baseEyes, 0, 0);
}

// TODO - might swap names blinking and squinting

// TODO - might be funny, based on how long he's been asleep, to randomly trigger peak
//    where he opens eyes from sleep, glances left and right, and then closes his eyes again.

 