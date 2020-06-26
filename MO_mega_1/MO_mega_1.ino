// I've moved all code to _setup_AND_main

/* Comments:
 *  Evolving sketch by Patrick Wheeler, 2020
 *    working on developing the control computer for a replica M-O robot
 *    
 *     >> Mega 1 = master processor, which handles behavior and articulation servos
 *        Mega 2 = slave processor, responsible solely for the LED matric for M-O's eyes
 *        Mega 3 = slave processor, responsible for Ball-Balance Robot, handling M-O's balance and mobility
 *    
 *    Capabilities:
 *      1 - Input (take operator commands)
 *        A - IR Remote
 *        B - Bluetooth
 *      2 - MO state and mood  
 *      3 - communicate with Slave Megas
 *      4 - Output
 *        A - Internal LED blink pattern
 *        B - RGB Pulse LED
 *        C - rear panel LCD
 *        D - sound playback
 *      5 - manage IMU for head
 *      6 - animation servos (arms, neck, etc)
 *        6A - red siren lamp
 *        
 *        
 *        TODO - Where is reading the IMU 6050?
 *    
 *    
 *    old:
 *      1 - IR Remote
 *      2 - Internal LED blink pattern
 *            Indicates Arduino functioning. Will later replicate on external LED pin 13 on rear panel of bot.
 *      3 - RGB LED
 *            Pulses green through bodywork when M-O is powered ON but asleep.
 *            Pulses RED if internal temp over 84˚
 *            Could be RED when battery is low.
 *            BLUE color is still unused. Maybe that will be low battery color.
 *      4 - Red Siren Lamp       
 *      5 - read IMU information from a MPU-6050
 *      6 - LED display   ** MOST LIKELY, NOT A FINAL FEATURE. POSSIBLE IT COULD BE USEFUL ON REAR PANEL. **
 *            Code taken from myThermometer sketch, based on Tutorial Lesson 15.
 *      7 - stepper motor added            
 */
