#include <Arduino.h>

/*
1st 74HC595 Pins  LED Pins (VCC)
QA                A
QB                B
QC                C
QD                D
QE                E
QF                F
QG                G
QH                DP

1st 74HC595 Pins  2nd 74HC595 Pins
QH'               DS

2nd 74HC595 Pins  LED Pins (GND)
QA                D1
QB                D2
QC                D3
QD                D4
QE                D5
QF                D6


*/

#define DATA_PIN 0   // DS
#define CLOCK_PIN 2  // SHCP
#define LATCH_PIN 1  // STCP

byte segment;
byte digit;

void setup() {
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
    segment = 1;
    digit = 1;
}

void loop() {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ~digit);
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, segment);
    digitalWrite(LATCH_PIN, HIGH);
    segment = segment << 1;
    if (segment == 0) {
        segment = 1;
        digit = digit << 1;
    }
    if (digit == 0) {
        digit = 1;
    }
    delay(100);
}
