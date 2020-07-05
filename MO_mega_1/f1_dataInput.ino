
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


// *1A --------------------------------- the IR Remote ---------------------------------
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
        sendToI2CSlave("play-4", 1); break;       // send command to slave number 1
      case 0xFF18E7: Serial.println("2");
        sendToI2CSlave("play-5", 1); break;       // send command to slave number 1
      case 0xFF7A85: Serial.println("3");    
          sendToI2CSlave("play-6", 1); break;      // send command to slave number 1
      case 0xFF10EF: Serial.println("4");    
          sendToI2CSlave("play-7", 1); break;     // send command to slave number 1
      case 0xFF38C7: Serial.println("5");    
          sendToI2CSlave("play-8", 1); break;      // send command to slave number 1
      case 0xFF5AA5: Serial.println("6");    
          sendToI2CSlave("play-9", 1); break;     // send command to slave number 1
      case 0xFF42BD: Serial.println("7");    
            sendToI2CSlave("play-10", 1); break;  // send command to slave number 1
      case 0xFF4AB5: Serial.println("8");    
            sendToI2CSlave("play-11", 1); break;  // send command to slave number 1
      case 0xFF52AD: Serial.println("9");    
            sendToI2CSlave("play-12", 1); break;  // send command to slave number 1
      case 0xFFFFFFFF: Serial.println(" REPEAT");break;  
    
      default: 
        Serial.println(" other IT signal : ");
        Serial.println(results.value);
  
    }// End SWITCH statement
  }
} //END translateIR function

     
// 1B --------------------------------- Bluetooth (4 functions from tutorial) ---------------------------------
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

// 1C --------------------------------- read data from IMU 6050 gyro ---------------------------------
void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (errorSamples < 200) {
    Wire.beginTransmission(mpu6050Address);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(mpu6050Address, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    errorSamples++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  errorSamples = 0;
  // Read gyro values 200 times
  while (errorSamples < 200) {
    Wire.beginTransmission(mpu6050Address);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(mpu6050Address, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    errorSamples++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  // Print the error values on the Serial Monitor
  Serial.println("*********************************************************************");
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
}

void read6050imu() {
    // === Read acceleromter data === //
  Wire.beginTransmission(mpu6050Address);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(mpu6050Address, 8, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value

  // pulled funcitonality from sketch "jun30_tempFrom6050
  //   changed 2nd parameter in .requestFrom() line above from 6 to 8 to get temp data
  mpu6050temperatureReading = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  mpu6050temperatureReading = (mpu6050temperatureReading / 340.00 + 36.53) * 9/5 +32 -3.4; // -3.4 is my manual calibration
//  Serial.print("temp deg F: "); Serial.print(mpu6050temperatureReading);
  
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorX; // AccErrorX ~(0.45) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorY; // AccErrorY ~(-2.78)
  
    // DONE - I feel like these Error values should be able to be programmed automatically, pulling results from the calculate_IMU_error() function called during setup.
    //    pw: It's better, but still drifts over time.
    //      TODO - what happens if we re-call calculate_IMU_error() every once in a while?
  
  // === Read gyroscope data === //
  previousTime6050 = currentTime6050;        // Previous time is stored before the actual time read
  currentTime6050 = millis();            // Current time actual time read
  elapsedTime6050 = (currentTime6050 - previousTime6050) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(mpu6050Address);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(mpu6050Address, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
//  Serial.print("GyroX: "); Serial.print(GyroX);
  // Correct the outputs with the calculated error values
  GyroX = GyroX - GyroErrorX; // GyroErrorX ~(-7.49)
  GyroY = GyroY - GyroErrorY; // GyroErrorY ~(3.2)
  GyroZ = GyroZ - GyroErrorZ; // GyroErrorZ ~ (1.12)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by seconds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime6050; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime6050;
  yaw =  yaw + GyroZ * elapsedTime6050;
  // Complementary filter - combine acceleromter and gyro angle values
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
      // pw: I tried adjusting this 0.04 number to 0.4 to see effect on drift. It didn't seem to improve much. Still drifting about 0.5 deg/min
      // need to fully read article:
      //  https://howtomechatronics.com/tutorials/arduino/arduino-and-mpu6050-accelerometer-and-gyroscope-tutorial/
  
  // Print the values on the serial monitor
//  Serial.print("  Corrected GyroX: "); Serial.print(GyroX);
//  Serial.print("  gyroAngleX: "); Serial.print(gyroAngleX);
//  
//  Serial.print(".   Pitch (x): ");
//  Serial.println(pitch);
  
//  Serial.print("    Roll (y): ");
//  Serial.println(roll);

  // pw July 4 2020 : mpu6050 code is currently working after being moved to MO-1. I have commented out the Serial.print lines here and at top of page (for temperature).

  // TODO - pitch and roll values slowly drift over time. Either our Error values are not right, or we're losing something in Precision.

  // TODO - there is a condition, perhaps a high-G stop, that makes pitch & roll read "nan"?
  //  Perhaps if we get a not-a-number condition, we need to reinitialize the MPU?

  // commented out Yaw output, because MO's head won't need it.
//  Serial.print("    Y (z): ");
//  Serial.println(yaw);
}
