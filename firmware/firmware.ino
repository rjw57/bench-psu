#include <LiquidCrystal.h>

#include "AnalogWrite16.h"
#include "pins.h"

// Liquid crystal display
LiquidCrystal lcd(
    PIN_LCD_RS, PIN_LCD_E, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7
);

/* Note on "analog" output
 * =======================
 *
 * The Arduino analog output is actually PWM. The analogWrite16 routine used in
 * this sketch outputs PWM at 244Hz which is quite low frequency as it goes. In
 * order to smooth this out we want a low-pass filter with a cut-off frequency
 * of at most 120Hz. Since
 *
 *   f_c = 1 / (2 * pi * R * C),
 *
 *   f_c < 240 => 2 * pi * R * C > 1/240
 *
 * or, equivalently,
 *
 *   R * C > 1 / (240 * 2 * pi)
 *         > 6.6e-4
 *
 * So, if C = 100n, then R > 6.6e-4 / 100e-9 or R > 6.6e3. A comfortable margin
 * is setting R > 10K and cascading two or more filters.
 *
 * Pleasingly, direct inspection of the board shows the use of a third order RC
 * filter. Measurement of C suggests C ~= 100nF and R is marked as 100K. This
 * means we should be able to feed PWM directly into the control board.
 */


const float PWM_P_TO_P = 3.3f; // We buffer through a 4050
const uint16_t MAX_ANALOG_WRITE = 0x03ff; // 10-bit PWM

// TODO: These formulae are taken directly from the reverse engineering of the
// B3603. They need verifying.

inline uint16_t v_out_to_pwm(float v_out) {
    float target_voltage = (16.f/15.f) * (31e-3f + 0.068f*v_out);
    float duty_cycle = max(0.f, min(1.f, target_voltage / PWM_P_TO_P));
    return static_cast<uint16_t>(duty_cycle * MAX_ANALOG_WRITE);
}

inline uint16_t i_out_to_pwm(float i_out) {
    float target_voltage = 16.f * (10e-3f + 0.05f*i_out);
    //float target_voltage = 0.97f*i_out + 0.14f;
    float duty_cycle = max(0.f, min(1.f, target_voltage / PWM_P_TO_P));
    return static_cast<uint16_t>(duty_cycle * MAX_ANALOG_WRITE);
}

/* See:
 * http://arduino.stackexchange.com/questions/12718/increase-pwm-bit-resolution
 */

/* Configure digital pins 9 and 10 as 16-bit PWM outputs. */
inline void setupHiPWM() {
    DDRB |= _BV(PB1) | _BV(PB2);        /* set pins as outputs */
    TCCR1A = _BV(COM1A1) | _BV(COM1B1)  /* non-inverting PWM */
        | _BV(WGM11) | _BV(WGM10);      /* mode 14: fast PWM, TOP=0x03ff */
    TCCR1B = _BV(WGM12)
        | _BV(CS10);                    /* no prescaling */
}

/* High precision version of analogWrite(). Works only on pins 9 and 10. */
inline void analogWriteHi(uint8_t pin, uint16_t val)
{
    switch (pin) {
        case  9: OCR1A = val; break;
        case 10: OCR1B = val; break;
    }
}


void setup() {
    // Enable high-precision PWM output. We use this to allow for 16-bit control
    // of the current and voltage setting without resorting to tricks such as
    // voltage summing two 8-bit pins, etc.
    setupHiPWM();

    // Configure LCD display
    lcd.begin(16, 2);
    lcd.print("I'm alive!");

    // Test
    analogWrite16(PIN_IOUT_SET, i_out_to_pwm(0.3));
    analogWrite16(PIN_VOUT_SET, v_out_to_pwm(3.3));
}

void loop() {
}
