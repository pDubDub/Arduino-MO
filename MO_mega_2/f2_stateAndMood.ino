
// **** SLEEP ****
void awakeStateTo(bool state) {
  isAwake = state;
  stateDidChange = true;
//  updateEyes();
  if (isAwake) {                        // woke up
    Serial.println("wake eyes");
    eyeState = "waking";
  } else {                              // went to sleep
    Serial.println("sleep eyes");
//    sleepEyes();
    eyeState = "sleeping";
    nextSleepEmote = millis() + (long)( (random(1,7)) * 30000 );
//    Serial.print("Next sleep emote will be at "); Serial.print(nextSleepEmote); Serial.println(" if still asleep then.");
  }
}

void runSleepingBehaviors() {
    // this runs if (isReady && !isAwake)
    
     /* pseudocode for random snorePurr:
      *  
      *  on went to sleep:
      *   • create nextSleepEmote to now + a random int(1-6) * 30000 millis delay passes (this would yield 0:30 to 3:00 in 30 second increments)
      *         (did this above in awakeStateTo() )
      *         
      *         -alternately, could just make this happen every minute, as a reminder that he in ON and ASLEEP
      *   
      *   
      *  then during this function each loop: 
      *   if (millis() > nextSleepEmote) { 
      *     
      *     • randomly pick eyes or snore
      *     • either do a random peek eye emote or a snore sound playback
      *       - maybe the snore sound could trigger a low, squinty eyed emote
      *     
      *     • reset nextSleepEmote
      *     nextSleepEmote = millis() + ( (int)random(1,6) * 30000 );
      *   }
      *  
    */

    // This system plays a sleepEmote (currently just a snore) at a random interval between 0:30 and 3:00 (in 30 second increments)

    if (millis() > nextSleepEmote) {
      // simplified form always snores for now
      myDFPlayer.play(3);
//      Serial.print("Snore at "); Serial.print(millis());

//      long snoreDelay = (long)((random(1,7)) * 30000);
      nextSleepEmote = millis() + (long)((random(1,7)) * 30000);
//      Serial.print(". Next delay will be "); Serial.print(snoreDelay);
//      Serial.print(". Next snore at "); Serial.println(nextSleepEmote);

      // this seems to be working in principle, after fixing issues trying to do math between int and long.
    }
}

// **** EMOTES ****
// TODO - should have incoming commands set a string variable "emote"
//   Based on "emote" play specific eye sequences and sounds, and then set emote = "none" when finished.

void playEmote(String emo) {
  Serial.print("MO-2 received command to perform emote "); Serial.println(emo);
}
