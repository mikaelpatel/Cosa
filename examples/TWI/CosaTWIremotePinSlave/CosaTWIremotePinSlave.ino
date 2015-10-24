/**
 * @file CosaTWIremotePinSlave.ino
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
 * Cosa demonstration of a TWI master-slave; Remote Pin handling.
 * Read pins on slave (typically ATtiny84/85).
 *
 * @section Circuit
 * The Arduino analog pins 4 (SDA) and 5 (SCL) are used for I2C/TWI
 * connection.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/IOPin.hh"
#include "Cosa/Watchdog.hh"

class RemotePinSlave : public TWI::Slave {
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
    Command(uint8_t cmd)
    {
      as_char = cmd;
    }
  };
  static const uint8_t BUF_MAX = 8;
  uint8_t m_buf[BUF_MAX];

public:
  RemotePinSlave() : TWI::Slave(ADDR)
  {
    write_buf(m_buf, sizeof(m_buf));
    read_buf(m_buf, sizeof(m_buf));
  }
  virtual void on_request(void* buf, size_t size);
};

void
RemotePinSlave::on_request(void* buf, size_t size)
{
  UNUSED(buf);
  UNUSED(size);
  Command cmd(m_buf[0]);
  if (cmd.m_type == DIGITAL_TYPE) {
    if (cmd.m_op == READ_OP)
      m_buf[0] = Pin::read((Board::DigitalPin) cmd.m_pin);
    else if (cmd.m_op == WRITE_OP) {
      IOPin::mode((Board::DigitalPin) cmd.m_pin, IOPin::OUTPUT_MODE);
      OutputPin::write((Board::DigitalPin) cmd.m_pin, m_buf[1]);
    }
  }
}

RemotePinSlave slave;

void setup()
{
  Watchdog::begin();
  slave.begin();
}

void loop()
{
  Event::service();
}
