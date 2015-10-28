/**
 * @file CosaTWIslave.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * Cosa demonstration of a TWI slave. Please see CosaTWImaster.
 *
 * @section Circuit
 * The Arduino analog pins 4 (SDA) and 5 (SCL) are used for I2C/TWI
 * connection.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Memory.h"

// A simple TWI slave device
class Echo : public TWI::Slave {
private:
  // Buffer for request and response
  static const uint8_t BUF_MAX = 8;
  uint8_t m_buf[BUF_MAX];

public:
  // Construct the echo slave device
  Echo() : TWI::Slave(0x5A)
  {
    write_buf(m_buf, sizeof(m_buf));
    read_buf(m_buf, sizeof(m_buf));
  }

  // Request handler; events from incoming requests
  virtual void on_request(void* buf, size_t size);
};

void
Echo::on_request(void* buf, size_t size)
{
  UNUSED(buf);
  UNUSED(size);
  m_buf[0] += 1;
  for (uint8_t i = 1; i < sizeof(m_buf); i++)
    m_buf[i] = m_buf[0] + i;
}

// The TWI echo device
Echo echo;

#if defined(BOARD_ATTINYX5)
Soft::UAT uart(Board::D1);
#endif

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaTWISlave: started"));

  // Check amount of free memory and size of classes
  TRACE(free_memory());
  TRACE(sizeof(Echo));
  TRACE(sizeof(OutputPin));

  // Start the watchdog ticks counter
  Watchdog::begin();

  // Start the TWI echo device
  echo.begin();
}

void loop()
{
  Event::service();
}
