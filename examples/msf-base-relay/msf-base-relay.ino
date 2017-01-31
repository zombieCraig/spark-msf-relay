///////////////////////////////////
//  MSF - Metasploit Framework   //
//  Hardware Bridge base API     //
///////////////////////////////////

// This is the bare minimum.  It will allow Metasploit to
// Connect to the device pull some stats and blink an LED

#include "spark-msf-relay.h"

MSFRelay msf;

void setup() {
  msf.setup();
  msf.begin();
}

void loop() {
  msf.loop();
}
