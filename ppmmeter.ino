/**
 * CO2 meter v.2
 * (c)2024 by Aleksandr.ru
 * @url http://aleksandr.ru
 */

#include <Wire.h>
#include <TM1637Display.h>
#include <SensirionI2CScd4x.h>
#include <Adafruit_BMP085.h>
#include <PMU.h> // https://github.com/dbuezas/lgt8fx/tree/master/lgt8f/libraries/PMU

#include "config.h"

const uint8_t SEG_START[] = {
    SEG_A | SEG_D | SEG_E | SEG_F,                  // C
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
    SEG_A | SEG_B | SEG_D | SEG_E | SEG_G,          // 2
    0x0                                             // _
};
const uint8_t SEG_DOTS[] = { SEG_DP, SEG_DP, SEG_DP, 0x0 }; // ..._

TM1637Display display(PIN_CLK, PIN_DIO);
SensirionI2CScd4x scd40;
Adafruit_BMP085 bmp180;

bool alarm = false;

void setup() {
    pinMode(PIN_PHR, INPUT);
    pinMode(PIN_PHPWR, OUTPUT);
    pinMode(PIN_LED, OUTPUT);

    #ifdef DEBUG
    Serial.begin(DEBUG);
    Serial.println("Starting up");
    #endif

    display.setBrightness(MAX_BRIGHT_D / 2);
    display.setSegments(SEG_START);

    Wire.begin();
    scd40.begin(Wire);
    while (!bmp180.begin(BMP085_ULTRALOWPOWER, &Wire)) {
      #ifdef DEBUG
      Serial.println("Could not find a valid BMP180 sensor");
      #endif
      display.showNumberHexEx(0xE1);
      #ifdef SKIP_ERRORS
      delay(DEBUG_DELAY);
      break;
      #endif
    }

    scd40.stopPeriodicMeasurement();
    uint16_t test = 0;
    do {
      scd40.performSelfTest(test);
      if (test != 0) {
        #ifdef DEBUG
        Serial.print("Self test failed with code ");
        Serial.println(test);
        #endif
        display.showNumberHexEx(0xE2);
        scd40.performFactoryReset();
      }
      else {
        #ifdef DEBUG
        Serial.println("Self test OK");
        #endif
      }
      #ifdef SKIP_ERRORS
      delay(DEBUG_DELAY);
      break;
      #endif
    } while (test != 0);

    while (scd40.startLowPowerPeriodicMeasurement()) {
      // return 0 on success, an error code otherwise
      display.showNumberHexEx(0xE3);
      #ifdef SKIP_ERRORS
      delay(DEBUG_DELAY);
      break;
      #endif
    }

    #ifdef DEBUG
    Serial.println("Setup comolete");
    delay(DEBUG_DELAY);
    #endif
}

void loop() {
    #ifdef DEBUG
    Serial.println("Go to sleep");
    delay(DEBUG_DELAY);
    #endif
    // https://github.com/dbuezas/lgt8fx/issues/155
    // https://github.com/dbuezas/lgt8fx/issues/155#issuecomment-1475204362
    bitClear(ADCSRA, ADEN);
    #ifdef DEBUG
    PMU.sleep(PM_POFFS0, SLEEP_8S);
    #else
    PMU.sleep(PM_POFFS0, SLEEP_32S);
    #endif
    bitSet(ADCSRA, ADEN);

    digitalWrite(PIN_PHPWR, HIGH);
    uint16_t light = analogRead(PIN_PHR);
    digitalWrite(PIN_PHPWR, LOW);
    #ifdef DEBUG
    Serial.print("Light level "); Serial.println(light);
    delay(DEBUG_DELAY);
    #endif
    light = constrain(light, MIN_BRIGHT_A, MAX_BRIGHT_A);

    byte brightness = map(light, MIN_BRIGHT_A, MAX_BRIGHT_A, MIN_BRIGHT_D, MAX_BRIGHT_D);
    #ifdef DEBUG
    Serial.print("Display brightness "); Serial.println(brightness);
    delay(DEBUG_DELAY);
    #endif
    // The setting takes effect when a command is given to change the data being displayed.
    display.setBrightness(brightness);

    int32_t pressure = bmp180.readPressure();
    if(pressure > 0) {
        #ifdef DEBUG
        Serial.print("Pressure "); Serial.print(pressure);
        Serial.print(" Pa = "); Serial.print(MMHG(pressure));
        Serial.print(" mmHg, temperature "); Serial.print(bmp180.readTemperature());
        Serial.print(" C, altitude "); Serial.print(bmp180.readAltitude());
        Serial.println(" m");
        delay(DEBUG_DELAY);
        #endif
        // @param ambientPressure Ambient pressure in hPa.
        // Convert value to Pa by: value * 100.
        scd40.setAmbientPressure(pressure / 100);
    }
    else {
        #ifdef DEBUG
        Serial.println("Failed to get pressure from BMP180");
        delay(DEBUG_DELAY);
        #endif
    }

    bool is_ready = false;
    uint16_t co2 = 0;
    float temperature = 0.0f;
    float humidity = 0.0f;
    scd40.getDataReadyFlag(is_ready);
    if (is_ready) {
        scd40.readMeasurement(co2, temperature, humidity);
        #ifdef DEBUG
        Serial.print("CO2 "); Serial.print(co2);
        Serial.print(" ppm, temperature "); Serial.print(temperature);
        Serial.print(" C, humidity "); Serial.print(humidity);
        Serial.println(" %");
        delay(DEBUG_DELAY);
        #endif

        if (co2 > 0) {
            #ifdef NIGHT_DIM
            if (light == MIN_BRIGHT_A) display.setSegments(SEG_DOTS);
            else
            #endif
            display.showNumberDec(co2, false);

            alarm = co2 > ALARM_THRESHOLD;
        }
        else {
            #ifdef DEBUG
            Serial.println("Invalid sample detected, skipping");
            delay(DEBUG_DELAY);
            #endif
        }
    }
    else {
        #ifdef DEBUG
        Serial.println("SCD40 is not ready");
        delay(DEBUG_DELAY);
        #else
        delay(500);
        #endif
    }

    //TODO: pwm is off while powerDown
    // byte pwm = map(light, MIN_BRIGHT_A, MAX_BRIGHT_A, MIN_BRIGHT_L, MAX_BRIGHT_L);
    // if (alarm) {
    //     #ifdef DEBUG
    //     Serial.print("Led PWM value "); Serial.println(pwm);
    //     delay(DEBUG_DELAY);
    //     #endif
    //     analogWrite(PIN_LED, pwm);
    // }
    // else {
    //     #ifdef DEBUG
    //     Serial.println("Led PWM off");
    //     delay(DEBUG_DELAY);
    //     #endif
    //     analogWrite(PIN_LED, 0);
    // }
    if (alarm) {
        digitalWrite(PIN_LED, HIGH);
    }
    else {
        digitalWrite(PIN_LED, LOW);
    }
}
