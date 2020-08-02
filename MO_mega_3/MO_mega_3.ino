
/* Comments:
 *  Evolving sketch by Patrick Wheeler, 2020
 *    working on developing the control computer for a replica M-O robot
 *    
 *        Mega 1 = master processor, which handles behavior and articulation servos
 *        Mega 2 = slave processor, responsible solely for the LED matric for M-O's eyes
 *     >> Mega 3 = slave processor, responsible for Ball-Balance Robot, handling M-O's balance and mobility
 *    
 *    Capabilities:
 *      1 - Input
 *        A - reveive commands & states from Mega-1
 *        B - read gyro data from IMU 9250 on BBR (used to balance and operate Ball Balance Robot)
 *      2 - MO state and mood tracking
 *      3 - communication back to Master
 *      4 - Output
 *        A - blink LED to indicate operation
 *      5 - operate the BBR
 *            
 *      If running, then run LED blink pattern.      
 *      Manage IMU for BBR
 *      
 *      What is MO's state? 
 *             isCollapsed?
 *             If isCollapsed, then do nothing.
 *      If no movement commands, then park (land foot) after delay.
 *      If not parked, then balance
 *        and adjust balance to meet movement requests.
 *        
 */
