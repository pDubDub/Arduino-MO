// 6A --  the Foreign Contaminent red siren light on M-O's head, currently set to trigger via #0 on the IR remote
void updateSirenLamp() {
  if (sirenDidChange == true) {     // state change boolean, so these commands only run when there's change, not every loop cycle
    if (sirenOn == true) {
      sirenSpinServo.write(115);            // tells continuous servo to go (160 is about 70 rpm)
      sirenLiftServo.write(SERVO_THROW);   // should send other servo up to throw
        // TODO -- only use this constant in this place. Might swtich to literal 180.
      digitalWrite(RED_LAMP_LED, HIGH);        // and red LED on
    } else {
      sirenSpinServo.write(87);                      //          stops rotation
      sirenLiftServo.write(0);                       //          retracts servo1
      digitalWrite(RED_LAMP_LED, LOW);               // and red LED off
    }
    sirenDidChange = false;         // reset DidChange boolean so IF won't run again next loop
  }
}
