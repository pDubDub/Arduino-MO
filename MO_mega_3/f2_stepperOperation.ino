
void updateSteppers() {  
  // stepper motor
            /*      forward would be B negative & D positive 
             *      backwards would be B postive & D negative
             *      left would be A negative & C positive
             *      right would be A postive & C negative
            */

      myStepperA.setSpeed(17);                       // added from sample
      myStepperC.setSpeed(17);
      myStepperB.setSpeed(17);
      myStepperD.setSpeed(17);
//                                                  // I've had this value as high as 17

      stepsToMove = 40;                             // not sure how 40 translates to degrees.
                                                    // TODO - should note that here.
      stepsToMove = (int)random(-50,51);            // overriding literal with a random distance
                                                    
//    if (true) {      // used to me if isAwake
          unsigned long currentMillisStepper = millis();

          if (currentMillisStepper - previousMillisStepper >= 400) {

          // save the last time you blinked the LED
          previousMillisStepper = currentMillisStepper;

          // The following frame switch was meant to get a demonstration pattern of running the various steppers.
          //    In practice, I can't get all 4 steppers functioning in the same sketch, I believe because it's drawing
          //    more current than power supply can provide.
          
          stepperFrame = 0;
//          stepperFrame = (int)random(0,2);
           switch(stepperFrame) {
              case 0:
                myStepperA.step(stepsToMove); 
//                myStepperC.step(-stepsToMove);
                break;
              case 1:
                myStepperB.step(stepsToMove); 
//                myStepperD.step(-stepsToMove); 
                break;
              case 2:
                myStepperC.step(stepsToMove); break;
              case 3:
                myStepperD.step(-stepsToMove); break;
            } // end SWITCH

            // this is currently written to only run frames 0 and 1, meaning only steppers A and B are moving.
//            (stepperFrame >= 1) ? (stepperFrame = 0) : (stepperFrame += 1);        //  if at end of animation loop, then reset to beginning

          }
//      myStepperA.step(stepsToMove);
//      delay(100);
//      myStepperC.step(-stepsToMove);
//      delay(100);
////        myStepperB.step(stepsToMove);
//      delay(100);
////        myStepperD.step(-stepsToMove);
//      delay(100);
//      delay(2);                                     // delay seems needed in order to get consistent motion
                                                  // but I don't like the idea of delay statements slowing down the loop
                                                  // will need to replace with a millis() funciton
      
//      stepsMoved = stepsMoved + stepsToMove;
      
      // I think this IF just make them switch directions in a while
//      if (stepsMoved >= 100) {                     // bigger number makes stepper move farther before switching direcions
//        stepsToMove = -1;
//      } else if (stepsMoved <=0) {
//        stepsToMove = 2;
//      }
//    }
    // I think later this code might be moved to a function, if not replaced entirely
    // this is really just to demonstrate MO program controlling a stepper motor
}
