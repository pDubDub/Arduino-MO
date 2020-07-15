
void awakeStateTo(bool state) {
  isAwake = state;
  stateDidChange = true;
//  updateEyes();
  if (isAwake) {
    Serial.println("wake eyes");
    eyeState = "waking";
  } else {
    Serial.println("sleep eyes");
//    sleepEyes();
    eyeState = "sleeping";
  }
}

// TODO - should have incoming commands set a string variable "emote"
//   Based on "emote" play specific eye sequences and sounds, and then set emote = "none" when finished.
