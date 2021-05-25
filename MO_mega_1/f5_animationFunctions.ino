
/* Here will go functions for
 *  
 *    animation (mostly controlling servos, but also the LEDs in the siren lamp)
 *    
 */
 
// 5A --  the Foreign Contaminent red siren light on M-O's head, currently set to trigger via #0 on the IR remote

void updateSirenLamp() {
  if (sirenDidChange == true) {     // state change boolean, so these commands only run when there's change, not every loop cycle
    if (sirenOn == true) { 
      sirenSpinServo.write(100);                        // tells continuous servo to go (90 ~ stopped, 100 is pretty close to 60rpm)
      
//      sirenLiftServo.write(180);                        // sends lift servo to 'up' position
      pwmServoBoard_1.writeMicroseconds(1, 1800);         // lift servo on PWM board 1
        
        // TODO -- might replace with .writeMicroseconds() to get better control of throw limits.
        // should take about 1.2 seconds to deploy

      digitalWrite(RED_LAMP_LED, HIGH);                 // and red LED on
    } else {
      sirenSpinServo.writeMicroseconds(1454);           // fine tuned for zero rotation of FS90R c-r servo
      
//      sirenLiftServo.write(0);                        // retracts servo1
      pwmServoBoard_1.writeMicroseconds(1, 1200);       // lift servo on PWM board 1
      
      digitalWrite(RED_LAMP_LED, LOW);                  // and red LED off
    }
    sirenDidChange = false;                             // reset DidChange boolean so IF won't run again next loop
  }
}

/*
 * Error checking and motion limits (i.e. is head in the acceptable position before raising arms) should be performed here.
 *    The servo class should only be concerned with moving servos.
 *    
 *    This is where we should have things like a compact() method, that parks the body, drops his head, etc.
 *    
 *    Servo class should have a getPosition() method that will return the current position, which methods here can compare
 *        with other postions and what commands are waiting from iOS.
 *        
 *       
 */

void servosToCenter() {
  // this is a temporary function, that centers all of the test servos
  Serial.println("servosToCenter called");

  // these are old commands, that send the servo directly to a position
  pwmServoBoard_1.writeMicroseconds(0, 1500);         // head louvres
  pwmServoBoard_1.writeMicroseconds(1, 1500);         // siren lift
    // 2 is siren spin
  pwmServoBoard_1.writeMicroseconds(3, 1500);         // head yaw
  pwmServoBoard_1.writeMicroseconds(4, 1500);         // head pitch
  pwmServoBoard_1.writeMicroseconds(5, 1500);         // head roll
  pwmServoBoard_1.writeMicroseconds(6, 1500);         // neck lift
  pwmServoBoard_1.writeMicroseconds(7, 1500);         // neck lean

  // these are new commands, using easing, that send a command, and update every main loop
  newTestServo8.goToCenter();
  newTestServo9.goToCenter();
  newTestServo10.goToCenter();
  
//  pwmServoBoard_2.writeMicroseconds(8, 1500);         // shoulder track
//  pwmServoBoard_2.writeMicroseconds(9, 1500);         // shoulder shrug
//  pwmServoBoard_2.writeMicroseconds(10, 1500);        // arm pivot
  pwmServoBoard_2.writeMicroseconds(11, 1500);        // arm extension
  pwmServoBoard_2.writeMicroseconds(12, 1500);        // scrubber cover
    // 13 is scrubber motor
  pwmServoBoard_2.writeMicroseconds(14, 1500);        // torso lift
  pwmServoBoard_2.writeMicroseconds(15, 1500);        // foot lift
}
 
void servosToStart() {
  Serial.println("servosToStart called.");

  pwmServoBoard_1.writeMicroseconds(0, 1500);         // head louvres
  pwmServoBoard_1.writeMicroseconds(1, 1500);         // siren lift
    // 2 is siren spin
  pwmServoBoard_1.writeMicroseconds(3, 1500);         // head yaw
  pwmServoBoard_1.writeMicroseconds(4, 1500);         // head pitch
  pwmServoBoard_1.writeMicroseconds(5, 1500);         // head roll
  pwmServoBoard_1.writeMicroseconds(6, 1500);         // neck lift
  pwmServoBoard_1.writeMicroseconds(7, 1500);         // neck lean
  
  newTestServo8.goToStart();
  newTestServo9.goToStart();
  newTestServo10.goToStart();
}
