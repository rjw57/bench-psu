/* Provide 10-bit resolution on output PWM pins 9 and 10. */

#pragma once
#ifndef ANALOG_WRITE_10__H
#define ANALOG_WRITE_10__H

/* See:
 * http://arduino.stackexchange.com/questions/12718/increase-pwm-bit-resolution
 */

/* Configure digital pins 9 and 10 as 10-bit PWM outputs. */
inline void setupPWM10() {
    DDRB |= _BV(PB1) | _BV(PB2);        /* set pins as outputs */
    TCCR1A = _BV(COM1A1) | _BV(COM1B1)  /* non-inverting PWM */
        | _BV(WGM11) | _BV(WGM10);      /* mode 14: fast PWM, TOP=0x03ff */
    TCCR1B = _BV(WGM12)
        | _BV(CS10);                    /* no prescaling */
}

/* High precision version of analogWrite(). Works only on pins 9 and 10. */
inline void analogWrite10(uint8_t pin, uint16_t val)
{
    switch (pin) {
        case  9: OCR1A = val; break;
        case 10: OCR1B = val; break;
    }
}

#endif // ANALOG_WRITE_10__H
