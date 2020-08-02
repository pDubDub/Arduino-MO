#include "Arduino.h"
#include <Servo.h>
#include <Math.h>

// class definition
class MoServo {
  
    // would sort of like this to just inherit from Servo, but could not get it to work that way. Yet… TODO
    private:
        //properties:
        int MIN_POS_MIC, MAX_POS_MIC, INIT_POS_MIC;                  // microseconds
          // MG90S MIN is 500, MAX is 2500
          // INIT is the desired startup/neutral position, and may vary from servo to servo. First used was actually 1420.
        
        int lastCommand, currentCommand, nextCommand;                //  microseconds
        int currentPosition;

//        int currentSpeed;                 // ?
          // I think the solution to mixing commands lies in tracking speed (microseconds/milli-loop) as we do longer duration animations
        
        bool currentlyMoving = false;
        
        // we WILL need vars for current and next millis?
        unsigned long startOfMove, durationOfMove = 1000.0;
        int startPosOfMove, distanceOfMove;
        int newDuration, nextDuration;
        float elapsedTime;                  // used for computing position along easing movement

        // now only appearing in commented out lines:
//        unsigned long currentUpdateMillis, millisOfLastMove;
        
    public:
        Servo servo;
        int pin;
        
        int newCommand;                   // should start at initialPosition

//        int minimumLimiter;               // these will be used to temporarily override MIN/MAX
//        int maximumLimiter;
        
        //methods:
        
        // constructor
        MoServo(int pin, int MIN, int MAX, int INIT) {
            this->pin = pin;
            this->MIN_POS_MIC = MIN;
            this->MAX_POS_MIC = MAX;
            this->INIT_POS_MIC = INIT;
            lastCommand = INIT_POS_MIC;
            currentCommand = INIT_POS_MIC;
            newCommand = INIT_POS_MIC;
            nextCommand = INIT_POS_MIC;
            currentPosition = INIT_POS_MIC;
            
            // perhaps setup commands like initial movement go here, and also attach()???
//            servo.attach(pin);
            
//            Serial.println("    debug: MoServo object constructed!");
        }

        // FUTURE - it might be nice to be able to redefine the center/default position of the servo while running, rather than having to edit code
        //    but that will add a deal of complexity to iOS app, so I won't go that way yet.
        
//        void write(int degrees) {     // this was temporary, while getting control working.
//          servo.write(degrees);
//        }

        void writeMicroseconds(int microseconds) {
          servo.writeMicroseconds(microseconds);
        }

        // a simple method to send the servo to its initial position
        void start() {
          servo.writeMicroseconds(INIT_POS_MIC);
        }

        // 2 versions of commandTo, one that accepts int, and one that accepts String
        void commandTo(int degrees) {
          newCommand = map(degrees, 0, 180, 2500, 500);
          newDuration = 1000;                                             // default duration
        }

        void commandTo(String degreeString) {                             // this is implemented method to move moServo objects.
          newCommand = map(degreeString.toInt(), 0, 180, 2400, 500);      // left-limit was 2500, but found it was clipping the end of moves. (i.e. no distinction 0-10 degrees)
          newDuration = 1000;
        }

        // 3rd version now accepts a duration too.
        void commandTo(String degreeString, int duration) {
          newCommand = map(degreeString.toInt(), 0, 180, 2400, 500);
          newDuration = duration;
        }

                    // TODO - we now accept durationOfMove as a parameter, or default it to 1 sec if not specified.
                    //   Now make class a little smart, and if the move is notably shorter, shorten this value dynamically.

                    // in fact, what might be nice, is a nice gentle (possibly randomized) duration, but…
                    //        IF distance is short, then timer randomly shortens
                    //        IF ∆t between commands is shorter, then durationOfMove shortens?
                    
        void updateServo() {
            // set currentCommand and nextCommand
            if (newCommand != lastCommand) {                // have we received a newCommand
//                Serial.print("New command received. ");
                if (currentlyMoving) {
                    nextCommand = newCommand;
                    nextDuration = newDuration;
                } else {
                    currentCommand = newCommand;
                    durationOfMove = (long)newDuration;
                    nextCommand = newCommand;
                    nextDuration = newDuration;
                }

//                Serial.print("Current position = ");
//                Serial.print(currentPosition);
//                Serial.print(". Last command = ");
//                Serial.print(lastCommand);
//                    // lastCommand reassignment can't happen until after this line
//                Serial.print(". Current command = ");
//                Serial.print(currentCommand);
//                Serial.print(". Next command = ");
//                Serial.println(nextCommand);
                
                lastCommand = newCommand;
//                millisOfLastMove = millis();
            }

            // moving/updating the servo position
            if ((currentPosition == currentCommand) && currentlyMoving) {   // state = time to stop
                // then we've reached the end
//                Serial.println("Reached the end.");
//                servo.write(-currentPosition);
                currentlyMoving = false;
                currentCommand = nextCommand;
                durationOfMove = (long)nextDuration;

                    // TODO - where is state = starting?
                    
            } else if (currentPosition != currentCommand){              // state = moving
                if (!currentlyMoving) {                             // state = just starting
                  // set beginning of move
                  startOfMove = millis();
//                  Serial.println("starting");
                  // set duration (we can use this later to change duration on command, yes?
//                  durationOfMove = 1000.0;
                    // THIS WORKS!
                    

                  
                  startPosOfMove = currentPosition;
                  distanceOfMove = currentCommand - currentPosition;
//                  Serial.print("Distance of new move: ");
//                  Serial.println(distanceOfMove);
                                  
                  currentlyMoving = true;               // move into this IF as we only need do it once?
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
                
//                Serial.print("elapsedTime = ");
//                Serial.print(t);
//                Serial.print("      computed point on curve = ");
//                Serial.println((pow(elapsedTime,2)/(2*(pow(elapsedTime,2)-elapsedTime)+1)));
                int proposedPosition =  distanceOfMove * (pow(elapsedTime,2)/(2 * (pow(elapsedTime,2)-elapsedTime) + 1)) + startPosOfMove;
//                Serial.print("proposed position=");
//                Serial.println(proposedPosition);
                
                // if difference is less than 10, then just move the difference instead of proposed
                if ( abs(currentPosition - currentCommand) < 10) {
                  // move to command
                  currentPosition = currentCommand;
                } else {
                  currentPosition = proposedPosition;
                }

                // TODO - could also make sure that algorithm is not proposing to move beyond destination, 
                //        unless of course we try to implement somekind of occasional bounce.
                
//                currentUpdateMillis = millis();   // not sure needed anymore

                // bounds checking
                if (currentPosition > MAX_POS_MIC) {
                  currentPosition = MAX_POS_MIC;
                } else if (currentPosition < MIN_POS_MIC) {
                  currentPosition = MIN_POS_MIC;
                }

                // TODO - at some point, we also want to make sure we are not past limiter settings

                // move the servo:
//                Serial.print("   Moving to ");
//                Serial.println(currentPosition);
                servo.writeMicroseconds(currentPosition);       // moves the servo **
                
//                millisOfLastMove = currentUpdateMillis;
            }
        }
};
