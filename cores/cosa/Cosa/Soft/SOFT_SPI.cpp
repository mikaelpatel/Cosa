/**
 * @file Cosa/Soft/SOFT_SPI.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Soft/SPI.hh"

namespace Soft {

SPI::Driver::Driver(Board::DigitalPin cs, 
		    uint8_t pulse,
		    Clock clock, 
		    uint8_t mode, 
		    Order order,
		    Interrupt::Handler* irq) :
  m_irq(irq),
  m_cs(cs, (pulse == 0)),
  m_pulse(pulse),
  m_mode(mode),
  m_order(order)
{
  m_next = spi.m_list;
  spi.m_list = this;
}

bool 
SPI::begin(Driver* dev)
{
  synchronized {
    if (m_dev != NULL) synchronized_return (false);
    // Acquire the driver controller
    m_dev = dev;
    // Set clock polarity
    m_sck.write(dev->m_mode & 0x02);
    // Enable device
    if (dev->m_pulse < 2) dev->m_cs.toggle();
    // Disable all interrupt sources on SPI bus
    for (dev = spi.m_list; dev != NULL; dev = dev->m_next)
      if (dev->m_irq) dev->m_irq->disable();
  }
  return (true);
}
  
bool 
SPI::end() 
{
  synchronized {
    if (m_dev == 0) synchronized_return (false);
    // Disable the device or give pulse if required
    m_dev->m_cs.toggle();
    if (m_dev->m_pulse > 1) m_dev->m_cs.toggle();
    // Enable the bus devices with interrupts
    for (Driver* dev = spi.m_list; dev != NULL; dev = dev->m_next)
      if (dev->m_irq != NULL) dev->m_irq->enable();
    // Release the driver controller
    m_dev = NULL;
  }
  return (true);
}
  
uint8_t
SPI::transfer(uint8_t value)
{
  uint8_t bits = CHARBITS;
  if (m_dev->m_order == MSB_ORDER) {
    synchronized do {
      m_mosi._write(value & 0x80);
      m_sck._toggle();
      value <<= 1;
      if (m_miso.is_set()) value |= 0x01;
      m_sck._toggle();
    } while (--bits);
  }
  else {
    synchronized do {
      m_mosi._write(value & 0x01);
      m_sck._toggle();
      value >>= 1;
      if (m_miso.is_set()) value |= 0x80;
      m_sck._toggle();
    } while (--bits);
  }
  return (value);
}

};

