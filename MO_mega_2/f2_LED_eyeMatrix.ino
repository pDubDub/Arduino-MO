
/*
 *  Base functionality to run a mini-OLED screen, later to be replaced by full sized LED matrix
 *  
 *  The tiny OLED is 128 x 32.
 *  The full size on will be 64 x 32 (but only using the bottom 64 x 16). All dimensions here are x2.
 */


// TODO - Do I even need this updateEyes() function? Do I instead just call functions like wakingEyes() when a state changes?

void updateEyes() {
  /* read the state
   *    if state has changed, then clearDisplay and draw new
  */

  // this 'stateDidChange' bool might no longer be used/needed.
//  if (stateDidChange) {
////    Serial.println("state change");
//    if (isAwake) {
//      // woke up
//      wakingEyes();
//    } else {
//      sleepingEyes();
//    }
//    stateDidChange = false;
//  }

  if (eyeState != "none") {                     // This works!
    if (eyeState == "waking") {
      wakingEyes();
    } else if (eyeState == "sleeping") {
      sleepingEyes();
    } else if (eyeState == "blinking") {
      blinkingEyes();
    }
    eyeState = "none";
  }

}

// *********** sequences:

void sleepingEyes() {
  showSquintyEyes();
  delay(60);
  
//    Serial.println("sleep eyes clear");
  display.clearDisplay();
  display.display();
}

void wakingEyes() {
  //squint
  showSleepyEyes();
  delay(60);

  //line
  drawLineAt(cent_y);
  delay(60);

  //squares
  showBigEyes();
  delay(60);

  // intermediate eye frame when they're slightly closer together
  drawEyes(10, 26, cent_x, cent_y, 20);
  delay(60);
  
  //eyes
  showBaseEyes();
}

void blinkingEyes() {                               // currently used as generic reaction to play commands
  //line
  drawLineAt(cent_y);
  delay(40);

  //eyes
  showBaseEyes();
}

// TODO - build squinting, that just shifts between  base - to squinty - and back to base.
//    create a last reaction millis, and if nothing changes in a while, randomly squint
//    (might swap names blinking and squinting)

// TODO - make a BIG eye reaction that goes along with 'huh' sound

// TODO - might be funny, based on how long he's been asleep, to randomly trigger peak
//    where he opens eyes from sleep, glances left and right, and then closes his eyes again.

// ********** expressions/frames:

void showBaseEyes() {
  eye_width = 26;
  eye_height = 10;
  eye_sep = 22;
    // TODO - maybe each expression is a struct (or equivalent) and we just pass an expession to draw eyes

  drawEyes(eye_height, eye_width, cent_x, cent_y, eye_sep);
}

void showBigEyes() {
  eye_width = 18;
  eye_height = 18;
  eye_sep = 22;

  drawEyes(eye_height, eye_width, cent_x, cent_y, eye_sep);
}

void showSquintyEyes() {                // his eyes randomly 'half blink'
  eye_width = 30;
  eye_height = 6;
  eye_sep = 22;

  drawEyes(eye_height, eye_width, cent_x, cent_y, eye_sep);
}

void showSleepyEyes() {
  eye_width = 14;
  eye_height = 2;
  eye_sep = 12;

  drawEyes(eye_height, eye_width, cent_x, cent_y, eye_sep);
}

// ************

void drawEyes(int height, int width, int cent_x, int cent_y, int seperation) {
  display.clearDisplay();
  // 0,0 is top left
  // x, y, w, h, radius, color  
  display.fillRoundRect(cent_x - seperation - width/2, cent_y - (height / 2), width, height, 3, SSD1306_WHITE);
  display.fillRoundRect(cent_x + seperation - width/2, cent_y - (height / 2), width, height, 3, SSD1306_WHITE);
  display.display();
}

void drawLineAt(int y) {
  display.clearDisplay();
  display.fillRect(0, y-2, display.width(), 4, SSD1306_WHITE);
  display.display();
}
 
