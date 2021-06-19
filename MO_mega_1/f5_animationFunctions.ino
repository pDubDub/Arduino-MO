
/* Here will go functions for
 *  
 *    animation (mostly controlling servos, but also the LEDs in the siren lamp)
 *    
 */
 
// 5A --  the Foreign Contaminent red siren light on M-O's head, currently set to trigger via #0 on the IR remote

void updateSirenLamp() {
  if (sirenDidChange == true) {     // state change boolean, so these commands only run when there's change, not every loop cycle
    if (sirenOn == true) { 
//      sirenSpinServo.write(100);                        // tells continuous servo to go (90 ~ stopped, 100 is pretty close to 60rpm)
      
      moServo01SirenLift.commandTo(170, 1200);                       // sends lift servo to 'up' position
        
        // TODO -- might replace with .writeMicroseconds() to get better control of throw limits.
        // should take about 1.2 seconds to deploy

      moServo02SirenSpin.commandTo(100);

      digitalWrite(RED_LAMP_LED, HIGH);                 // and red LED on
    } else {
      // turning off the siren...
//      sirenSpinServo.writeMicroseconds(1454);           // fine tuned for zero rotation of FS90R c-r servo
      
      moServo01SirenLift.goToStart();                        // retracts servo1

      moServo02SirenSpin.goToStart();
      
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
 
  moServo00Louvers.goToCenter();                        // head louvres
  moServo01SirenLift.goToCenter();                      // siren lift
    // 2 is siren spin
  moServo03HeadPan.goToCenter();                        // head yaw
  newTestServo4.goToCenter();                           // head pitch
  newTestServo5.goToCenter();                           // head roll
  newTestServo6.goToCenter();                           // neck lift
  newTestServo7.goToCenter();                           // neck lean

  // these are new commands, using easing, that send a command, and update every main loop
  moServo08ArmTrack.goToCenter();                       // shoulder track                 
  moServo09ArmShrug.goToCenter();                       // shoulder shrug
  moServo10ArmLift.goToCenter();                          // arm pivot
  moServo11ArmExtend.goToCenter();                      // arm extension
    // 12 will be scrubber cover flip
    // 13 is scrubber motor
  moServo14Torso.goToCenter();                          // torso lift
  moServo15Foot.goToCenter();                           // foot lift
}
 
void servosToStart() {
  Serial.println("servosToStart called.");

  moServo00Louvers.goToStart();                         // head louvres
  moServo01SirenLift.goToStart();                       // siren lift
  moServo02SirenSpin.goToStart();                       // 2 is siren spin
  moServo03HeadPan.goToStart();                            // head yaw
  newTestServo4.goToStart();                            // head pitch
  newTestServo5.goToStart();                            // head roll
  newTestServo6.goToStart();                            // neck lift
  newTestServo7.goToStart();                            // neck lean
    
    // pwm board 2...  
  moServo08ArmTrack.goToStart();
  moServo09ArmShrug.goToStart();
  moServo10ArmLift.goToStart();
  moServo11ArmExtend.goToStart();
    // 12 will be scrubber cover flip
    // 13 is scrubber motor
  moServo14Torso.goToStart();
  moServo15Foot.goToStart();
}

void torsoStandsUp() {
  Serial.println();
  Serial.println("Would call servo commands to stand up.");

  moServo14Torso.commandTo(150);
  newTestServo6.commandTo(90);
//  newTestServo6.goToCenter();
      // TODO - This goToCenter() command does not appear to be working.
}

void torsoParks() {
  Serial.println();
  Serial.println("Park command. Will send servos to start");
  // really, this might be part of some other methods, like a sleep() method
  // but might be able to be called seperately, so you can park him without sleeping him
  servosToStart();
}

/*
 * Wherever the sleep command lives, it might call the eyes to sleep and then call this Park() command.
 * We might even do alternate park() methods, that might make him turn and then park, like he does when meeting Wall-E.
 */
