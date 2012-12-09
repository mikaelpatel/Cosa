/**
 * @file CosaOneWireMaster.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Cosa demonstration of the OneWire Master-Slave.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OneWire.h"
#include "Cosa/Watchdog.h"
#include "Cosa/Memory.h"
#include "Cosa/Trace.h"

// A dummy device to read/write to the OneWire interface
class Device : public OneWire::Driver {
public:
  Device(OneWire* pin) : OneWire::Driver(pin) {}
};

// The OneWire bus on pin 7 and led heartbeat
OneWire oneWire(7);
Device device(&oneWire);
OutputPin ledPin(13);

void setup()
{
  trace.begin(9600, PSTR("CosaOneWireMaster: started"));
  TRACE(free_memory());
  Watchdog::begin();
}

#define __DEBUG__

void loop()
{
  static uint16_t ok = 0;
  static uint16_t rst = 0;
  static uint16_t err = 0;

  // Read ROM
  ledPin.toggle();
  while (!oneWire.reset()) rst++;
  oneWire.write(OneWire::READ_ROM, 8);
  uint8_t rom[OneWire::ROM_MAX + 1];
  oneWire.begin();
  for (uint8_t i = 0; i < membersof(rom) - 1; i++)
    rom[i] = oneWire.read(8);
  if (oneWire.end() != 0) err++; else ok++;
  rom[OneWire::ROM_MAX] = oneWire.end();
  ledPin.toggle();

#ifdef __DEBUG__
  trace.print(ok);
  trace.print_P(PSTR(":"));
  trace.print(rst);
  trace.print_P(PSTR(":"));
  trace.print(err);
  trace.print_P(PSTR(" "));
  trace.print(rom, sizeof(rom), 16);
  Watchdog::delay(512);
#endif

  // Match ROM and request STATUS
  ledPin.toggle();
  while (!oneWire.reset()) rst++;
  oneWire.write(OneWire::MATCH_ROM, 8);
  for (uint8_t i = 0; i < membersof(rom) - 1; i++)
    oneWire.write(rom[i]);
  oneWire.write(OneWire::Device::STATUS);
  uint8_t status[8];
  oneWire.begin();
  for (uint8_t i = 0; i < membersof(status) - 1; i++)
    status[i] = oneWire.read(8);
  if (oneWire.end() != 0) err++; else ok++;
  status[7] = oneWire.end();
  ledPin.toggle();

#ifdef __DEBUG__
  trace.print(ok);
  trace.print_P(PSTR(":"));
  trace.print(rst);
  trace.print_P(PSTR(":"));
  trace.print(err);
  trace.print_P(PSTR(" "));
  trace.print(status, sizeof(status));
  Watchdog::delay(512);
#else
  if (ok % 100 == 0) {
    trace.print(ok);
    trace.print_P(PSTR(":"));
    trace.print(rst);
    trace.print_P(PSTR(":"));
    trace.print(err);
    trace.println();
  }
  Watchdog::delay(128);
#endif
}
