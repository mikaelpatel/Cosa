/**
 * @file Cosa/SPI.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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

#include "Cosa/SPI.hh"
#include "Cosa/Power.hh"

SPI spi  __attribute__ ((weak));

#if defined(__ARDUINO_TINY__)
#include "Cosa/Pins.hh"

// Create mapping to USI data direction register and port for ATtiny variants
#if defined(__ARDUINO_TINYX4__)
#define DDR DDRA
#define PORT PORTA
#elif defined(__ARDUINO_TINYX5__)
#define DDR DDRB
#define PORT PORTB
#endif

SPI::Driver::Driver(Board::DigitalPin cs, uint8_t pulse,
		    Clock clock, uint8_t mode, Order order,
		    Interrupt::Handler* irq) :
  m_irq(irq),
  m_cs(cs, (pulse == 0)),
  m_pulse(pulse),
  m_cpol(mode)
{
  // USI command for hardware supported bit banging 
  m_usicr = (_BV(USIWM0) | _BV(USICS1) | _BV(USICLK) | _BV(USITC));
  if (mode == 1 || mode == 2) m_usicr |= _BV(USICS0);

  // Attach driver to SPI bus controller device list
  m_next = spi.m_list;
  spi.m_list = this;
}

SPI::SPI(uint8_t mode, Order order) :
  m_dev(0)
{
  // Set port data direction. Note ATtiny MOSI/MISO are DI/DO.
  // Do not confuse with SPI chip programming pins
  synchronized {
    bit_set(DDR, Board::MOSI);
    bit_set(DDR, Board::SCK);
    bit_clear(DDR, Board::MISO);
    bit_set(PORT, Board::MISO);
    USICR = (_BV(USIWM0) | _BV(USICS1) | _BV(USICLK) | _BV(USITC));
    if (mode == 1 || mode == 2) USICR |= _BV(USICS0);
  }
}

SPI::SPI() :
  m_dev(0)
{
  // Set port data direction. Note ATtiny MOSI/MISO are DI/DO.
  // Do not confuse with SPI chip programming pins
  synchronized {
    bit_set(DDR, Board::MOSI);
    bit_set(DDR, Board::SCK);
    bit_clear(DDR, Board::MISO);
    bit_set(PORT, Board::MISO);
  }
}

bool
SPI::begin(Driver* dev)
{
  synchronized {
    if (m_dev != 0) synchronized_return (false);
    // Acquire the driver controller
    m_dev = dev;
    // Set clock polarity
    if (dev->m_cpol & 0x02) 
      bit_set(PORT, Board::SCK);
    else
      bit_clear(PORT, Board::SCK);
    // Enable device
    if (dev->m_pulse < 2) dev->m_cs.toggle();
    // Disable all interrupt sources on SPI bus
    for (dev = spi.m_list; dev != 0; dev = dev->m_next)
      if (dev->m_irq) dev->m_irq->disable();
  }
  return (true);
}

#else

SPI::Driver::Driver(Board::DigitalPin cs, uint8_t pulse,
		    Clock clock, uint8_t mode, Order order,
		    Interrupt::Handler* irq) :
  m_irq(irq),
  m_cs(cs, (pulse == 0)),
  m_pulse(pulse),
  // SPI Control Register for master mode
  m_spcr(_BV(SPE)                	| 
	 ((order & 0x1) << DORD) 	| 
	 _BV(MSTR)               	|
	 ((mode & 0x3) << CPHA)  	| 
	 ((clock & 0x3) << SPR0)),
  // SPI Clock control in Status Register 
  m_spsr(((clock & 0x04) != 0) << SPI2X)
{
  // Attach driver to SPI bus controller device list
  m_next = spi.m_list;
  spi.m_list = this;
}

SPI::SPI(uint8_t mode, Order order) :
  m_list(0),
  m_dev(0)
{
  // Initiate the SPI port and control for slave mode
  synchronized {
    bit_set(DDRB, Board::MISO);
    bit_clear(DDRB, Board::MOSI); 
    bit_clear(DDRB, Board::SCK);
    bit_clear(DDRB, Board::SS);
    SPCR = (_BV(SPIE) 			| 
	    _BV(SPE)			| 
	    ((order & 0x1) << DORD) 	|
	    ((mode & 0x3) << CPHA)); 
  }
}

SPI::SPI() :
  m_list(0),
  m_dev(0)
{
  // Initiate the SPI data direction for master mode
  // The SPI/SS pin must be an output pin in master mode
  synchronized {
    bit_set(DDRB, Board::MOSI);
    bit_set(DDRB, Board::SCK);
    bit_set(DDRB, Board::SS);
    bit_clear(DDRB, Board::MISO);
    bit_clear(PORTB, Board::SCK);
    bit_clear(PORTB, Board::MOSI);
    bit_set(PORTB, Board::SS);
  }
  // Other the SPI setup is done by the SPI::Driver::begin()
}

bool
SPI::begin(Driver* dev)
{
  synchronized {
    if (m_dev != 0) synchronized_return (false);
    // Acquire the driver controller
    m_dev = dev;
    // Initiate SPI hardware with device settings
    SPCR = dev->m_spcr;
    SPSR = dev->m_spsr;
    // Enable device
    if (dev->m_pulse < 2) dev->m_cs.toggle();
    // Disable all interrupt sources on SPI bus
    for (dev = spi.m_list; dev != 0; dev = dev->m_next)
      if (dev->m_irq) dev->m_irq->disable();
  }
  return (true);
}

void 
SPI::Slave::on_interrupt(uint16_t arg) 
{ 
  // Sanity check that a buffer is defined
  if (m_buffer == 0) return;

  // Append to buffer and push event when buffer is full
  m_buffer[m_put++] = arg;
  if (m_put != m_max) return;
  
  // Push receive completed to event dispatch
  Event::push(Event::RECEIVE_COMPLETED_TYPE, this, m_put);
  m_put = 0;
}

// Current slave device. Should be a singleton
SPI::Slave* SPI::Slave::s_device = 0;

ISR(SPI_STC_vect)
{
  SPI::Slave* device = SPI::Slave::s_device;
  if (device != 0) device->on_interrupt(SPDR);
}
#endif


bool
SPI::end()
{ 
  synchronized {
    if (m_dev == 0) synchronized_return (false);
    // Disable the device or give pulse if required
    m_dev->m_cs.toggle();
    if (m_dev->m_pulse > 1) m_dev->m_cs.toggle();
    // Enable the bus devices with interrupts
    for (Driver* dev = spi.m_list; dev != 0; dev = dev->m_next)
      if (dev->m_irq != 0) dev->m_irq->enable();
    // Release the driver controller
    m_dev = 0;
  }
  return (true);
}

void 
SPI::transfer(void* buffer, size_t count)
{
  if (count == 0) return;
  uint8_t* bp = (uint8_t*) buffer;
  do {
    *bp = transfer(*bp);
    bp += 1;
  } while (--count);
}

void 
SPI::transfer(void* dst, const void* src, size_t count)
{
  if (count == 0) return;
  uint8_t* dp = (uint8_t*) dst;
  const uint8_t* sp = (const uint8_t*) src;
  do *dp++ = transfer(*sp++); while (--count);
}

void 
SPI::read(void* buf, size_t count)
{
  if (count == 0) return;
  uint8_t* bp = (uint8_t*) buf;
  do *bp++ = transfer(0); while (--count);
}

void 
SPI::write(const void* buf, size_t count)
{
  if (count == 0) return;
  const uint8_t* bp = (const uint8_t*) buf;
  do transfer(*bp++); while (--count);
}

void 
SPI::write_P(const uint8_t* buf, size_t count)
{
  if (count == 0) return;
  do transfer(pgm_read_byte(buf++)); while (--count);
}
