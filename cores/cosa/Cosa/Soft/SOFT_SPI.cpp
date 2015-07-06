/**
 * @file Cosa/Soft/SOFT_SPI.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Soft/SPI.hh"

using namespace Soft;

SPI::Driver::Driver(Board::DigitalPin cs,
		    Pulse pulse,
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
  UNUSED(clock);
  m_next = spi.m_list;
  spi.m_list = this;
}

void
SPI::acquire(Driver* dev)
{
  // Acquire the device driver. Wait if busy. Synchronized update
  uint8_t key = lock();
  while (UNLIKELY(m_busy)) {
    unlock(key);
    yield();
    key = lock();
  }
  // Set current device and mark as busy
  m_busy = true;
  m_dev = dev;
  // Set clock polarity
  m_sck.write(dev->m_mode & 0x02);
  // Disable all interrupt sources on SPI bus
  for (SPI::Driver* dev = m_list; dev != NULL; dev = dev->m_next)
    if (dev->m_irq != NULL) dev->m_irq->disable();
  unlock(key);
}

void
SPI::release()
{
  // Lock the device driver update
  uint8_t key = lock();
  // Release the device driver
  m_busy = false;
  m_dev = NULL;
  // Enable all interrupt sources on SPI bus
  for (SPI::Driver* dev = m_list; dev != NULL; dev = dev->m_next)
    if (dev->m_irq != NULL) dev->m_irq->enable();
  unlock(key);
}

bool
SPI::attach(Driver* dev)
{
  if (dev->m_next != NULL) return (false);
  dev->m_next = m_list;
  m_list = dev;
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

