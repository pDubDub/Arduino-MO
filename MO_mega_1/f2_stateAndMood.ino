
/* Here will go functions for
 *  
 *    state and mood control/logic
 *    
 */

 


void toggleAwakeState() {
  isAwake = !isAwake;

  Serial.print("\n **** M-O isAwake toggled to "); Serial.print(isAwake ? "Awake" : "Asleep"); Serial.println(" ****");
  String message2send = "";
  
  if (isAwake) { 
    message2send = "awake:1";
    // TODO - Why is there not a pulsingLEDisOn = false here?

    
    
      // tried making this goToCenter() but it did not work. ??
      // but this simulates an awake servo animation
//      moServo14Torso.commandTo(150);

      torsoStandsUp();

    /* there need to be more states... 
     *        compact and asleep
     *        compact and awake
     *        standing and awake
     */

    
  } else { 
    message2send = "awake:0";
    pulsingLEDisOn = true;
    wave = 4.712;                                       // these two statements added to assure pulsing starts from off
    previousPulseUpdateMillis = millis();

    // EXPERIMENT: can I recalibrate IMU when we go to sleep
    //      calculate_IMU_error();
    // In practice, we would likely want to wait until he had time to compact and settle before doing this, if it works at all.
    //   Possibly, before waking up too?
    // Done - Does recalibrating IMU on sleep have positive results? Not particularly.

    // TODO - I think I would rather have sleep trigger a 5 second pause, in which we take average pitch readings, in a pitchAdjust variable.
    //   Then in the IMU code, add the pitchAdjust (and rollAdjust) values to pitch (and roll).
    // Probably don't want to do this until after we develop some smoothing/filtering code to make readings more stable.

    torsoParks();
    
//    servosToStart();
    // this is a temporary funciton in f5 to center the servos.
    // Eventually, we would want more sophisticated animations to send everything back to home positions.
    
  }
  sendToI2CSlave(message2send, 1);
    // FUTURE - also send to slave 2
  sendToI2CSlave(message2send, 2);

  // TODO - Isn't it more efficent to use sendMessageToAllListeners() function?
  
  Serial2.println(message2send);                                    // also send to app over BT
  Serial.println("    Message \"" + message2send + "\" also sent to iOS app over BTLE.");

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
    lcd.print(sirenOn ? "Contaminant     " : "All Clean       ");
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


void animate(String emote) {
      // Because emotes can be triggered from IR, iOS or even other emotes, or even random reactions,
      //   each IF or CASE in IR or BT on page f1 should trigger a behavior that should live here in f2_stateAndMood
      
      // Accept a sting emote
      // Call servo movements
      // Send emote string on to slaves
      sendToI2CSlave("emo:1", 1);                           // example
      
      //    Which in turn read emote and play sounds and animate eyes
      // Then return emote state string back to "none"
      
}      
