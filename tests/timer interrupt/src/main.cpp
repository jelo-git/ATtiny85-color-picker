#include <Arduino.h>
#include <FastLED.h>
#include <avr/interrupt.h>

#define LED_PIN 4

CRGB leds[1];
bool blink;

void setup() {
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, 1);
    blink = false;

    // Set up timer 1
    cli();  // Disable global interrupts
    TCCR1 |= 0b00001111;
    TIMSK |= 1 << TOIE1;
    sei();  // Enable global interrupts
}

void loop() {
    if (blink) {
        leds[0] = CRGB::White;
        FastLED.show();
    } else {
        FastLED.clear(true);
    }
}

ISR(TIMER1_OVF_vect) {
    blink = !blink;
}
