
/*
 * Function to update the blinking LED that indicates processor is running
 */

void blinkRunningLED() {
  // check to see if it's time to blink the LED; 
          // meaning, is the difference between the current time and last time you blinked the LED
          // bigger than the interval at which you want to blink the LED.
  // TODO - experiment with putting this in an external class, so perhaps all 3 Mega's can reuse same code.
  // TODO -  will likely duplicate this on an external LED on back panel on finished bot **
          
  currentMillis = millis();

  if (currentMillis - previousBlinkLEDMillis >= BLINK_INTERVAL) {       // interval declared above (100 millis or .1 second)
    previousBlinkLEDMillis = currentMillis;                             // save the last time you blinked the LED
    
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

    //  if at end of animation loop, then reset to beginning (condensed from IF-ELSE to TERNARY
    (frame >= 12) ? frame = 0 : frame += 1;
  } // end of timing IF 
} // end of blinkRunningLED()
