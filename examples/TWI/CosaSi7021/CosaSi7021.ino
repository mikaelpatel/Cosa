/**
 * @file CosaSi7021.ino
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
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

#include <util/crc16.h>

static const uint8_t ADDR = 0x40;
TWI::Driver dev(ADDR);

uint8_t crc_update(uint8_t crc, uint8_t data)
{
  crc ^= data;
  for (uint8_t i = CHARBITS; i != 0; i--) {
    if (crc & 0x80)
      crc = (crc << 1) ^ 0x131;
    else
      crc = (crc << 1);
  }
  return (crc);
}

bool read(uint8_t cmd, uint8_t& value)
{
  uint8_t reg;
  int count;

  twi.begin(&dev);
  twi.write(cmd);
  count = twi.read(&reg, sizeof(reg));
  twi.end();
  if (count != sizeof(reg)) return (false);
  value = reg;
  return (true);
}

bool read(uint8_t cmd, uint16_t& value, bool check = true)
{
  uint8_t buf[3];
  int size;
  int count;

  size = check ? sizeof(buf) : sizeof(buf) - 1;
  twi.begin(&dev);
  twi.write(cmd);
  count = twi.read(buf, size);
  twi.end();
  if (count != size) return (false);
  if (check) {
    uint8_t crc;
    crc = crc_update(0, buf[0]);
    crc = crc_update(crc, buf[1]);
    if (crc != buf[2]) return (false);
  }
  value = ((buf[0] << 8) | buf[1]);
  return (true);
}

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaSi7021: started"));
  Watchdog::begin();
  RTC::begin();

  uint16_t xmd;
  uint8_t reg;
  uint8_t rev;
  uint8_t sna[8];
  uint8_t snb[6];
  int count;

  // Read user register
  ASSERT(read(0xE7, reg));
  trace << PSTR("user: ") << bin << reg << endl;

  // Read control register
  ASSERT(read(0x11, reg));
  trace << PSTR("control: ") << bin << reg << endl;

  // Read electronic serial number
  xmd = 0xFA0F;
  twi.begin(&dev);
  count = twi.write(xmd);
  twi.end();
  ASSERT(count == sizeof(xmd));
  twi.begin(&dev);
  count = twi.read(&sna, sizeof(sna));
  twi.end();
  ASSERT(count == sizeof(sna));
  trace << PSTR("sna: ");
  trace.print(sna, sizeof(sna), IOStream::hex);

  xmd = 0xFCC9;
  twi.begin(&dev);
  count = twi.write(xmd);
  twi.end();
  ASSERT(count == sizeof(xmd));
  twi.begin(&dev);
  count = twi.read(&snb, sizeof(snb));
  twi.end();
  ASSERT(count == sizeof(snb));
  trace << PSTR("snb: ");
  trace.print(snb, sizeof(snb), IOStream::hex);

  // Read software revision
  xmd = 0x84B8;
  twi.begin(&dev);
  count = twi.write(xmd);
  twi.end();
  ASSERT(count == sizeof(xmd));
  twi.begin(&dev);
  count = twi.read(&rev, sizeof(rev));
  twi.end();
  ASSERT(count == sizeof(rev));
  trace << PSTR("rev: ") << hex << rev << endl;
}

void loop()
{
  uint16_t value;

  // Read humidity
  ASSERT(read(0xE5, value));
  // int16_t humidity = ((12500L * value) >> 16) - 600;
  float humidity = ((125.00 * value) / 65536) - 6.00;
  trace << humidity << PSTR("% RH, ");

  // Temperature from humidity read
  // ASSERT(read(0xE0, value, false));
  // Read temperature
  ASSERT(read(0xE3, value));
  // int16_t temp = ((17572L * value) >> 16) - 4685;
  // float temp = ((175.72 * value) / 65536) - 46.85;
  float temp = ((175.72 * value) / 65536) - (46.85 + 70.0);
  trace << temp << PSTR("° C") << endl;

  sleep(2);
}
