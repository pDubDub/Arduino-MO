
/* Here will go functions for
 *  
 *    LED and LCD output
 *    
 */

// *4A --------------------------- blink pattern on internal and external LED ---------------------------
void blinkRunningLED() {
  // check to see if it's time to blink the LED; 
          // meaning, is the difference between the current time and last time you blinked the LED
          // bigger than the interval at which you want to blink the LED.
  // TODO - experiment with putting this in an external class, so perhaps all 3 Mega's can reuse same code.
  // TODO -  will likely duplicate this on an external LED on back panel on finished bot **
          
  currentMillis = millis();
  if (currentMillis - previousBlinkLEDMillis >= BLINK_INTERVAL) {       // interval declared above (100 millis or .1 second)
    previousBlinkLEDMillis = currentMillis;                       // save the last time you blinked the LED
    
    switch(frame) {
      case 0:
        digitalWrite(LED_BUILTIN, HIGH); break;
      case 1:
        digitalWrite(LED_BUILTIN, LOW); break;
      case 2:
        digitalWrite(LED_BUILTIN, HIGH); break;
      case 3:
        digitalWrite(LED_BUILTIN, LOW); break;
      case 7:
        digitalWrite(LED_BUILTIN, HIGH); break;
      case 8:
        digitalWrite(LED_BUILTIN, LOW); break;
    } // end SWITCH

    // TODO - I think it would be nice if every once in a while, Mega-1 sends i2C message to Mega-2 and 3 
    // to sync this cycle. But not this often.
    
//    if (frame == 12) {
//        sendToI2CSlave("SYNC",1);
//    }

    // took this out. For some reason after implementing, sound commands no longer worked.

    //  if at end of animation loop, then reset to beginning (condensed from IF-ELSE to TERNARY
    (frame >= 12) ? frame = 0 : frame += 1;
  } // end of timing IF 
} // end of blinkRunningLED()


// *4B --------------------------- causes the RGB LED to pulse green only if bot is in sleep mode ---------------------------
void updatePulsingLED() {
  //    ** RGB LED pulses Green when Asleep
  //              pulses RED when internal temp is high
  //              FUTURE - pulse BLUE when battery is low
  
  //  using sine wave LED tutorial https://www.sparkfun.com/tutorials/329

  // TODO - I'm not sure. The pulsing red light for power or temp might actually need to go on even if isAwake, no?

  // TODO - this if else is running every loop. But should only run on changes.
  if (!sleepStrobeIsOn && !isAwake) {             // if isAwake bool false, then he's asleep. turn on sleep strobe.
    sleepStrobeIsOn = true;
    wave = 4.712;                                 // the bottom of the wave   
    previousPulseUpdateMillis = millis();
    Serial.println(" \n ------------- strobe on ----");

    // TODO - This condition never actually triggers, as updatePulsingLED() is not called until sleepStrobeIsOn == true.
    //    So it might deserve deletion.

    
  } else if (isAwake && pulseBrightnessValue < 1) {    // this should stop strobe only at bottom
    sleepStrobeIsOn = false;
  }

  // changing pulse color to red if tempF above 90
  //  **  LATER we can use this to indicate low system battery or batteries **
  desiredColor = mpu6050Temperature > 90 ? red : green;           // This works. 
  // For some reason, using pins 9 or 10 would freeze sketch, so RGB now on 7, 8 & 11

  if ((pulseBrightnessValue < 1 ) && (pulseColor != desiredColor)) {    // should change pulse color only at bottom
    pulseColor = desiredColor;
    // Serial.print("color changed");
  }

  if (sleepStrobeIsOn) {      // strobe runs green LED while M-O is ON but sleeping
    // set brightness      
    pulseBrightnessValue = sin(wave) * (pulseMaxBrightness/2) + (pulseMaxBrightness/2);   // replaced 127.5 with variable to define amplitude of wave
//    Serial.println(wave);
    if(pulseColor == red) {
      analogWrite(RGB_LED_RED, pulseBrightnessValue);
    } else {
      analogWrite(RGB_LED_GREEN, pulseBrightnessValue);
    }
  
    // this is the old green-only pulse command
    //analogWrite(GREEN, pulseBrightnessValue);
    
    /* Move us along the sine wave for next cycle:
    *  PULSE_WAVELENGTH = 6.283
    *  PULSE_PERIOD = 1.5
    *  dt (milliseconds)
    *  increment = (PULSE_WAVELENGTH/PULSE_PERIOD) * (dt/1000)
    */
    //        wave = wave + ((PULSE_WAVELENGTH/PULSE_PERIOD) * (dt/180000));
    
    // not sure why this is 180,000 instead of 1,000, but that's what I needed to time it out correctly
    // ** UPDATE - since WAVELENGTH AND PERIOD are constants, we should pass this in as a value, 
    //              instead of doing the division every time through the loop
    //    wave = wave + (dt/90000);   // <-- streamlined.
    
    /*
     *      Replaced the version that used dt
     *                        wave = wave + (dt/90000);   // <-- streamlined.
     *      
     *      With (currentMillis - previousPulseUpdateMillis)
     *                              wave = wave + ((dt / (period *1000)) * wavelength)
     *                              
     *      This ends up working smoothly, and now the period value seems to be faithfully reproduced.           
     */
     
    // this fixes a startup condition where pulse does not ease in, because (currentMillis - previousPulseUpdateMillis) is too great
    if ( previousPulseUpdateMillis == 0 ) {                         
      previousPulseUpdateMillis = millis();
    }
    
    currentMillis = millis();
    wave = wave + (((currentMillis - previousPulseUpdateMillis) / (2.0 * 1000)) * 6.283) ;
    previousPulseUpdateMillis = currentMillis;

    // TODO - for further efficiency, could still pre-calculate the literal value of 6.283/(2.0 * 1000) to elimate math calculations each loop

    // TODO - if dt is no longer used anywhere, could be removed from sketch
    // TODO - can also remove the constants for PULSE_WAVELENGTH and PULSE_PERIOD

    // for reference: the constant declarations from above.
    //float PULSE_WAVELENGTH = 6.283;       // sine wave cycle (6.283 radians is pi*2, or one full cycle)
    //float PULSE_PERIOD = 1.50;            // 1.5 seconds is chosen speed
    //      Apr 30 2020, I replaced PULSE_PERIOD with "3" and I think the speed is much better  
    
    if (wave >= 6.283) { wave = 0; }                        // when we reach the end of the wave (6.283 is pi*2, or one full cycle), reset to beginning
      
  } // end of sleepStroneIsOn IF 

  // incidentally, it appear that the current dt for a loop cycle is about 40 millis, or 0.040 seconds (when asleep)
} // (end 4B)


// 4C --------------------------- rear panel operation message LED screen ---------------------------
void updateRearLEDscreen() {
  // currently copying over myThermometer sketch
  // with added milli time check

  if (currentMillis - previousLCDMillis >= 1000) {       // 1000 millis = 1 second)
    /* Run through this loop once every second.
     * Allows us to update the top line every couple seconds, but still loop through every sec for time display. 
     * 
     * Alternating mesasges on top line, and fixed IMU data on second? (For now)
     */
     
    previousLCDMillis = currentMillis;                          // save the last time you updated the LED
  
    lcd.setCursor(0, 0);
    switch(updateLCDframe) {                                    // all messages 16 char to clear blank spaces
      case -6:                                                  // startup message only runs at startup
        lcd.print("    Microbe     "); lcd.setCursor(0,1);
        lcd.print("  Obliterator   "); break;
      case -3:
        lcd.print("    Ready to    "); lcd.setCursor(0,1);
        lcd.print("     Clean      "); break;
      case 0: 
        lcd.print("M-O Bot: Online ");lcd.setCursor(0,1);
        lcd.print("                "); 
        break;                                            
      case 3: 
        lcd.print("Status: "); lcd.print(isAwake ? "Awake   " : "Sleeping"); break;
           // TODO - this line appears twice: once here and also in awakeStateChange 
      case 6:
      case 7:
      case 8:
        lcd.print("Runtime:        ");                          // left extra spaces here to clear Status messages

        // TIME:
        seconds = millis() / 1000; minutes = seconds/60; seconds = seconds % 60;
        
        lcd.setCursor(9,0); lcd.print(minutes); lcd.print(":");
        if(seconds < 10) { lcd.print("0"); }
        lcd.print(seconds);
         break;
      case 9:
        // this now pulls temp from mpu6050 instead of thermister
        dtostrf(mpu6050Temperature, 4, 1, buffer2);                   // data to String
        // DONE - pulsing strobe red color logic needs to pull from mpu6050Temperature now instead of thermister
        
         //( TEMP FUNCTION lies below, because it still updates constantly for blue strobe )

         // old format:
//         sprintf(buffer2,"Int-Temp: %3d\337F", mpu6050Temperature);
              // format string from https://forum.arduino.cc/index.php?topic=441616.0
              // "\337" is code for degree character
         lcd.print("Int-Temp: "); lcd.print(buffer2); lcd.print("\337F"); break;
       case 12:
         // TODO - INSERT BATTERY MESSAGE HERE - "Main Battery: V"
         lcd.print("Power Level: TBA");
         break;
    }
  
    if (updateLCDframe >= 14) { 
      updateLCDframe = 0;
    } else { 
      updateLCDframe += 1;
    }

    // added functionality to print IMU pitch/roll to lower line
    if (updateLCDframe >= 0) {
      lcd.setCursor(1, 1);
      lcd.print(pitch >= 0 ? "p: " : "p:");
      lcd.print(pitch); 
      lcd.setCursor(8, 1);
      lcd.print(roll >= 0 ? " r: " : " r:");
      lcd.print(roll);
    }

    
   //   TODO - add boolean outsideLCDupdate or cycleLCDnormally that let's outside calls pause this sequence for at least an interval when they update the top line

    // TEMP - inspired by tutorial https://www.hacktronics.com/Tutorials/arduino-thermistor-tutorial.html
//    tempReading = analogRead(A0);                 
//    tempK =  log(((10240000/tempReading) - 10000));
//    tempK = 1 / (0.001129148 + (0.000234125 * tempK) + (0.0000000876741 * tempK * tempK * tempK));       //  Temp Kelvin
//    tempF = ((tempK - 273.15) * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
//    tempDisp = tempF * 10;            // converts temp to Int for sprintf function
  }
} // end 4C
