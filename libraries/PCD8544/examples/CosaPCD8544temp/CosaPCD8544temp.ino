/**
 * @file CosaPCD8544temp.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Demonstration of the PCD8544 device driver with mapping to
 * IOStream::Device, and off-screen canvas. Uses 1-wire temperature
 * sensor, DS18B20.
 *
 * @section Circuit
 * PCD8544 is a low voltage device (3V3) and signals require level
 * shifter (74HC4050 or 10K resistor).
 * @code
 *                    (1) PCD8544/LCD::Serial3W
 *                        +------------+
 * (RST)----------[ > ]-1-|RST         |
 * (D9/D3)--------[ > ]-2-|CE          |
 * (D8/D2)--------[ > ]-3-|DC          |
 * (D6/D0)--------[ > ]-4-|DIN         |
 * (D7/D1)--------[ > ]-5-|CLK         |
 * (3V3)----------------6-|VCC         |
 * (GND)----------[220]-7-|LED         |
 * (GND)----------------8-|GND         |
 *                        +------------+
 *
 *                    (2) PCD8544/LCD::SPI3W
 *                        +------------+
 * (RST)---------[ > ]--1-|RST         |
 * (D9/D3)-------[ > ]--2-|CE          |
 * (D8/D2)-------[ > ]--3-|DC          |
 * (MOSI/D11/D5)-[ > ]--4-|DIN         |
 * (SCK/D13/D4)--[ > ]--5-|CLK         |
 * (3V3)----------------6-|VCC         |
 * (GND)---------[220]--7-|LED         |
 * (GND)----------------8-|GND         |
 *                        +------------+
 *
 *                        DS18B20/sensor
 * (VCC)--[4K7]--+        +------------+
 * (GND)---------(------1-|GND         |\
 * (D5)----------+------2-|DQ          | |
 * (VCC/GND)----------+-3-|VDD         |/
 *                        +------------+
 * @endcode
 *
 * Connect Arduino to DS18B20 in D5 and GND. May use parasite
 * powering (connect DS18B20 VCC to GND) otherwise to VCC.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <OWI.h>
#include <DS18B20.h>

#include <Canvas.h>
#include "Canvas/OffScreen.hh"
#include "Canvas/Icon/arduino_icon_64x32.h"
#include "Canvas/Element/Textbox.hh"

#include <Font.h>
#include "FixedNums8x16.hh"

#include <PCD8544.h>

#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"

// One-Wire and digital thermometer
OWI owi(Board::D5);
DS18B20 sensor(&owi);

// Select PCD8544 IO Adapter; Serial Output Pins or SPI
LCD::Serial3W port;
// LCD::SPI3W port;
PCD8544 lcd(&port);

void setup()
{
  // Set up watchdog for low power sleep
  Watchdog::begin();

  // Initiate the LCD screen and show splash screen with arduino icon
  lcd.begin();
  lcd.set_cursor((lcd.WIDTH - 64)/2, 1);
  lcd.draw_icon(arduino_icon_64x32);
  sleep(2);

  // Use LCD bind to trace; print some startup information
  trace.begin(&lcd);
  trace << PSTR("\fCosaPCD8544temp: started\n");
  sleep(2);
  trace << clear;
  TRACE(sensor.connect(0));
  TRACE(sensor.read_power_supply());
  sleep(2);
  trace << clear;
  trace << (OWI::Driver&) sensor;
  sleep(2);
 }

void loop()
{
  // Request a new sample from the sensor, Read temperature later on
  sensor.convert_request();

  // Create an offscreen bitmap for the presentation
  OffScreen<PCD8544::WIDTH, PCD8544::HEIGHT> offscreen;

  // Use a textbox for the font and position
  Textbox textbox(&offscreen);
  IOStream console(&textbox);
  textbox.set_cursor(21, 17);
  textbox.set_text_font(&fixednums8x16);

  // Draw the offscreen bitmap with the temperature
  offscreen.begin();
  offscreen.draw_roundrect(8, 8, lcd.WIDTH - 16, lcd.HEIGHT - 16, 8);

  // Read temperature and print to offscreen bitmap
  sensor.read_scratchpad();
  console << sensor;

  // Draw the bitmap to the LCD screen
  lcd.set_cursor(0, 0);
  lcd.draw_bitmap(offscreen.bitmap(), offscreen.WIDTH, offscreen.HEIGHT);

  // Take a nap
  sleep(2);
}
