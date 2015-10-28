/**
 * @file CosaIOEvent.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
 * Cosa IOEvent demonstration.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Event.hh"
#include "Cosa/IOEvent.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

typedef IOBuffer<UART::RX_BUFFER_MAX> Buffer;

class Handler : public Event::Handler {
public:
  virtual void on_event(uint8_t type, uint16_t value)
  {
    TRACE(type);
    Buffer* buf = (Buffer*) value;
    while (buf->available()) trace << (char) buf->getchar();
  }
};

Handler handler;
IOEvent<Buffer> ibuf(&handler);
Buffer obuf;
UART uart(0, &ibuf, &obuf);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaIOEvent: started"));
}

void loop()
{
  Event::service();
}

