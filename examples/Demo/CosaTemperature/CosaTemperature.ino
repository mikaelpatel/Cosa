/**
 * @file CosaTemperature.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * IOStream::Device, and usage of off-screen canvas.
 *
 * @section Circuit
 * Connect Arduino to DS18B20 in D5 and GND. Parasite powering.
 *
 * Connect Arduino to PCD8544 (Arduino => PCD8544):
 * D6 ==> SDIN, D7 ==> SCLK, D8 ==> DC, D9 ==> SCE.
 * RST ==> RST.
 * 
 * The PCD8544 should be connect using 3.3 V signals and VCC. 
 * Back-light should be max 3.3 V. Reduce voltage with 100-500 ohm 
 * resistor to ground.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/FixedPoint.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/IOStream/Driver/PCD8544.hh"
#include "Cosa/Canvas/OffScreen.hh"
#include "Cosa/Canvas/Font/FixedNums8x16.hh"
#include "Cosa/Canvas/Icon/arduino_icon_64x32.h"
#include "Cosa/Canvas/Element/Textbox.hh"

OWI owi(Board::D5);
DS18B20 sensor(&owi);
PCD8544 lcd;
#undef putchar

void setup()
{
  // Set up watchdog for low power sleep
  Watchdog::begin();

  // Initiate the LCD screen and show arduino icon
  lcd.begin(0x38);
  lcd.putchar('\f');
  lcd.set_cursor((lcd.WIDTH - 64)/2, 1);
  lcd.draw_icon(arduino_icon_64x32);
  SLEEP(2);

  // Use LCD bind to trace
  trace.begin(&lcd);
  trace << PSTR("\fCosaPCD8544: started\n");
  SLEEP(2);
  trace << '\f';
  TRACE(sensor.connect(0));
  TRACE(sensor.read_power_supply());
  SLEEP(3);

  // Pipeline the conversion requests from the sensor
  sensor.convert_request();
  SLEEP(1);
  sensor.power_off();
 }

void loop()
{
  // Request a new sample from the sensor and read temperature
  sensor.read_scratchpad();
  sensor.convert_request();
  SLEEP(1);
  sensor.power_off();

  // Get temperature and convert from fixed point (could use float)
  FixedPoint temp(sensor.get_temperature(), 4);
  int16_t integer = temp.get_integer();
  uint16_t fraction = temp.get_fraction(2);

  // Create an offscreen bitmap for the presentation
  uint8_t buffer[PCD8544::WIDTH * PCD8544::HEIGHT / CHARBITS];
  OffScreen offscreen(PCD8544::WIDTH, PCD8544::HEIGHT, buffer);

  // Use a textbox for the font and position
  Textbox textbox(&offscreen);
  IOStream console(&textbox);
  textbox.set_cursor(21, 17);
  textbox.set_text_font(&fixednums8x16);

  // Draw the offscreen bitmap with the temperature
  offscreen.begin();
  offscreen.draw_roundrect(8, 8, lcd.WIDTH - 16, lcd.HEIGHT - 16, 8);
  console << integer << '.'; 
  if (fraction < 10) console << '0';
  console << fraction;  

  // Draw the bitmap to the LCD screen
  lcd.putchar('\f');
  lcd.draw_bitmap(offscreen.get_bitmap(), offscreen.WIDTH, offscreen.HEIGHT);
  SLEEP(1);
}
