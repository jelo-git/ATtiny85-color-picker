#include <Arduino.h>

/*
74HC595 Pins  LED Pins (GND)
QA            D1
QB            D2
QC            D3
QD            D4
QE            D5
QF            D6
*/

#define DATA_PIN 0   // DS
#define CLOCK_PIN 1  // SHCP
#define LATCH_PIN 2  // STCP

byte segment;

void setup() {
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
    segment = 1;
}

void loop() {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ~segment);
    digitalWrite(LATCH_PIN, HIGH);
    segment = segment << 1;
    if (segment == 0) {
        segment = 1;
    }
    delay(250);
}
