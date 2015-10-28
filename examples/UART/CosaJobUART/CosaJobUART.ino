/**
 * @file CosaJobUART.ino
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
 * Use Job to implement an UART.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/Job.hh"
#include "Cosa/Serial.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/UART.hh"
#include "Cosa/Trace.hh"

namespace Soft {

/**
 * Soft UART based on Job.
 */
class UART : public Serial, public Periodic {
public:
  /**
   * Construct Soft UART with transmitter on given output pin and
   * receiver on given pin change interrupt pin. Received data is
   * captured by an interrupt service routine and put into given
   * buffer (i.e. io-stream).
   * @param[in] tx transmitter pin.
   * @param[in] rx receiver pin.
   * @param[in] ibuf input buffer.
   */
  UART(Job::Scheduler* scheduler,
       Board::DigitalPin tx, IOStream::Device* obuf) :
    Serial(),
    Periodic(scheduler, 1000),
    m_tx(tx, 1),
    m_obuf(obuf),
    m_stops(1),
    m_bits(8),
    m_bit(0),
    m_data(0)
  {
  }

  /**
   * @override IOStream::Device
   * Write character to serial port output buffer. Returns character
   * if successful otherwise on error or buffer full returns EOF(-1),
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c)
  {
    int res = m_obuf->putchar(c);
    return (res);
  }

  /**
   * @override Serial
   * Start Soft UART device driver.
   * @param[in] baudrate serial bitrate (default 9600).
   * @param[in] format serial frame format (default DATA8, NO PARITY, STOP2).
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool begin(uint32_t baudrate = DEFAULT_BAUDRATE,
		     uint8_t format = DEFAULT_FORMAT)
  {
    m_stops = 1 + ((format & STOP2) != 0);
    m_bits = 5 + ((format & DATA8) >> 1);
    period((1000000UL / baudrate));
    expire_at(time() + 1000);
    start();
    return (true);
  }

protected:
  virtual void on_expired();
  OutputPin m_tx;
  IOStream::Device* m_obuf;
  uint8_t m_stops;
  uint8_t m_bits;
  volatile uint8_t m_bit;
  volatile uint16_t m_data;
};

void
UART::on_expired()
{
  if (m_bit == 0) {
    int c = m_obuf->getchar();
    if (c == IOStream::EOF) goto error;
    m_data = ((0xff00 | c) << 1);
    m_bit = m_bits + m_stops + 1;
  }
  m_tx._write(m_data & 0x01);
  m_data >>= 1;
  m_bit -= 1;
 error:
  reschedule();
}

};

RTT::Scheduler scheduler;
IOBuffer<32> obuf;
Soft::UART serial(&scheduler, Board::D8, &obuf);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaJobUART: started"));
  RTT::begin();
  serial.begin(11000);
}

void loop()
{
  uart.puts(PSTR("Hello world!"));
  serial.puts(PSTR("Hello world!"));
  delay(500);
}
