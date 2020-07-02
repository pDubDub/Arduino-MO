
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
      sirenLiftServo.write(180);                        // sends lift servo to 'up' position
        
        // TODO -- might replace with .writeMicroseconds() to get better control of throw limits.
        // should take about 1.2 seconds to deploy

      digitalWrite(RED_LAMP_LED, HIGH);        // and red LED on
    } else {
      sirenSpinServo.writeMicroseconds(1454);        //          fine tuned for zero rotation of FS90R c-r servo
      sirenLiftServo.write(0);                       //          retracts servo1
      digitalWrite(RED_LAMP_LED, LOW);               // and red LED off
    }
    sirenDidChange = false;         // reset DidChange boolean so IF won't run again next loop
  }
}
