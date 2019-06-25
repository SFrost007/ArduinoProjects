/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */

static const int LED = 3;


void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  flash_short(3);
  pause_for(2);
  flash_long(2);
  pause_for(2);
  flash_short(3);
  delay(2000);
}




void flash_short(int repeats) {
  for (int i=0; i<repeats; i++) {
    light_for(200);
    pause_for(1);
  }
}

void flash_long(int repeats) {
  for (int i=0; i<repeats; i++) {
    light_for(600);
    pause_for(1);
  }
}

void pause_for(int repeats) {
  delay(repeats * 100);
}

void light_for(int ms) {
  digitalWrite(LED, HIGH);
  delay(ms);
  digitalWrite(LED, LOW);
}

