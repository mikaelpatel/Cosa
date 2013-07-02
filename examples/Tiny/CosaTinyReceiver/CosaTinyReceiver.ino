/**
 * @file CosaTinyReceiver.ino
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
 * @section Description
 * Demonstration of the PCD8544 device driver with mapping to 
 * IOStream::Device for the ATTINYX5. Receive messages from 
 * CosaVWIsender (RF433/Virtual Wire) and display on LCD/PCD8544.
 * 
 * @section Circuit
 * Connect ATtinyX5 to PCD8544 (ATtinyX5 => PCD8544):
 * D0 ==> SDIN, D1 ==> SCLK, D2 ==> DC, D3 ==> SCE.
 * Connect ATtinyX5 to RF433 Receiver: D4 ==> DATA.
 * 
 * The PCD8544 should be connect using 3.3 V signals and VCC. 
 * Back-light should be max 3.3 V. Reduce voltage with 100-500 ohm 
 * resistor to ground.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/LCD/Driver/PCD8544.hh"
#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"

PCD8544 lcd;

VirtualWireCodec codec;
VWI::Receiver rx(Board::D4, &codec);
const uint16_t SPEED = 4000;

void setup()
{
  // Start watchdog and realtime clock
  Watchdog::begin();
  RTC::begin();

  // Start virtual wire interface and receiver
  VWI::begin(SPEED);
  rx.begin();

  // Start LCD and bind trace output to display
  lcd.begin();
  trace.begin(&lcd);
  trace << PSTR("\fCosaTinyReceiver: started");
  SLEEP(2);
}

// Message to receive
struct msg_t {
  uint16_t nr;
  uint16_t luminance;
  uint16_t temperature;
};

void loop()
{
  // Count some statistics
  static uint16_t cnt = 0;
  static uint16_t err = 0;
  static uint16_t nr = 0;

  // Receive message and sanity check
  msg_t msg;
  int8_t len = rx.recv(&msg, sizeof(msg));
  if (len != sizeof(msg)) return;

  // Check for dropped messages
  if (msg.nr != nr) err += 1;

  // Update statistics
  cnt += 1;
  nr = msg.nr + 1;

  // Display received message and statistics
  uint16_t vcc = AnalogPin::bandgap(1100);
  uint16_t ert = (err * 100L) / cnt;
  uint32_t s = Watchdog::millis() / 1000L;
  uint16_t min = s / 60L;
  uint16_t sec = s % 60L;
  lcd.set_cursor(0,5);
  trace << endl;
  trace << msg.nr << ':'
	<< msg.luminance << PSTR(", ")
	<< msg.temperature << endl;
  trace << PSTR("cnt = ") << cnt << endl;
  trace << PSTR("err = ") << err << endl;
  trace << PSTR("err% = ") << ert << '%' << endl;
  trace << PSTR("Vcc = ") << vcc << PSTR(" mV") << endl;
  trace << PSTR("T = ") << min << ':';
  if (sec < 10) trace << '0';
  trace << sec;
}

