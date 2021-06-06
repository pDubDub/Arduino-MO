#include "Arduino.h"
#include <Math.h>
//#include <Wire.h>
//#include <Servo.h>
//#include <Adafruit_PWMServoDriver.h>                // for 16-channel I2C Servo Driver

// class definition
class MoServo {
  
    // would sort of like this to just inherit from Servo, but could not get it to work that way. Yet… TODO
    private:
        //properties:

        String SERVO_NAME;
//        Adafruit_PWMServoDriver pwmServoBoard;
        int CHANNEL_NUM;                                           // from 0 to 15
        
        int POSITION_MIN_MICROSECONDS;
        int POSITION_MAX_MICROSECONDS;
        int POSITION_START_MICROSECONDS;
        int POSITION_CENTER_MICROSECONDS;
          // MG90S MIN is 500, MAX is 2500
          // CENTER is the default, middle position, and may vary from servo to servo. 
          // START is the desired startup position, however, as some motions will start up at a retracted position.
          // First center used was actually 1420.
        
        int lastCommand, currentCommand, nextCommand;                //  microseconds
        
        // 

//        int currentSpeed;                 // ?
          /* I think the solution to mixing commands lies in tracking speed (microseconds/milli-loop) 
           *  as we do longer duration animations.
          */
          
        bool currentlyMoving = false;
        bool microsecondsIncreasing = false;                      // a simple var to track which direction we're going
        
        // we WILL need vars for current and next millis?
        unsigned long startOfMove, durationOfMove = 1000.0;       // time values
          // TODO - we could conceivably set duration dynamically based on distance of move?
        int startPosOfMove, distanceOfMove;                       // distance values
        int newDuration, nextDuration;
        float elapsedTime;                  // used for computing position along easing movement

        // now only appearing in commented out lines:
//        unsigned long currentUpdateMillis, millisOfLastMove;

        int currentPosition;        // was private. moved to public for parallel direct/pvm servo testing.
        
        int newServoCommand;                   // should start at initialPosition

        int minLimit;               // these will be used to temporarily override MIN/MAX
        int maxLimit;
        
    public:
//        Servo servo;                  // XX - these are obselete after transition to PWM servo board
//        int pin;                      // XX


        
        //methods:
        
        // old constructor              // XX - obselete constructor now that we've transitioned to PWM servo board
//        MoServo(int pin, int MIN, int MAX, int INIT) {
//            this->pin = pin;
//            this->POSITION_MIN_MICROSECONDS = MIN;
//            this->POSITION_MAX_MICROSECONDS = MAX;
//            this->POSITION_START_MICROSECONDS = INIT;
//            lastCommand = POSITION_START_MICROSECONDS;
//            currentCommand = POSITION_START_MICROSECONDS;
//            newServoCommand = POSITION_START_MICROSECONDS;
//            nextCommand = POSITION_START_MICROSECONDS;
//            currentPosition = POSITION_START_MICROSECONDS;
//
//            // perhaps setup commands like initial movement go here, and also attach()???
////            servo.attach(pin);
//        }

        // new constructor for PWM board servo
//        MoServo(String SERVO_NAME, Adafruit_PWMServoDriver &pwmServoBoard, int CHANNEL_NUM, int MIN, int CTR, int MAX) {
//                I tried to pass a reference to the ServoBoard so I could writeMicroseconds() directly from this class,
//                but it didn't work.
        MoServo(String SERVO_NAME, int CHANNEL_NUM, int MIN, int MAX, int START, int CTR) {
            // takes name, channel number, and microsecond limits for MIN, CTR, and MAX

            // TODO - I'm feeling like these would be simpler to configure if they take degrees instead of microseconds.
            //      Although, obviously, that's not quite as precise.
            
          this->SERVO_NAME = SERVO_NAME;
//          this->pwmServoBoard = pwmServoBoard;
          this->CHANNEL_NUM = CHANNEL_NUM;
          this->POSITION_MIN_MICROSECONDS = MIN;
          this->POSITION_MAX_MICROSECONDS = MAX;
          this->POSITION_START_MICROSECONDS = START;
          this->POSITION_CENTER_MICROSECONDS = CTR;
          lastCommand = POSITION_START_MICROSECONDS;
          currentCommand = POSITION_START_MICROSECONDS;
          newServoCommand = POSITION_START_MICROSECONDS;
          nextCommand = POSITION_START_MICROSECONDS;
          currentPosition = POSITION_START_MICROSECONDS;
          minLimit = MIN;
          maxLimit = MAX;

          // Note: Having print commands in constructor causes Crash when more than one class instance.
//          Serial.println("");
//          Serial.print("    debug: moServo named ");
//          Serial.print(SERVO_NAME);
//          Serial.println(" constucted.");
        }

        // FUTURE - it might be nice to be able to redefine the center/default position of the servo while running, rather than having to edit code
        //    but that will add a deal of complexity to iOS app, so I won't go that way yet.
        
//        void write(int degrees) {     // this was temporary, while getting control working.
//          servo.write(degrees);
//        }

        int getPosition() {
          return currentPosition;

          // TODO - should add a map() or a computed property so this function can return degrees instead of microseconds
        }

        String getName() {
          return SERVO_NAME;
        }

        int getChannel() {
          return CHANNEL_NUM;
        }

        // XX - I think this is just a test method to directly write an immediate position, rather than using easing
//        void writeMicroseconds(int microseconds) {
//          servo.writeMicroseconds(microseconds);
////          pwmServoBoard.writeMicroseconds(CHANNEL_NUM, microseconds);
//          Serial.print(SERVO_NAME);
//          Serial.print(" writeMicroseconds ");
//          Serial.println(microseconds);
////          pwmServoBoard_2.writeMicroseconds(CHANNEL_NUM, microseconds);
//            // not declared in this scope!
//        }

        // a simple method to send the servo to its initial position
        void goToStart() {
//          servo.writeMicroseconds(POSITION_START_MICROSECONDS);
            newServoCommand = POSITION_START_MICROSECONDS;
//            Serial.print(CHANNEL_NUM); Serial.print(" startup called to ");
//            Serial.print(newServoCommand); Serial.println(" μs.");
        }

        // a simple method to send the servo to its 'centered' normal/ready position
        void goToCenter() {
          newServoCommand = POSITION_CENTER_MICROSECONDS;
        }

        // 2 versions of commandTo, one that accepts int, and one that accepts String
        void commandTo(int degrees) {
          newServoCommand = map(degrees, 0, 180, 2500, 500);         // TODO - should these map between MIN and MAX ??
          newDuration = 1000;                                             // default duration
//          Serial.print("Servo "); Serial.print(CHANNEL_NUM); Serial.print(" commandTo(int degrees) to ");
//          Serial.print(newServoCommand); Serial.println(" μs.");

          // Now do limit checking.
          limitCheck();
        }

        void commandTo(String degreeString) {                             // this is implemented method to move moServo objects.
          newServoCommand = map(degreeString.toInt(), 0, 180, 2400, 500);      // left-limit was 2500, but found it was clipping the end of moves. (i.e. no distinction 0-10 degrees)
          newDuration = 1000;
//          Serial.print("Servo "); Serial.print(CHANNEL_NUM); Serial.print(" commandTo(String degrees) to "); 
//          Serial.print(newServoCommand);; Serial.println(" μs.");
          
          limitCheck();
        }

        // 3rd version now accepts a duration too.
        void commandTo(String degreeString, int duration) {
          newServoCommand = map(degreeString.toInt(), 0, 180, 2400, 500);
          newDuration = duration;

          limitCheck();
        }

                    // TODO - we now accept durationOfMove as a parameter, or default it to 1 sec if not specified.
                    //   Now make class a little smart, and if the move is notably shorter, shorten this value dynamically.

                    // in fact, what might be nice, is a nice gentle (possibly randomized) duration, but…
                    //        IF distance is short, then timer randomly shortens
                    //        IF ∆t between commands is shorter, then durationOfMove shortens?

        void limitCheck() {
           /*
           * If newServoCommand is less than MIN, newServoCommand = MIN
           * else if less than minLimit, then newServoCommand = minLimit
           * else if greater than MAX, newServoCommand = MAX
           * else if greater than maxLimit, newServoCommand = maxLimit
           */

           if (newServoCommand < POSITION_MIN_MICROSECONDS) {
            newServoCommand = POSITION_MIN_MICROSECONDS;
//            Serial.print(CHANNEL_NUM); Serial.println("Beyond MIN");
           } else if (newServoCommand < minLimit) {
            newServoCommand = minLimit;
//            Serial.print("Min Limiter "); Serial.println(minLimit);
           } else if (newServoCommand > POSITION_MAX_MICROSECONDS) {
            newServoCommand = POSITION_MAX_MICROSECONDS;
//            Serial.println("Beyond MAX");
           } else if (newServoCommand > maxLimit) {
            newServoCommand = maxLimit;
//            Serial.print("Max Limiter "); Serial.println(maxLimit);
           }
        }
        
        int updateServo() {
          // This is the main method, making the servo update with easing towards the next commanded position

            // set currentCommand and nextCommand
            if (newServoCommand != lastCommand) {                // have we received a newServoCommand
//                Serial.println("Update: New command received.");
                if (currentlyMoving) {
                    nextCommand = newServoCommand;
                    nextDuration = newDuration;
                } else {
                    currentCommand = newServoCommand;
                    durationOfMove = (long)newDuration;
                    nextCommand = newServoCommand;
                    nextDuration = newDuration;
                }

//                Serial.print("Current position = "); Serial.print(currentPosition);
//                Serial.print(". Last command = "); Serial.print(lastCommand);
//                    // lastCommand reassignment can't happen until after this line
//                Serial.print(". Current command = "); Serial.print(currentCommand);
//                Serial.print(". Next command = "); Serial.println(nextCommand);
                
                lastCommand = newServoCommand;
//                millisOfLastMove = millis();
            }

            // moving/updating the servo position
            if ((currentPosition == currentCommand) && currentlyMoving) {   // state = time to stop
                // then we've reached the end
//                Serial.println("Reached the end.");
//                servo.write(-currentPosition);
//                Serial.print("Servo "); Serial.print(CHANNEL_NUM); Serial.print(" at end of move to ");
//                Serial.println(currentCommand);
                currentlyMoving = false;
                currentCommand = nextCommand;
                durationOfMove = (long)nextDuration;
                    
            } else if (currentPosition != currentCommand){              // state = moving
                if (!currentlyMoving) {                                 // state = just starting
                  // set beginning of move
                  startOfMove = millis();
                  currentlyMoving = true;               // move into this IF as we only need do it once?
//                  Serial.print(CHANNEL_NUM); Serial.print(" starting to "); Serial.println(currentCommand);
                  
                  // set duration (we can use this later to change duration on command, yes?
//                  durationOfMove = 1000.0;
                    // THIS WORKS!
                    
                  startPosOfMove = currentPosition;
                  distanceOfMove = currentCommand - currentPosition;
//                  Serial.print("Distance of new move: "); Serial.print(distanceOfMove);
                  
                  if (currentCommand > currentPosition) {
                    // this bool is being implemented to make sure the motion only goes in one direction
                    //  eliminating a behavior that sometime makes the servo jerk back and forth at the end
                    microsecondsIncreasing = true;          // we're going up
//                    Serial.println(" up");
                  } else {
                    microsecondsIncreasing = false;         // we're going down
//                    Serial.println(" down");
                  }
                }

                /*  Easing algorithm:
                 *   total_travel"c" * ((∆t^2)/(2*((∆t^2)-∆t)+1))
                 *   
                 *   where t = current time / total time ( from 0 to 1.0)
                 *   
                 *   C$2*((B10*B10)/(2 * ((B10*B10)-B10) + 1) ) where B10 ranges from 0 to 1
                 *   distanceOfMove * (pow(t,2)/(2 * (power(t,2)-t) + 1));
                 *   
                 *        if B10 ranges 0 to 1.1 or more, you get a little bit of pass and return (aka bounce)
                 */

                elapsedTime = (millis() - startOfMove) / (float)durationOfMove;
                // TODO - might also error check here, to make sure elapsedTime is not beyond scope of move
                if (elapsedTime > durationOfMove) {
//                  Serial.println("Time Out!");
                }
//                Serial.print("elapsedTime = ");
//                Serial.print(t);
//                Serial.print("      computed point on curve = ");
//                Serial.println((pow(elapsedTime,2)/(2*(pow(elapsedTime,2)-elapsedTime)+1)));
                int proposedPosition =  distanceOfMove * (pow(elapsedTime,2)/(2 * (pow(elapsedTime,2)-elapsedTime) + 1)) + startPosOfMove;
//                Serial.print("Channel "); Serial.print(CHANNEL_NUM); Serial.print(" proposed position="); Serial.println(proposedPosition);
                
                // if difference is less than 10, then just move the difference instead of proposed
                    // TODO - I think this is the cause of the jumping at the end of some moves.
//                    if ( abs(currentPosition - currentCommand) < 4) {
////                  if ( abs(distanceOfMove) < 1) {       // never stopped the move.
//                      // move to command
//                      currentPosition = currentCommand;
//                      Serial.print("Suspect ABS line on servo number ");
//                      Serial.println(CHANNEL_NUM);
//                    } else {
//                      currentPosition = proposedPosition;
//                    }

                    // new version of previous IF
                    if ( abs(currentPosition - currentCommand) <= 1) {
                       currentPosition = currentCommand;
//                       Serial.print(CHANNEL_NUM); Serial.println(": Condition A, change is less than 4");
                    } else if ((microsecondsIncreasing) && (proposedPosition < currentPosition)) {
                      // if going up, and proposed is less than where we are, just stop
                      currentPosition = currentCommand;
//                      Serial.print(CHANNEL_NUM); Serial.println(": Condition B, increasing but proposal is lower");
                    } else if ((!microsecondsIncreasing) && (proposedPosition > currentPosition)) {
                      // if going down, and proposed is greater than where we are, just stop
                      currentPosition = currentCommand;
//                      Serial.print(CHANNEL_NUM); Serial.println(": Condition C, decreasing but proposal is higher");
                    } else {
                      currentPosition = proposedPosition;
                    }

                // TODO - could also make sure that algorithm is not proposing to move beyond destination, 
                //        unless of course we try to implement somekind of occasional bounce.
                
//                currentUpdateMillis = millis();   // not sure needed anymore

                // bounds checking
                // TODO - this should compare to LIMITS too
                    if (currentPosition > POSITION_MAX_MICROSECONDS) {
                      currentPosition = POSITION_MAX_MICROSECONDS;
                    } else if (currentPosition < POSITION_MIN_MICROSECONDS) {
                      currentPosition = POSITION_MIN_MICROSECONDS;
                    }

                // TODO - at some point, we also want to make sure we are not past limiter settings

                // and finally, move the servo:

//                servo.writeMicroseconds(currentPosition);       // moves the servo **
//                pwmServoBoard.writeMicroseconds(CHANNEL_NUM, currentPosition);

              // turned off these print statements, to see if they were causing anomalous movement.
//                Serial.print("   write to channel "); Serial.print(CHANNEL_NUM); Serial.print(" to "); Serial.print(currentPosition); Serial.println(" μs.");

                
                
//                millisOfLastMove = currentUpdateMillis;
            }

//            Serial.print("   class: "); Serial.print(currentPosition);
            return currentPosition;
                
        } // end of update()
};
