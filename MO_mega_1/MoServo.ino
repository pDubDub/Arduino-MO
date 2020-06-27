#include "Arduino.h"
#include <Servo.h>
#include <Math.h>

// class definition
class MoServo {
  
    // would sort of like this to just inherit from Servo, but could not get it to work that way. Yet… TODO
    private:
        //properties:
        int MIN_POS_MIC, MAX_POS_MIC, INIT_POS_MIC;                  // microseconds
        
        int lastCommand, currentCommand, nextCommand;                //  microseconds
        int currentPosition;

//        int currentSpeed;                 // ?
          // I think the solution to mixing commands lies in tracking speed (microseconds/milli-loop) as we do longer duration animations
        
        bool currentlyMoving = false;
        
        // we WILL need vars for current and next millis?
        unsigned long startOfMove, durationOfMove;
        int startPosOfMove, distanceOfMove;
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
            
            Serial.println("    debug: MoServo object constructed!");
        }

        // TODO - will need a good way, perhaps a method, to center/default the servo. 
        // The first MG90S that I tested was centered at 1420 microseconds, due to how the arm connects.
        
//        void write(int degrees) {     // this was temp, while getting control working.
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
        }

        void commandTo(String degreeString) {
          newCommand = map(degreeString.toInt(), 0, 180, 2500, 500);
        }
        
        void updateServo() {
            // set currentCommand and nextCommand
            if (newCommand != lastCommand) {                // have we received a newCommand
//                Serial.print("New command received. ");
                if (currentlyMoving) {
                    nextCommand = newCommand;
                } else {
                    currentCommand = newCommand;
                    nextCommand = newCommand;
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

                    // TODO - where is state = starting?
                    
            } else if (currentPosition != currentCommand){              // state = moving
                if (!currentlyMoving) {                             // state = just starting
                  // set beginning of move
                  startOfMove = millis();
//                  Serial.println("starting");
                  // set duration (we can use this later to change duration on command, yes?
                  durationOfMove = 1000.0;
                    // THIS WORKS!
                    // TODO - we could now either accept durationOfMove as a parameter,
                    //   or make class a little smart, and if the move is notably shorter, we could shorten this value dynamically

                    // in fact, what might be nice, is a nice gentle (possibly randomized) duration, but…
                    //        IF distance is short, then timer randomly shortens
                    //        IF ∆t between commands is shorter, then durationOfMove shortens?
                  
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
