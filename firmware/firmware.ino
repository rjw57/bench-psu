#include <LiquidCrystal.h>

#include "AnalogWrite10.h"
#include "pins.h"

// Parameters for mapping desired voltage to 10-bit PWM output. Voltages are
// specified in mV.
const int V_SLOPE_RECIPROCAL = 62;
const int V_INTERCEPT = 9;
const int V_PWM_MAX = 490; // never send more than this as PWM output

// Parameters for mapping deisrec current to 10-bit PWM output. Currents are
// specified in mA
const int I_SLOPE = 92;
const int I_INTERCEPT = 24;
const int I_PWM_MAX = 600; // never send more than this as PWM output

inline uint16_t mv_out_to_pwm(int mv_out) {
    return max(0, min(V_PWM_MAX, V_INTERCEPT + (mv_out / V_SLOPE_RECIPROCAL)));
}

inline uint16_t mi_out_to_pwm(float mi_out) {
    return max(0, min(I_PWM_MAX, I_INTERCEPT + (mi_out * I_SLOPE) / 1000));
}

// Liquid crystal display
LiquidCrystal lcd(
    PIN_LCD_RS, PIN_LCD_E, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7
);

class RotaryEncoder {
    public:
        enum Direction {
            DIR_CLOCK, DIR_COUNTER_CLOCK, DIR_NONE
        };

        RotaryEncoder(int clk, int dt) : RotaryEncoder(clk, dt, -1) { }
        RotaryEncoder(int clk, int dt, int sw)
            : clk_pin_(clk), dt_pin_(dt), sw_pin_(sw)
        {
            pinMode(clk_pin_, INPUT);
            pinMode(dt_pin_, INPUT);
            if(sw_pin_ >= 0) {
                pinMode(sw_pin_, INPUT);
            }

            last_clk_ = digitalRead(clk_pin_);
        }

        Direction poll_direction() {
            int clk, dt;
            Direction ret_val = DIR_NONE;

            clk = digitalRead(clk_pin_);
            if(clk != last_clk_) {
                dt = digitalRead(dt_pin_);
                if(dt != clk) {
                    ret_val = DIR_CLOCK;
                } else {
                    ret_val = DIR_COUNTER_CLOCK;
                }
            }

            last_clk_ = clk;
            return ret_val;
        }

    private:
        int clk_pin_, dt_pin_, sw_pin_;
        int last_clk_;
};

RotaryEncoder re(PIN_RE_CLK, PIN_RE_DT, PIN_RE_SW);
int counter = 0;

void setup() {
    // Enable high-precision PWM output. We use this to allow for 16-bit control
    // of the current and voltage setting without resorting to tricks such as
    // voltage summing two 8-bit pins, etc.
    setupPWM10();

    // Test
    analogWrite10(PIN_IOUT_SET, mi_out_to_pwm(500));
    analogWrite10(PIN_VOUT_SET, mv_out_to_pwm(3300));
}

void loop() {
}

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
