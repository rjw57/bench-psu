#pragma once
#ifndef PINS__H
#define PINS__H

// Pins for rotary encoder switch
const int PIN_RE_SW = 6;
const int PIN_RE_CLK = 7;
const int PIN_RE_DT = 8;

// Pins for LCD module
const int PIN_LCD_RS = A0;
const int PIN_LCD_E = A1;
const int PIN_LCD_D4 = 2;
const int PIN_LCD_D5 = 3;
const int PIN_LCD_D6 = 4;
const int PIN_LCD_D7 = 5;

// Output pins to D3806 module
const int PIN_IOUT_SET = 9;
const int PIN_VOUT_SET = 10;
const int PIN_NOT_ON = A7;

// Input pins from D3806
const int PIN_CC_CV = A5;
const int PIN_IOUT_SENSE = A6;
const int PIN_VOUT_SENSE = A7;

#endif // PINS__H
