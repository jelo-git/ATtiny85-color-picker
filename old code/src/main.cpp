#include <Arduino.h>
#include <FastLED.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define DATA_PIN 0   // DS
#define CLOCK_PIN 2  // SHCP
#define LATCH_PIN 1  // STCP
#define SENSOR_PIN 3
#define LED_PIN 4

#define READING_POPULATION 255
#define SENSITIVITY 0.4
#define STANDBY_TIMER 64

byte *registers;  // array to hold the values of the 74hc595 registers
// index: 0 -> segment register, 1 -> digit register

byte red, green, blue, timer;
bool sensorReadFlag;
CRGB led[1];

void clearRegisters() {
    registers[0] = 0;
    registers[1] = 0b11111111;
}

void setRegister(int n, byte value) {
    registers[n] = value;
}

void setRegisterPin(int pin, bool value) {
    int r = pin / 8;  // determine which register to use
    int b = pin % 8;  // determine which bit to set
    if (value) {
        registers[r] |= (1 << b);
    } else {
        registers[r] &= ~(1 << b);
    }
}

void shiftOutRegisters() {
    digitalWrite(LATCH_PIN, LOW);  // pull latch low to start sending data
    for (int i = 1; i >= 0; i--) {
        shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, registers[i]);
    }
    digitalWrite(LATCH_PIN, HIGH);  // pull latch high to stop sending data
}

byte getNumberSegments(uint8_t number) {
    switch (number) {
        case 0:
            return 0b11111100;
        case 1:
            return 0b01100000;
        case 2:
            return 0b11011010;
        case 3:
            return 0b11110010;
        case 4:
            return 0b01100110;
        case 5:
            return 0b10110110;
        case 6:
            return 0b10111110;
        case 7:
            return 0b11100000;
        case 8:
            return 0b11111110;
        case 9:
            return 0b11110110;
        case 10:
            return 0b11101110;
        case 11:
            return 0b00111110;
        case 12:
            return 0b10011100;
        case 13:
            return 0b01111010;
        case 14:
            return 0b10011110;
        case 15:
            return 0b10001110;
        case 16:
            return 0b00000001;
        default:
            return 0b00000000;
    }
}
// 6 5 4 3 2 1 0 digit numbers based on position
void displayDigit(uint8_t digit, uint8_t position, bool dp = false) {
    byte segment = getNumberSegments(digit);
    if (dp) segment |= 0b00000001;
    setRegister(0, segment);
    setRegister(1, ~(1 << (7 - position)));
    shiftOutRegisters();
}

void checkSensor(CRGB val, byte *variable) {
    led[0] = val;
    FastLED.show();
    delay(100);
    for (int i = 0; i < READING_POPULATION; i++) {
        uint16_t reading = analogRead(SENSOR_PIN);
        reading = map(reading, 0, 1023, 0, 255);
        if (i == 0) {
            *variable = (*variable * (1.0 - SENSITIVITY)) + (reading * SENSITIVITY);
        }
        delay(5);
    }
    FastLED.clear(true);
}

void goSleep() {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    ADCSRA &= ~(1 << ADEN);  // disable ADC
    sleep_cpu();
}

void loadingAnimation() {
    for (int i = 0; i < 6; i++) {
        displayDigit(16, i);
        delay(250);
    }
}

void setup() {
    registers = new byte[2];
    clearRegisters();

    red = 0;
    green = 0;
    blue = 0;
    timer = 0;
    led[0] = CRGB(0, 0, 0);
    sensorReadFlag = true;

    // setup output pins
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
    // setup FastLED output
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(led, 1);
    // setup input pins
    pinMode(SENSOR_PIN, INPUT);

    // Set up timer 1
    cli();  // Disable global interrupts
    TCCR1 |= 0b00001111;
    TIMSK |= 1 << TOIE1;
    // sei();  // Enable global interrupts
}

void loop() {
    switch (sensorReadFlag) {
        case true:
            loadingAnimation();
            delay(1000);
            checkSensor(CRGB::Red, &red);
            checkSensor(CRGB::Green, &green);
            checkSensor(CRGB::Blue, &blue);
            FastLED.clear(true);
            sensorReadFlag = false;
            sei();
            break;
        case false:
            displayDigit(red / 16, 5);
            displayDigit(red % 16, 4, true);
            displayDigit(green / 16, 3);
            displayDigit(green % 16, 2, true);
            displayDigit(blue / 16, 1);
            displayDigit(blue % 16, 0);
            break;
    }
}

ISR(TIMER1_OVF_vect) {
    timer++;
    if (timer >= STANDBY_TIMER) {
        cli();
        clearRegisters();
        shiftOutRegisters();
        goSleep();
    }
}