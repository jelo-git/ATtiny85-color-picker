#include <Arduino.h>
#include <FastLED.h>

#define DATA_PIN 0    // DS
#define CLOCK_PIN 2   // SHCP
#define LATCH_PIN 1   // STCP
#define SENSOR_PIN 3  // Sensor
#define LED_PIN 4     // LED

int reading;
CRGB leds[1];
byte digits[6];
byte segment[] = {
    0b11111100,  // 0
    0b01100000,  // 1
    0b11011010,  // 2
    0b11110010,  // 3
    0b01100110,  // 4
    0b10110110,  // 5
    0b10111110,  // 6
    0b11100000,  // 7
    0b11111110,  // 8
    0b11110110   // 9
};

void setup() {
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);

    pinMode(SENSOR_PIN, INPUT);

    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, 1);

    FastLED.setBrightness(0);
    leds[0] = CRGB::White;
    FastLED.show();

    delay(250);
    reading = analogRead(SENSOR_PIN);

    FastLED.clear(true);

    digits[5] = reading / 100000;
    digits[4] = (reading / 10000) % 10;
    digits[3] = (reading / 1000) % 10;
    digits[2] = (reading / 100) % 10;
    digits[1] = (reading / 10) % 10;
    digits[0] = reading % 10;
}

void loop() {
    for (int n = 6; n >= 0; n--) {
        digitalWrite(LATCH_PIN, LOW);
        shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ~(1 << (n + 2)));
        shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, segment[digits[n]]);
        digitalWrite(LATCH_PIN, HIGH);
    }
}
