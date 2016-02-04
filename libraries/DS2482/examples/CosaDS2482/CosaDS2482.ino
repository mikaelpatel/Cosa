/**
 * @file CosaDS2482.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2016, Mikael Patel
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
 * Cosa demonstration of DS2482 I2C-to-1-Wire bridge device driver;
 * DS18B20.
 *
 * @section Circuit
 * The Arduino analog pins 4 (SDA) and 5 (SCL) are used for I2C/TWI
 * connection. DS18B20 on 1-Wire bus.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <DS2482.h>

#include "Cosa/RTT.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

#include <util/crc16.h>

DS2482 owi;

// One-Wire Commands
enum {
  SEARCH_ROM = 0xf0,
  READ_ROM = 0x33,
  MATCH_ROM = 0x55,
  SKIP_ROM = 0xcc
};

const size_t ROM_MAX = 8;

// DS18B20 Commands
enum {
  CONVERT_T = 0x44,
  READ_SCRATCHPAD = 0xbe
};

// DS18B20 Scratchpad Memory
struct scratchpad_t {
  int16_t temperature;
  int8_t high_trigger;
  int8_t low_trigger;
  uint8_t configuration;
  uint8_t reserved[3];
  uint8_t crc;
};

void setup()
{
  uint32_t hz;
  uint8_t rom[ROM_MAX];
  uint8_t crc = 0;
  int res;

  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaDS2482: started"));
  RTT::begin();

  // Set I2C clock frequency
  // hz = TWI::DEFAULT_FREQ;
  hz = 400000L;
  // hz = TWI::MAX_FREQ;
  twi.set_freq(hz);
  trace << PSTR("TWI clock: ") << hz / 1000.0 << PSTR(" khz") << endl;

  MEASURE("Device Reset:", 1) ASSERT(owi.device_reset());
  MEASURE("Device Configuration:", 1) ASSERT(owi.device_config());

  trace << PSTR("Read Data Register: ")
	<< hex << DS2482::READ_DATA_REGISTER << '='
	<< bin << owi.set_read_pointer(DS2482::READ_DATA_REGISTER)
	<< endl;

  trace << PSTR("Read Configuration Register: ")
	<< hex << DS2482::CONFIGURATION_REGISTER << '='
	<< bin << owi.set_read_pointer(DS2482::CONFIGURATION_REGISTER)
	<< endl;

  trace << PSTR("Read Channel Selection Register: ")
	<< hex << DS2482::CHANNEL_SELECTION_REGISTER << '='
	<< bin << owi.set_read_pointer(DS2482::CHANNEL_SELECTION_REGISTER)
	<< endl;

  trace << PSTR("Read Status Register: ")
	<< hex << DS2482::STATUS_REGISTER << '='
	<< bin << owi.set_read_pointer(DS2482::STATUS_REGISTER)
	<< endl;

  MEASURE("Read ROM: ", 1) {
    ASSERT(owi.one_wire_reset());
    ASSERT(owi.one_wire_write_byte(READ_ROM));
    for (int i = 0; i < (int) sizeof(rom); i++) {
      rom[i] = owi.one_wire_read_byte();
      crc = _crc_ibutton_update(crc, rom[i]);
    }
    ASSERT(crc == 0);
  }

  trace << PSTR("rom=") << hex << rom[0];
  for (int i = 1; i < (int) sizeof(rom); i++)
    trace << ' ' << hex << rom[i];
  trace << endl;
  trace.flush();

  MEASURE("Search ROM: ", 1) {
    uint8_t value = 0;
    uint8_t bits = 0;
    uint8_t ix = 0;
    bool dir = 0;
    bool id;
    bool nid;
    ASSERT(owi.one_wire_reset());
    ASSERT(owi.one_wire_write_byte(SEARCH_ROM));
    do {
      ASSERT((res = owi.one_wire_triplet(dir)) >= 0);
      dir = (res & 4) != 0;
      id = (res & 1) != 0;
      nid = (res & 2) != 0;
      value = (value >> 1);
      if (id) value |= 0x80;
      bits += 1;
      if (bits == CHARBITS) {
	rom[ix++] = value;
	bits = 0;
      }
    } while (id != nid);
  }

  trace << PSTR("rom=") << hex << rom[0];
  for (int i = 1; i < (int) sizeof(rom); i++)
    trace << ' ' << hex << rom[i];
  trace << endl;
  trace.flush();
}

void loop()
{
  scratchpad_t scratchpad;
  uint8_t crc = 0;

  MEASURE("Issue Reset:", 1) {
    ASSERT(owi.one_wire_reset());
  }

  MEASURE("Issue Convert Temperature:", 1) {
    ASSERT(owi.one_wire_write_byte(SKIP_ROM));
    ASSERT(owi.one_wire_write_byte(CONVERT_T));
  }

  MEASURE("Wait for Conversion:", 1)
    delay(750);

  MEASURE("Issue Read Scratchpad:", 1) {
    ASSERT(owi.one_wire_reset());
    ASSERT(owi.one_wire_write_byte(SKIP_ROM));
    ASSERT(owi.one_wire_write_byte(READ_SCRATCHPAD));
  }

  MEASURE("Read Scratchpad (9 byte):", 1) {
    uint8_t* p = (uint8_t*) &scratchpad;
    for (int i = 0; i < (int) sizeof(scratchpad); i++) {
      p[i] = owi.one_wire_read_byte();
    }
  }
  MEASURE("Validate check sum:", 1) {
    uint8_t* p = (uint8_t*) &scratchpad;
    for (int i = 0; i < (int) sizeof(scratchpad); i++) {
      crc = _crc_ibutton_update(crc, p[i]);
    }
    ASSERT(crc == 0);
  }
  trace << PSTR("temperature=")
	<< scratchpad.temperature * 0.0625 << PSTR(" C")
	<< endl;
  trace.flush();

  MEASURE("Issue Read Scratchpad:", 1) {
    ASSERT(owi.one_wire_reset());
    ASSERT(owi.one_wire_write_byte(SKIP_ROM));
    ASSERT(owi.one_wire_write_byte(READ_SCRATCHPAD));
  }

  MEASURE("Read Scratchpad (2 byte):", 1) {
    uint8_t* p = (uint8_t*) &scratchpad;
    for (int i = 0; i < (int) sizeof(scratchpad.temperature); i++) {
      p[i] = owi.one_wire_read_byte();
    }
  }
  trace << PSTR("temperature=")
	<< scratchpad.temperature * 0.0625 << PSTR(" C")
	<< endl	<< endl;
  trace.flush();

  sleep(2);
}
