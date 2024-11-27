# DIY CO2 meter for home and office use v.2

![CO2 meter v2](https://aleksandr.ru/sitefiles/398/20241118_165211.jpg)

Real-time display of current CO2 level. Alarm indicator.
Automatic display brightness. Powered from accumulator. USB charging.

# Components

- LGT8F328P LQFP32 MiniEVB 5V pro-mini
- SCD40 CO2 sensor
- BMP180 pressure sensor
- TM1637 display
- TP4056 charging board
- 18650 accumulator
- SS12F15 G6 1P2T switch
- 5516 photoresistor and 10K resistor
- 3 mm. led and 330 Ohm resistor
- 7 M3x6 and 2 M2x4 screws
- Dupont wires

## 3D printed case

Get it from [my thingiverse](https://www.thingiverse.com/thing:6836694)

## Schematics

Display

```
D7 to TM1637 CLK
D8 to TM1637 DIO
```

Signal led

```
D9 to signal led
```

Light sensor

```
D10 to 5516 photoresistor
A0 between 5516 photoresistor and 10k resistor
GND to 10k resistor
```

I2C sensors

```
A4 to SDA of SCD40 and BMP180
A5 to SCL of SCD40 and BMP180
```

Power

```
5V and GND to TP4056 output
```

## More info

See at my website [in Russian](https://aleksandr.ru/blog/avtonomniy_izmeritel_co2)
or [translated to English](https://aleksandr-ru.translate.goog/blog/avtonomniy_izmeritel_co2?_x_tr_sl=ru&_x_tr_tl=en&_x_tr_hl=ru).
