
// could actually design this so slaves can message master too. but is there reason to?

void receiveEvent(int bytes) {                        // called when message received on I2C
  message = "";                                         // clear old message
  while(Wire.available()) { 
      message += (char)Wire.read();                     // read chars from Wire and concat to String
  }
//  Serial.println("I2C Message received: " + message);

//  if(message == "SYNC") {
//    frame = 0;
//  }
// BUG - this IF was preventing sound files from playing.
// ERROR FOUND -> accidentally used a single = instead of a double ==
// TODO - try this again

  // receiving code copied from Mega-2
  if (message.startsWith("awake")) {                  // checks for either 'awake:0' or 'awake:1'
    message.remove(0,6);                              // removes the beginning of message, leaving just a code
    if (message == "0") {                             // message was 'awake:0' 

      // TODO - problem: if Mega-3 re-starts after Mega-1 is running, it never hears "ready:1"
      //      so, we may need Mega-3 to query a state from Mega-1 upon startup.
      //        Alternately, Mega-1 could broadcast state every minute (or so) to make sure everyone is on the same page.
      //      Method 1 might result in less traffic though. Upon Mega-1 hearing "query" it would broadcast a set of states.
      
//        Serial.println("I2C message says MO is asleep");
        isAwake = false;

    } else if (message == "1") {     // message was 'awake:1'

//      Serial.println("I2C message says MO is awake");
      isAwake = true;
      
      // I mean, in theory, Mega-3 should not do anything unless awake:1 AND ready:1
      //  Perhaps if it gets a wake command, but Mega-3.isReady = false, it can query for confirmation of actual wake state?
      
    }
  } else if (message == "ready:1") {
    isReady = true;
  } else if (message == "ready:0") {
    isReady = false;
  }

}
