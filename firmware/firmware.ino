#include "AnalogWrite16.h"

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

const int PIN_VOUT_SET = 9;
const int PIN_IOUT_SET = 10;

const float SUPPLY_VOLTAGE = 5.f - 0.4f; // Minus 400mV from BAT85 diode

// TODO: These formulae are taken directly from the reverse engineering of the
// B3603. They need verifying.

inline uint16_t v_out_to_pwm(float v_out) {
    float target_voltage = (16.f/15.f) * (31e-3f + 0.068f*v_out);
    float duty_cycle = max(0.f, min(1.f, target_voltage / SUPPLY_VOLTAGE));
    return static_cast<uint16_t>(duty_cycle * 65535.f);
}

inline uint16_t i_out_to_pwm(float i_out) {
    float target_voltage = 16.f * (10e-3f + 0.05f*i_out);
    float duty_cycle = max(0.f, min(1.f, target_voltage / SUPPLY_VOLTAGE));
    return static_cast<uint16_t>(duty_cycle * 65535.f);
}

void setup() {
    // Enable high-precision PWM output. We use this to allow for 16-bit control
    // of the current and voltage setting without resorting to tricks such as
    // voltage summing two 8-bit pins, etc.
    setupPWM16();

    // Test
    analogWrite16(PIN_IOUT_SET, i_out_to_pwm(20e-3));
    analogWrite16(PIN_VOUT_SET, v_out_to_pwm(5.0));
}

void loop() {
}
