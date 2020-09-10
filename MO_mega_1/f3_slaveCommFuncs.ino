
/* Here will go functions for
 *  
 *    communicating commands to slave Arduinos over I2C
 *    
 */

// 3 - sending I2C commands to other Ardiunos (slaves)
void sendToI2CSlave(String command, int slave) {
    // slave 1 = MO-2, which handles sound and eyes
    // slave 2 = MO-3, which runs the BBR
    
    Wire.beginTransmission(slave);   // transmit to device number 
    command.toCharArray(message, 8);
    Wire.write(message);     // send data to other Arduinos
    Wire.endTransmission();  // stop transmitting
    Serial.println("    Sent message \"" + command + "\" to slave " + slave + " over I2C.");
    /*
     *  Condensed 4 print lines to 1. (String) didn't seem required. Removed it, and still  works.
     *     Serial.println((String)"Sent I2C Message \"" + command + "\" to slave #" + slave);
     */

    // TODO - this needs some error logic
    // If receiver is not available, the program gets stuck.
    /*
     * 
     * https://arduino.stackexchange.com/questions/30353/i2c-connection-freezes-master-if-slave-disconnects
     * 
     * 
    */
}

void sendMessageToAllListeners(String message) {
  // this function will send a String to all places at once

  Serial.println((String)"\n -> Sending message \"" + message + "\" to all I2C and BTLE listeners:");
  sendToI2CSlave(message, 1);

//  Serial.println((String)"\n -> Sending message \"" + message + "\" to Mega-3 (slave 2) over I2C");
  sendToI2CSlave(message, 2);

//  Serial.println((String)"\n -> Sending message \"" + message + "\" to iOS app over BT");
  Serial1.print(message);
}
