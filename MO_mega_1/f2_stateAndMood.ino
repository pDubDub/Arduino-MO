
/* Here will go functions for
 *  
 *    state and mood control/logic
 *    
 */

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
