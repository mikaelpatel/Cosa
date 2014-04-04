/**
 * @file CosaTWIremotePinMaster.ino
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
 * Cosa demonstration of a TWI master-slave; Remote Pin handling.
 * Read/Write pins on slave (typically ATtiny84/85).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

OutputPin ledPin(Board::LED);

class RemotePin {
private:
  static const uint8_t ADDR = 0x5A;
  static const uint8_t READ_OP = 0;
  static const uint8_t WRITE_OP = 1;
  static const uint8_t DIGITAL_TYPE = 0x00;
  static const uint8_t ANALOG_TYPE = 0x01;
  struct Command {
    union {
      uint8_t as_char;
      struct {
	uint8_t m_pin:4;
	uint8_t m_type:2;
	uint8_t m_op:2;
      };
    };
    Command(uint8_t op, Board::DigitalPin pin)
    {
      m_op = op;
      m_type = DIGITAL_TYPE;
      m_pin = (uint8_t) pin;
    }
  };
  static TWI::Driver dev;
public:
  static bool read(Board::DigitalPin pin, uint8_t& value);
  static bool write(Board::DigitalPin pin, uint8_t value);
};

TWI::Driver RemotePin::dev(ADDR);

bool
RemotePin::read(Board::DigitalPin pin, uint8_t& value)
{
  Command cmd(READ_OP, pin);
  bool res = false;
  twi.begin(&dev);
  uint8_t count = twi.write(&cmd, sizeof(cmd));
  if (count != sizeof(cmd)) goto error;
  count = twi.read(&value, sizeof(value));
  res = (count == sizeof(value));
 error:
  twi.end();
  return (res);
}

bool
RemotePin::write(Board::DigitalPin pin, uint8_t value)
{
  Command cmd(WRITE_OP, pin);
  twi.begin(&dev);
  uint8_t count = twi.write(cmd.as_char, &value, sizeof(value));
  twi.end();
  return (count == sizeof(cmd) + sizeof(value));
}

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaTWIremotePinMaster: started"));
  TRACE(free_memory());
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(TWI));
  Watchdog::begin();
}

void loop()
{
  static uint8_t state = 0;
  uint8_t button;
  for (uint16_t i = 100; i < 700; i += 100) {
    int j;
    for (j = 0; j < 700; j += i) {
      RemotePin::read(Board::D0, button);
      RemotePin::write(Board::D5, state & button);
      state = !state;
      Watchdog::delay(i);
    }
  }
}
