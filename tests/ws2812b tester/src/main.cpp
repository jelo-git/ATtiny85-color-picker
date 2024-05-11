#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 4

CRGB led[1];
byte red, green, blue;
bool rg, gb;

void setup() {
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(led, 1);
    red = 255;
    green = 0;
    blue = 0;
    rg = true;
    gb = false;
}

void loop() {
    if (rg) {
        if (red > 0) {
            red--;
            green++;
        } else {
            rg = false;
            gb = true;
        }
    } else if (gb) {
        if (green > 0) {
            green--;
            blue++;
        } else {
            rg = false;
            gb = false;
        }
    } else {
        if (blue > 0) {
            blue--;
            red++;
        } else {
            rg = true;
            gb = false;
        }
    }
    led[0] = CRGB(red, green, blue);
    FastLED.show();
    delay(5);
}
