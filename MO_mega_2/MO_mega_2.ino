/* Comments:
 *  Evolving sketch by Patrick Wheeler, 2020
 *    working on developing the control computer for a replica M-O robot
 *    
 *        Mega 1 = master processor, which handles behavior and articulation servos
 *     >> Mega 2 = slave processor, responsible solely for the LED matric for M-O's eyes
 *        Mega 3 = slave processor, responsible for Ball-Balance Robot, handling M-O's balance and mobility
 *    
 *    Capabilities:
 *      0 - receive commands from Mega 1
 *      1 - Output
 *        A - Internal LED blink pattern
 *      2 - operate LED matrix for M-O's eyes
 *            - take commands from Master
 *            - create appropriate expressions
 *            - randomized frames/sequences to keep M-O expressive when not receiving commands
 *                (i.e. an occasional blink)
 *      3 - maintain array of expressions (frames)
 *      4 - control audio playback
*              * pulled from DFPlayerOnUno sketch
*              (left out a lot of added tutorial functionality that I didn't think MO needs)
 *    
 */
