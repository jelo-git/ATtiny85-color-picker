#include <Arduino.h>

/*
74HC595 Pins  LED Pins (+5V)
QA            A
QB            B
QC            C
QD            D
QE            E
QF            F
QG            G
QH            DP
*/

#define DATA_PIN 0   // DS
#define CLOCK_PIN 1  // SHCP
#define LATCH_PIN 2  // STCP

byte segment;

void setup() {
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
    segment = 0b10100101;
}

void loop() {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, segment);
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ~segment);
    digitalWrite(LATCH_PIN, HIGH);
    delay(250);
}
