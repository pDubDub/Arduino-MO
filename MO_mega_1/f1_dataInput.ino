
/* Here will go functions for 
 *        • IR remote
 *        • Bluetooth communication
 *        • reading IMU 6050 gyroscope information
 * 
 */

 void sendMessageToAllListeners(String message) {
  // this function will send a String to all places at once

  Serial.println((String)" -> Sending message \"" + message + "\" to Mega-2 (slave 1) over I2C");
  sendToI2CSlave(message, 1);

  Serial.println((String)" -> Sending message \"" + message + "\" to Mega-3 (slave 2) over I2C");
  sendToI2CSlave(message, 2);

  Serial.println((String)" -> Sending message \"" + message + "\" to iOS app over BT");
  Serial1.print(message);
}

// TODO - build an i2C receiver, so that other Megas can send 'query' and Mega-1 will send out important states?
 //     Does iOS app need same? Or does it already have it below in BTListener?

// *1A - the IR Remote
void readFromIRRemote() {
  if (irrecv.decode(&results)) {                                // have we received an IR signal?
    translateIR(); 
    irrecv.resume(); // receive the next value
  }  
}

void translateIR() {                        
      // takes action based on IR code received
      // this function was taken from tutorial for IR Remote

  // TODO - IR signals should change a botState variable, and behaviors like LCD or siren servos should be handled elsewhere based on botState
      
  currentMillis = millis();                 // gets current time
  if (currentMillis - previousIRMillis >= 500) {         // only runs loop 0.5 seconds after last time, replacing old delay(500) statement
    previousIRMillis = currentMillis;                  
                                                          // could test at 0. using Serial.print
                                                          // might not need millis if statement
                                                          // just doing it because tutorial used delay(500)
    switch(results.value)
    // describing Remote IR codes
    {
      case 0xFFA25D:                                    // "POWER" button
        toggleAwakeState();
        break;
      case 0xFFE21D: Serial.println("FUNC/STOP"); break;
      case 0xFF629D: Serial.println("VOL+"); break;
      case 0xFF22DD: Serial.println("FAST BACK");    break;
      case 0xFF02FD: Serial.println("PAUSE");    break;
      case 0xFFC23D: Serial.println("FAST FORWARD");   break;
      case 0xFFE01F: Serial.println("DOWN");    break;
      case 0xFFA857: Serial.println("VOL-");    break;
      case 0xFF906F: Serial.println("UP");    break;
      case 0xFF9867: Serial.println("EQ");    break;
      case 0xFFB04F: Serial.println("ST/REPT");    break;
      case 0xFF6897:                                      // button "0" = siren toggle
        toggleSirenState();
        break;
      case 0xFF30CF: Serial.println("1");
        // Note: "1" plays track 2, skipping the Startup Chime which actually lives at track 1
        sendToI2CSlave("play-4", 1);   // send command to slave number 1
        break;
      case 0xFF18E7: Serial.println("2");
        sendToI2CSlave("play-5", 1);        // send command to slave number 1
        break;
      case 0xFF7A85: Serial.println("3");    
          sendToI2CSlave("play-6", 1);      // send command to slave number 1
          break;
      case 0xFF10EF: Serial.println("4");    
          sendToI2CSlave("play-7", 1);      // send command to slave number 1
          break;
      case 0xFF38C7: Serial.println("5");    
          sendToI2CSlave("play-8", 1);      // send command to slave number 1
          break;
      case 0xFF5AA5: Serial.println("6");    
          sendToI2CSlave("play-9", 1);      // send command to slave number 1
          break;
      case 0xFF42BD: Serial.println("7");    
            sendToI2CSlave("play-10", 1);   // send command to slave number 1
            break;
      case 0xFF4AB5: Serial.println("8");    
            sendToI2CSlave("play-11", 1);   // send command to slave number 1
            break;
      case 0xFF52AD: Serial.println("9");    
            sendToI2CSlave("play-12", 1);   // send command to slave number 1
            break;
      case 0xFFFFFFFF: Serial.println(" REPEAT");break;  
    
      default: 
        Serial.println(" other IT signal : ");
        Serial.println(results.value);
  
    }// End SWITCH statement
  }
} //END translateIR function
     
// 1B - Bluetooth (4 functions from tutorial)
// TODO - don't love the presence of delay()'s
void sendBTCommand(const char * command) {
  Serial.println((String)"Command \"" + command + "\" sent to iOS app over BT");
  Serial1.println(command);
  
  delay(100);                         //wait some time
  
  char reply[100];
  int i = 0;
  while (Serial1.available()) {
    reply[i] = Serial1.read();
    i += 1;
  }
  reply[i] = '\0';   //end the string
  Serial.println((String)"Message \"" + reply + "\" received from AT-09");
//  Serial.println(reply);
  // Serial.println("Reply: successful");

  delay(50);                          // wait some time
}

String readSerial(){
  char reply[50];
  int i = 0;
  while (Serial1.available()) {
    reply[i] = Serial1.read();
    i += 1;
  }
  reply[i] = '\0';   //end the string
  if(strlen(reply) > 0){
    Serial.print("readSerial() BT Receeived: ");
    Serial.println(reply);
    // Serial.println("Houston we have a signal!");
  }
  String command(reply);
  return command;
}

// this is from tutorial and doesn't appear to be used.
void writeSerialToBLE(int value) {
  Serial1.println(value);
}

// this is also from the tutorial, and may be superfluous, as below I was able to write to Serial1 with one command
void writeToBLE(char value) {
  Serial.print("Writing hex :");
  Serial.println(value, HEX);
  Serial1.write(value);
}

void readFromBluetooth() {
  currentMillis = millis();                               // gets current time
  if (currentMillis - previousBTMillis >= 500) {         // only runs loop 0.5 seconds after last time - no delay()
    previousBTMillis = currentMillis;
                 
    // reacting to various BT message from iOS app:
    newCommand = readSerial();                // reads message from BTE
    if(newCommand.length() > 1) {             // and if longer than 0 characters…
      Serial.println((String)" readFromBluetooth() New BT Command: " + newCommand);       // TODO - this line is redundant, as readSerial() also prints
//      Serial.println(newCommand);
      // lastCommand = newCommand;
      // lastCommand variable is for detecting change, so we would only scan switch if it's a new command
      if(newCommand == "Hello World!") {
        Serial.println(" Replying \"Hello\" to iOS App to confirm comms functionality.");
        Serial1.write("M-O says Hello");        // send text back to M-O BLE app on iPhone
      } else if (newCommand == "M-O") {
        Serial1.write("M-O says Mo");
        sendToI2CSlave("play-4", 1);
      } else if (newCommand == "Yip") {
        Serial1.write("M-O says Yip");
        sendToI2CSlave("play-6", 1);
      } else if (newCommand == "speak") {
        sendToI2CSlave("speak", 1);
      } else if (newCommand == "sleep") {
        toggleAwakeState();                    // TODO - does this work, just toggling, or can I reach a state where sleep on app is triggering wake here?
      } else if (newCommand == "wake") {
        toggleAwakeState();
      } else if (newCommand == "iOSOK") {
        Serial.println(" iOS app has connected via Bluetooth");
        Serial.println((String)" -? Sending isAwake state and \"M-O Connected\" as confirmation back to iOS app");
        Serial1.write(isAwake ? "awake:1" : "awake:0");                       // sending isAwake state… first?  
            delay(50);                                                        // don't really know if this is neccesary, but felt safe
        Serial1.write("M-O Connected");                                       // confirmation to iOS app
            delay(1000);
        Serial1.write("ready:1");                                             // makes iOS app read "Microbe Obliterator Ready"

        // TODO - noticed that I was originally using Serial1.write() to Bluetoth but have carelessly started using
        //          Serial1.print() …and it doesn't seem to make a functional difference.
        
      } else if (newCommand.startsWith("A1")) {                               // first implementation of iOS moving a servo
        newCommand.remove(0,3);                                               // removes first 3 characters
        servo1.commandTo(newCommand, 800);                                    // now accepts a duration in microseconds
      }
      
    } // end newcommand.length() IF
  } // end millis IF
}
