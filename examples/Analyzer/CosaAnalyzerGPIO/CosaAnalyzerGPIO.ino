/**
 * @file CosaAnalyzerGPIO.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * @section Description
 * Logic Analyzer based performance measurement of CosaGPIO.
 * Measurements are for Arduino Uno/Nano (Cosa 1.1.3).
 *
 * @section Circuit
 * Trigger on CHAN0/D13/LED rising.
 *
 * +-------+
 * | CHAN0 |-------------------------------> ledPin(LED/D13)
 * | CHAN1 |-------------------------------> outPin(D7);
 * | CHAN2 |-------------------------------> dataPin(D8);
 * | CHAN3 |-------------------------------> clockPin(D9);
 * |       |
 * | GND   |-------------------------------> GND
 * +-------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/GPIO.hh"
#include "Cosa/Math.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

GPIO outPin(Board::D7, GPIO::OUTPUT_MODE);
GPIO dataPin(Board::D8, GPIO::OUTPUT_MODE);
GPIO clockPin(Board::D9, GPIO::OUTPUT_MODE);
GPIO ledPin(Board::LED, GPIO::OUTPUT_MODE);
uint8_t data;

void setup()
{
  // Print short info about the logic analyser probe channels
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAnalyzerGPIO: started"));
  trace << PSTR("CHAN0 - D13/LED [^]") << endl;
  trace << PSTR("CHAN1 - D7 (out)") << endl;
  trace << PSTR("CHAN2 - D8 (data)") << endl;
  trace << PSTR("CHAN3 - D9 (clock)") << endl;
  trace.flush();

  // Initial data
  data = rand(255);

  // Allow some time to start logic analyzer trigger
  sleep(2);

  // Trigger sampling
  ~ledPin;
  DELAY(50);
  ~ledPin;
  DELAY(50);
}

void loop()
{
  // Measure overhead of test case marker;
  // 0.5 us per pulse
  ~ledPin;
  ~ledPin;
  DELAY(10);

  // Measure four pulses using assignment operator;
  // 7.675 us (1.8 us per pulse)
  ~ledPin;
  outPin = 1;
  outPin = 0;
  outPin = 1;
  outPin = 0;
  outPin = 1;
  outPin = 0;
  outPin = 1;
  outPin = 0;
  ~ledPin;
  DELAY(10);

  // Measure four pulses using toggle operator;
  // 4.5 us (1 us per pulse)
  ~ledPin;
  ~outPin;
  ~outPin;
  ~outPin;
  ~outPin;
  ~outPin;
  ~outPin;
  ~outPin;
  ~outPin;
  ~ledPin;
  DELAY(10);

  // Measure clocked serial transfer of a byte using assignment operator;
  // 28.1875 us (3.46 us per bit)
  ~ledPin;
  for(uint8_t bit = 0x80; bit != 0; bit >>= 1) {
    dataPin = (data & bit);
    clockPin = 1;
    clockPin = 0;
  }
  ~ledPin;
  DELAY(10);

  // Measure clocked serial transfer of a byte using assignment
  // operator with const value;
  // 28.0625 us (3.45 us per bit)
  ~ledPin;
  for(uint8_t bit = 0x80; bit != 0; bit >>= 1) {
    if (data & bit) dataPin = 1; else dataPin = 0;
    clockPin = 1;
    clockPin = 0;
  }
  ~ledPin;
  DELAY(10);

  // Measure clocked serial transfer of a byte using assignment operator
  // and toggle operator;
  // 21.6875 us (2.63 us per bit)
  ~ledPin;
  for(uint8_t bit = 0x80; bit != 0; bit >>= 1) {
    dataPin = (data & bit);
    ~clockPin;
    ~clockPin;
  }
  ~ledPin;
  DELAY(10);

  // Measure clocked serial transfer of a byte using assignment
  // operator with const value and toggle operator;
  // 21.5625 us (2.33 us per bit)
  ~ledPin;
  for(uint8_t bit = 0x80; bit != 0; bit >>= 1) {
    if (data & bit) dataPin = 1; else dataPin = 0;
    ~clockPin;
    ~clockPin;
  }
  ~ledPin;
  DELAY(10);

  // Measure clocked serial transfer of a byte using assignment operator
  // and toggle operator fully unrolled
  // 19.000 us (2.31 us per bit)
  ~ledPin;
  dataPin = (data & 0x80);
  ~clockPin;
  ~clockPin;
  dataPin = (data & 0x40);
  ~clockPin;
  ~clockPin;
  dataPin = (data & 0x20);
  ~clockPin;
  ~clockPin;
  dataPin = (data & 0x10);
  ~clockPin;
  ~clockPin;
  dataPin = (data & 0x08);
  ~clockPin;
  ~clockPin;
  dataPin = (data & 0x04);
  ~clockPin;
  ~clockPin;
  dataPin = (data & 0x02);
  ~clockPin;
  ~clockPin;
  dataPin = (data & 0x01);
  ~clockPin;
  ~clockPin;
  ~ledPin;
  DELAY(10);

  // Measure clocked serial transfer of a byte using assignment operator
  // of const value and toggle operator fully unrolled
  // 18.5000 us (2.25 us per bit)
  ~ledPin;
  if (data & 0x80) dataPin = 1; else dataPin = 0;
  ~clockPin;
  ~clockPin;
  if (data & 0x40) dataPin = 1; else dataPin = 0;
  ~clockPin;
  ~clockPin;
  if (data & 0x20) dataPin = 1; else dataPin = 0;
  ~clockPin;
  ~clockPin;
  if (data & 0x10) dataPin = 1; else dataPin = 0;
  ~clockPin;
  ~clockPin;
  if (data & 0x08) dataPin = 1; else dataPin = 0;
  ~clockPin;
  ~clockPin;
  if (data & 0x04) dataPin = 1; else dataPin = 0;
  ~clockPin;
  ~clockPin;
  if (data & 0x02) dataPin = 1; else dataPin = 0;
  ~clockPin;
  ~clockPin;
  if (data & 0x01) dataPin = 1; else dataPin = 0;
  ~clockPin;
  ~clockPin;
  ~ledPin;
  DELAY(10);

  // Measure four pulses using static write member function;
  // 1.5 us (250 ns per pulse)
  ~ledPin;
  GPIO::write(Board::D8, 1);
  GPIO::write(Board::D8, 0);
  GPIO::write(Board::D8, 1);
  GPIO::write(Board::D8, 0);
  GPIO::write(Board::D8, 1);
  GPIO::write(Board::D8, 0);
  GPIO::write(Board::D8, 1);
  GPIO::write(Board::D8, 0);
  ~ledPin;
  DELAY(10);

  // Measure four pulses using static toggle member function
  // 1.0 us (125 ns per pulse)
  ~ledPin;
  GPIO::toggle(Board::D8);
  GPIO::toggle(Board::D8);
  GPIO::toggle(Board::D8);
  GPIO::toggle(Board::D8);
  GPIO::toggle(Board::D8);
  GPIO::toggle(Board::D8);
  GPIO::toggle(Board::D8);
  GPIO::toggle(Board::D8);
  ~ledPin;
  DELAY(10);

  // Measure clocked serial transfer of a byte using static write
  // member function
  // 10.8125 us (1.29 us per bit)
  ~ledPin;
  for(uint8_t bit = 0x80; bit != 0; bit >>= 1) {
    GPIO::write(Board::D9, data & bit);
    GPIO::write(Board::D10, 1);
    GPIO::write(Board::D10, 0);
  }
  ~ledPin;
  DELAY(10);

  // Measure clocked serial transfer of a byte using static write
  // member function with const value
  // 8.5625 us (1.01 us per bit)
  ~ledPin;
  for(uint8_t bit = 0x80; bit != 0; bit >>= 1) {
    if (data & bit) GPIO::write(Board::D9, 1); else GPIO::write(Board::D9, 0);
    GPIO::write(Board::D10, 1);
    GPIO::write(Board::D10, 0);
  }
  ~ledPin;
  DELAY(10);

  // Measure clocked serial transfer of a byte using static write
  // and toggle member functions
  // 9.8125 us (1.16 us per bit)
  ~ledPin;
  for(uint8_t bit = 0x80; bit != 0; bit >>= 1) {
    GPIO::write(Board::D9, data & bit);
    GPIO::toggle(Board::D10);
    GPIO::toggle(Board::D10);
  }
  ~ledPin;
  DELAY(10);

  // Measure clocked serial transfer of a byte using static write
  // with const value and toggle member functions
  // 7.9375 us (0.93 us per bit)
  ~ledPin;
  for(uint8_t bit = 0x80; bit != 0; bit >>= 1) {
    if (data & bit) GPIO::write(Board::D9, 1); else GPIO::write(Board::D9, 0);
    GPIO::toggle(Board::D10);
    GPIO::toggle(Board::D10);
  }
  ~ledPin;
  DELAY(10);

  // Measure clocked serial transfer of a byte using static write
  // and toggle member functions fully unrolled
  // 7.1250 us (0.83 us per bit)
  ~ledPin;
  GPIO::write(Board::D9, data & 0x80);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  GPIO::write(Board::D9, data & 0x40);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  GPIO::write(Board::D9, data & 0x20);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  GPIO::write(Board::D9, data & 0x10);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  GPIO::write(Board::D9, data & 0x08);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  GPIO::write(Board::D9, data & 0x04);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  GPIO::write(Board::D9, data & 0x02);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  GPIO::write(Board::D9, data & 0x01);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  ~ledPin;
  DELAY(10);

  // Measure clocked serial transfer of a byte using static write
  // and toggle member functions fully unrolled
  // 5.3125 us (0.60 us per bit)
  ~ledPin;
  if (data & 0x80) GPIO::write(Board::D9, 1); else GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  if (data & 0x40) GPIO::write(Board::D9, 1); else GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  if (data & 0x20) GPIO::write(Board::D9, 1); else GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  if (data & 0x10) GPIO::write(Board::D9, 1); else GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  if (data & 0x08) GPIO::write(Board::D9, 1); else GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  if (data & 0x04) GPIO::write(Board::D9, 1); else GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  if (data & 0x02) GPIO::write(Board::D9, 1); else GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  if (data & 0x01) GPIO::write(Board::D9, 1); else GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  ~ledPin;

  // New data value
  data = rand(255);
  sleep(1);
}

