/**
 * @file CosaTemperature.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Demonstration of the PCD8544 device driver with mapping to
 * IOStream::Device, and off-screen canvas. Uses 1-wire temperature 
 * sensor, DS18B20.
 *
 * @section Circuit
 * Connect Arduino to DS18B20 in D5 and GND. May use parasite 
 * powering (connect DS18B20 VCC to GND) otherwise to VCC.
 *
 * Connect Arduino to PCD8544 (Arduino => PCD8544):
 * D6 ==> SDIN, D7 ==> SCLK, D8 ==> DC, D9 ==> SCE.
 * RST ==> RST.
 * 
 * The PCD8544 should be connect using 3.3 V signals and VCC. 
 * Back-light should be max 3.3 V. Reduce voltage with 100-500 ohm 
 * resistor to ground or use a 3.3 V Arduino.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/LCD/Driver/PCD8544.hh"
#include "Cosa/Canvas/OffScreen.hh"
#include "Cosa/Canvas/Font/FixedNums8x16.hh"
#include "Cosa/Canvas/Icon/arduino_icon_64x32.h"
#include "Cosa/Canvas/Element/Textbox.hh"

OWI owi(Board::D5);
DS18B20 sensor(&owi);
PCD8544 lcd;

void setup()
{
  // Set up watchdog for low power sleep
  Watchdog::begin();

  // Initiate the LCD screen and show splash screen with arduino icon
  lcd.begin();
  lcd.set_cursor((lcd.WIDTH - 64)/2, 1);
  lcd.draw_icon(arduino_icon_64x32);
  SLEEP(2);

  // Use LCD bind to trace; print some startup information
  trace.begin(&lcd);
  trace << PSTR("\fCosaTemperature: started\n");
  SLEEP(2);
  trace << clear;
  TRACE(sensor.connect(0));
  TRACE(sensor.read_power_supply());
  SLEEP(2);
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
  lcd.draw_bitmap(offscreen.get_bitmap(), offscreen.WIDTH, offscreen.HEIGHT);

  // Take a nap
  SLEEP(2);
}
