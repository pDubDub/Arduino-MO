
/* Here will go functions for
 *  
 *    communicating commands to slave Arduinos over I2C
 *    
 */

// 3 - sending I2C commands to other Ardiunos (slaves)
void sendToI2CSlave(String command, int slave) {
    Wire.beginTransmission(slave);   // transmit to device 1
    command.toCharArray(message, 8);
    Wire.write(message);     // send data to other Arduinos
    Wire.endTransmission();  // stop transmitting
    Serial.println(" Sent I2C Message \"" + command + "\" to slave #" + slave);
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
