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
 * Logic Analyzer Cosa GPIO; trigger on rising edge transition
 * on ledPin. Measurement values are for Arduino Uno
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/GPIO.hh"
#include "Cosa/Math.hh"

GPIO outPin(Board::D8, GPIO::OUTPUT_MODE);
GPIO dataPin(Board::D9, GPIO::OUTPUT_MODE);
GPIO clockPin(Board::D10, GPIO::OUTPUT_MODE);
GPIO ledPin(Board::LED, GPIO::OUTPUT_MODE);
uint8_t data;

void setup()
{
  // Initial pin state
  outPin = 0;
  dataPin = 0;
  clockPin = 0;
  ledPin = 0;

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
  // Measure overhead in test case marker;
  // 0.5 us
  ~ledPin;
  ~ledPin;
  DELAY(10);

  // Measure four pulses using assignment operator;
  // 4.5 us (1 us per pulse)
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
  // 23.375 us (2.86 us per bit)
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
  // 21.250 us (2.59 us per bit)
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
  // 21.875 us (2.67 us per bit)
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
  // 19.21 us (2.33 us per bit)
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
  // 19.208 us (2.33 us per bit)
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
  // 16.125 us (1.95 us per bit)
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
  // 10.875 us (1.29 us per bit)
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
  // 8.375 us (0.98 us per bit)
  ~ledPin;
  for(uint8_t bit = 0x80; bit != 0; bit >>= 1) {
    if (data & bit)
      GPIO::write(Board::D9, 1);
    else
      GPIO::write(Board::D9, 0);
    GPIO::write(Board::D10, 1);
    GPIO::write(Board::D10, 0);
  }
  ~ledPin;
  DELAY(10);

  // Measure clocked serial transfer of a byte using static write
  // and toggle member functions
  // 9.875 us (1.17 us per bit)
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
  // 7.750 us (0.91 us per bit)
  ~ledPin;
  for(uint8_t bit = 0x80; bit != 0; bit >>= 1) {
    if (data & bit)
      GPIO::write(Board::D9, 1);
    else
      GPIO::write(Board::D9, 0);
    GPIO::toggle(Board::D10);
    GPIO::toggle(Board::D10);
  }
  ~ledPin;
  DELAY(10);

  // Measure clocked serial transfer of a byte using static write
  // and toggle member functions fully unrolled
  // 7.167 us (0.83 us per bit)
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
  // 5.250 us (0.59 us per bit)
  ~ledPin;
  if (data & 0x80)
    GPIO::write(Board::D9, 1);
  else
    GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  if (data & 0x40)
    GPIO::write(Board::D9, 1);
  else
    GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  if (data & 0x20)
    GPIO::write(Board::D9, 1);
  else
    GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  if (data & 0x10)
    GPIO::write(Board::D9, 1);
  else
    GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  if (data & 0x08)
    GPIO::write(Board::D9, 1);
  else
    GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  if (data & 0x04)
    GPIO::write(Board::D9, 1);
  else
    GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  if (data & 0x02)
    GPIO::write(Board::D9, 1);
  else
    GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  if (data & 0x01)
    GPIO::write(Board::D9, 1);
  else
    GPIO::write(Board::D9, 0);
  GPIO::toggle(Board::D10);
  GPIO::toggle(Board::D10);
  ~ledPin;

  // New data value
  data = rand(255);
  sleep(1);
}

