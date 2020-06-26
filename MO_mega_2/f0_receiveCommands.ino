
// could actually design this so slaves can message master too. but is there reason to?

void receiveEvent(int bytes) {                        // called when message received on I2C
  message = "";                                         // clear old message
  while(Wire.available()) { 
      message += (char)Wire.read();                     // read chars from Wire and concat to String
  }
  Serial.println("I2C Message received: " + message);

//  if(message == "SYNC") {
//    frame = 0;
//  }
// BUG - this IF was preventing sound files from playing.
// ERROR FOUND -> accidentally used a single = instead of a double ==
// TODO - try this again
  
  if (message.startsWith("play") && isAwake) {            
    // decode messages from MO-1 in the form of "play-1" - but only when awake
    message.remove(0,5);                        // remove "play-"
    int trackInt = message.toInt();             // convert to int
    myDFPlayer.play(trackInt);
    
    eyeState = "blinking";
    if (trackInt == 11) {
        eyeState = "huh";
    }
    // TODO - could make a switch statement here so each sound is matched with its own eye reaction
    
  } else if (message.startsWith("awake")) {   // this will be the new message format
    message.remove(0,6);                        // removes the beginning of message, leaving just a code
    if (message == "0") { 
      // isAwake false
      awakeStateTo(false);
      // Serial.println(isReady ? "ready" : "not ready");
      // TODO - problem: if Mega-2 re-starts after Mega-1 is running, it never hears "ready:1"
      //      so, we may need Mega-2 to query a state from Mega-1 upon startup.
      //        Alternately, Mega-1 could broadcast state every minute (or so) to make sure everyone is on the same page.
      //      Method 1 might result in less traffic though. Upon Mega-1 hearing "query" it would broadcast a set of states.
//      isAwake = false;
//      stateDidChange = true;

      // TODO - I think the point of the state machine is to call a wake.toggle() method, which will flip isAwake and stateDidChange
      
      if (isReady) {                                            // to only snore if isReady operational
        Serial.println("I2C message says MO is asleep");
//        isAwake = false;
        myDFPlayer.play(3);                     // play the snore  track 3
        // TODO - eventually spin this off into a whole "sleeping" behavior funcion, which includes random time interval triggered snore sound.
      }
    } else if (message == "1") {
      // isAwake true
      awakeStateTo(true);
      Serial.println("I2C message says MO is awake");
//      isAwake = true;
//      stateDidChange = true;
//      wakeUpEyes();
      if (isReady) {
          myDFPlayer.play((int)random(5,12));      // play a random yip track 5 to 12 to say he's awake
          // TODO - eventually spin this off, into a whole "waking up" behavior function
      }
    }
  } else if (message == "speak") {
    myDFPlayer.play((int)random(5,13));
//    blinkEyes();

  } else if (message == "ready:1") {
    isReady = true;
    myDFPlayer.play(2);                       // play startup chime
    // TODO - these statement should enact local state changes too
  } else if (message == "ready:0") {
    isReady = false;
  }
  // TODO - implement listening for sirenState message
  // TODO - this section needs cleaning up
}
