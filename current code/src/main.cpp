#include <Arduino.h>
#include <FastLED.h>

#define DATA_PIN 0    // DS
#define CLOCK_PIN 2   // SHCP
#define LATCH_PIN 1   // STCP
#define SENSOR_PIN 3  // Sensor
#define LED_PIN 4     // LED

#define READINGS 16
#define COLORS 3
#define TIME_FOR_OBJECT_SWITCH 10000

CRGB colorArr[] = {CRGB::Red, CRGB::Green, CRGB::Blue};  // Array of colors
unsigned int black[COLORS];                              // Values for black paper
unsigned int white[COLORS];                              // Values for white paper
unsigned int values[COLORS];                             // Values for the colors
CRGB leds[1];                                            // LED strip
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
    0b11110110,  // 9
    0b11101110,  // A
    0b00111110,  // b
    0b10011100,  // C
    0b01111010,  // d
    0b10011110,  // E
    0b10001110   // F
};

/// @brief shift out the bits to the 74HC595
/// @param digit digit place [0,5]
/// @param value segments to light up
void commit(byte digit, byte value) {
    // pull the latch low so the LEDs don't change while we're sending in bits
    digitalWrite(LATCH_PIN, LOW);
    // shift out the bits of the current digit place
    // 0 represents which digit to light up, 1 disables that digit
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ~(1 << (digit + 2)));
    // shift out the bits of the current digit segments
    // 1 represents which segments to light up, 0 disables that segment
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, value);
    // pull the latch high to lock in the data
    digitalWrite(LATCH_PIN, HIGH);
}

/// @brief Calibrate the sensor by reading white and black paper
void calibrate() {
    // White paper
    commit(0, 0b00000001);
    delay(TIME_FOR_OBJECT_SWITCH);
    commit(0, 0b00000000);
    for (int n = 0; n < COLORS; n++) {
        leds[0] = colorArr[n];
        FastLED.setBrightness(255);
        FastLED.show();
        delay(100);
        int value = 0;
        for (int i = 0; i < READINGS; i++) {
            value += analogRead(SENSOR_PIN);
            delay(5);
        }
        white[n] = value / READINGS;
    }
    FastLED.clear(true);
    // Black paper
    commit(1, 0b00000001);
    delay(TIME_FOR_OBJECT_SWITCH);
    commit(1, 0b00000000);
    for (int n = 0; n < COLORS; n++) {
        leds[0] = colorArr[n];
        FastLED.setBrightness(255);
        FastLED.show();
        delay(100);
        int value = 0;
        for (int i = 0; i < READINGS; i++) {
            value += analogRead(SENSOR_PIN);
            delay(5);
        }
        black[n] = value / READINGS;
    }
    FastLED.clear(true);
    commit(2, 0b00000001);
    delay(TIME_FOR_OBJECT_SWITCH);
    commit(2, 0b00000000);
}

/// @brief Read values for a specific colors
void readColors() {
    for (int n = 0; n < COLORS; n++) {
        leds[0] = colorArr[n];
        FastLED.setBrightness(255);
        FastLED.show();
        delay(100);
        int value = 0;
        for (int i = 0; i < READINGS; i++) {
            value += analogRead(SENSOR_PIN);
            delay(5);
        }
        value = value / READINGS;
        int greyDiff = white[n] - black[n];
        values[n] = (value - black[n]) * 0xFF / greyDiff;  // gives roughly 0-255
        if (values[n] < 0) values[n] = 0;
        if (values[n] > 0xFF) values[n] = 0xFF;
        FastLED.clear(true);
    }
}

/// @brief Convert the reading to digits displayed on the 7-segment display
void readingToDigits(unsigned int reading, byte idx = 1) {
    digits[idx] = (reading / 0x10) % 0x10;
    digits[idx - 1] = reading % 0x10;
}

/// @brief Function running once at the beginning of the program
void setup() {
    digits[0] = 0;
    digits[1] = 0;
    digits[2] = 0;
    digits[3] = 0;
    digits[4] = 0;
    digits[5] = 0;
    // set up output pins
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
    // set up input pin
    pinMode(SENSOR_PIN, INPUT);
    // set up LED
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, 1);

    calibrate();
    readColors();
    readingToDigits(values[0], 5);  // red
    readingToDigits(values[1], 3);  // green
    readingToDigits(values[2], 1);  // blue
}

/// @brief Function running in a loop after the setup
void loop() {
    for (int n = 5; n >= 0; n--) {
        commit(n, n % 2 == 0 ? (segment[digits[n]] | 0b00000001) : segment[digits[n]]);
    }
}
