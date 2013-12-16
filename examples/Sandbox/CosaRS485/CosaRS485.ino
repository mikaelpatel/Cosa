/**
 * @file CosaRS485.ino
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
 * Testing the UART transmit completed interrupt handler. First step
 * towards a RS485 driver. Typical usage will have three states; idle,
 * transmitting or receiving.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"

class RS485 : public UART {
private:
  IOBuffer<UART::BUFFER_MAX> m_ibuf;
  IOBuffer<UART::BUFFER_MAX> m_obuf;
  OutputPin m_de;
public:
  /**
   * Construct RS485 driver with data output enable connect to given pin.
   * @param[in] de data output enable.
   */
  RS485(Board::DigitalPin de) : 
    UART(0, &m_ibuf, &m_obuf),
    m_de(de)
  {
    UART_SETUP(0, *this);
  }

  /**
   * Enable the line driver.
   */
  void data_output_enable()
  {
    m_de.set();
  }

  /**
   * Disable the line driver.
   */
  void data_output_disable()
  {
    m_de.clear();
  }

  /**
   * @override UART
   * Transmit completed callback. Clear data output enable pin.
   */
  virtual void on_transmit_completed() 
  {
    data_output_disable();
  }
};

RS485 rs485(Board::LED);

void setup()
{
  rs485.begin(9600);
  trace.begin(&rs485, PSTR("CosaRS485: started"));
  Watchdog::begin();
}

void loop()
{
  static uint8_t nr = 0;
  rs485.data_output_enable();
  trace << nr++;
  trace << PSTR(":board led is on during the transmissions\n"); 
  SLEEP(1);
}
