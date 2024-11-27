/**
 * CO2 meter v.2
 * (c)2024 by Aleksandr.ru
 * @url http://aleksandr.ru
 */

// #define DEBUG 115200
// #define SKIP_ERRORS
#ifdef DEBUG
#define DEBUG_DELAY 700
#endif

// #define NIGHT_DIM // turn display into dots when dark

// Analog input light level:
// 5516 photoresistor + 10k resistor
#define MIN_BRIGHT_A 250
#define MAX_BRIGHT_A 1000

// Display brightness:
// A number from 0 (lowes brightness) to 7 (highest brightness)
#ifdef NIGHT_DIM
#define MIN_BRIGHT_D 0
#define MAX_BRIGHT_D 2
#else // tint film 15% on display
#define MIN_BRIGHT_D 0
#define MAX_BRIGHT_D 4
#endif

// Led brighness (pwm):
// 3mm led with 330ohm resistor
// #define MIN_BRIGHT_L 50
// #define MAX_BRIGHT_L 255

#define ALARM_THRESHOLD 999 // co2 level
#define MMHG(pa) pa / 133.32239

#define PIN_PHR A0 // photoresistor
#define PIN_PHPWR 10 // photo power
#define PIN_LED 9 // red led

#define PIN_CLK 7 // display
#define PIN_DIO 8 // display

// SEG_DP added after TM1637 v1.2.0
#ifndef SEG_DP
#define SEG_DP 0b10000000
#endif
