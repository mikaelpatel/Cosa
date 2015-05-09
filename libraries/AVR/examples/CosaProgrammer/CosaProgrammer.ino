/**
 * @file CosaProgrammer.hh
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
 * Demonstration of the Cosa AVR Programmer class. Allows serial
 * programming of AVR devices over SPI. Implements the full
 * instruction specification. Used to implement the Cosa ISP.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <AVR.h>

#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

Programmer isp;

void setup()
{
  uint8_t buf[64];

  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaProgrammer: started"));
  Watchdog::begin();

  ASSERT(isp.begin());

#ifdef __CHIP_ERASE__
  isp.chip_erase();
#endif

  trace << PSTR("poll:") << hex << isp.isbusy() << endl;
  trace << PSTR("lock:") << hex << isp.read_lock_bits() << endl;
  trace << PSTR("signature:")
	<< hex << isp.read_signature_byte(0) << ':'
	<< hex << isp.read_signature_byte(1) << ':'
	<< hex << isp.read_signature_byte(2) << endl;
  trace << PSTR("fuse:")
	<< hex << isp.read_fuse_bits() << ':'
	<< hex << isp.read_fuse_high_bits() << ':'
	<< hex << isp.read_extended_fuse_bits() << endl;
  trace << PSTR("calibration:") << hex << isp.read_calibration_byte() << endl;

  isp.read_eeprom_memory(buf, 0x01fc, 4);
  trace.print(buf, 4, IOStream::hex);

  isp.write_eeprom_memory(0x01fc, 0xde);
  isp.write_eeprom_memory(0x01fd, 0xad);
  isp.write_eeprom_memory(0x01fe, 0xbe);
  isp.write_eeprom_memory(0x01ff, 0xef);
  isp.read_eeprom_memory(buf, 0x01fc, 4);
  trace.print(buf, 4, IOStream::hex);


  for (uint16_t src = 0; src < 1024; src += sizeof(buf) / 2) {
    isp.read_program_memory(buf, src, sizeof(buf));
    trace.print(buf, sizeof(buf), IOStream::hex);
  }
  trace << endl;

#ifdef __PROGRAM_MEMORY__
  memset(buf, 0xa5, sizeof(buf));
  isp.write_program_memory(0, buf, sizeof(buf) - 8);
  isp.write_program_memory(32, buf, sizeof(buf) - 10);
  isp.write_program_memory(64, buf, sizeof(buf) - 12);
  isp.write_program_memory(96, buf, sizeof(buf) - 14);

  for (uint16_t src = 0; src < 256; src += sizeof(buf)) {
    isp.read_program_memory(buf, src, sizeof(buf));
    trace.print(buf, sizeof(buf), IOStream::hex);
  }
  trace << endl;

  for (uint16_t src = 0; src < 256; src += sizeof(buf)) {
    isp.read_eeprom_memory(buf, src, sizeof(buf));
    trace.print(buf, sizeof(buf), IOStream::hex);
  }
  trace << endl;
#endif

#ifdef __EEPROM_MEMORY__
  memset(buf, 0xff, sizeof(buf));
  isp.write_eeprom_memory(0, buf, sizeof(buf));

  for (uint16_t src = 0; src < 256; src += sizeof(buf)) {
    isp.read_eeprom_memory(buf, src, sizeof(buf));
    trace.print(buf, sizeof(buf), IOStream::hex);
  }
  trace << endl;
#endif

  isp.end();
  ASSERT(true == false);
}
