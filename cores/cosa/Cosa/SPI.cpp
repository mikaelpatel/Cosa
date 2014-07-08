/**
 * @file Cosa/SPI.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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

#include "Cosa/SPI.hh"
#include "Cosa/Power.hh"

// Configuration: Allow SPI transfer interleaving
#if !defined(BOARD_ATTINY)
#define USE_SPI_PREFETCH
#endif

SPI spi  __attribute__ ((weak));

#if defined(SPDR)

SPI::Driver::Driver(Board::DigitalPin cs, Pulse pulse,
		    Clock rate, uint8_t mode, Order order,
		    Interrupt::Handler* irq) :
  m_next(NULL),
  m_irq(irq),
  m_cs(cs, (pulse == 0)),
  m_pulse(pulse),
  // SPI Control Register for master mode
  m_spcr(_BV(SPE)
	 | ((order & 0x1) << DORD)
	 | _BV(MSTR)
	 | ((mode & 0x3) << CPHA)
	 | ((rate & 0x3) << SPR0)),
  // SPI Clock control in Status Register 
  m_spsr(((rate & 0x04) != 0) << SPI2X)
{
}

SPI::SPI(uint8_t mode, Order order) :
  m_list(NULL),
  m_dev(NULL)
{
  // Initiate the SPI port and control for slave mode
  synchronized {
    bit_set(DDRB, Board::MISO);
    bit_mask_clear(DDRB, _BV(Board::MOSI) | _BV(Board::SCK) | _BV(Board::SS));
    SPCR = (_BV(SPIE) | _BV(SPE)
	    | ((order & 0x1) << DORD)
	    | ((mode & 0x3) << CPHA)); 
  }
}

SPI::SPI() :
  m_list(NULL),
  m_dev(NULL)
{
  // Initiate the SPI data direction for master mode
  // The SPI/SS pin must be an output pin in master mode
  synchronized {
    bit_mask_set(DDRB, _BV(Board::MOSI) | _BV(Board::SCK) | _BV(Board::SS));
    bit_clear(DDRB, Board::MISO);
    bit_mask_clear(PORTB, _BV(Board::SCK) | _BV(Board::MOSI));
    bit_set(PORTB, Board::MISO);
  }
  // Other the SPI setup is done by the SPI::Driver::begin()
}

bool
SPI::begin(Driver* dev)
{
  synchronized {
    if (m_dev != NULL) synchronized_return (false);

    // Acquire the driver controller
    m_dev = dev;

    // Initiate SPI hardware with device settings
    SPCR = dev->m_spcr;
    SPSR = dev->m_spsr;

    // Enable device
    if (dev->m_pulse < PULSE_LOW) dev->m_cs.toggle();

    // Disable all interrupt sources on SPI bus
    for (dev = spi.m_list; dev != NULL; dev = dev->m_next)
      if (dev->m_irq != NULL) dev->m_irq->disable();
  }
  return (true);
}

void
SPI::Driver::set_clock(Clock rate)
{
  m_spcr = (m_spcr & ~(0x3 << SPR0)) | ((rate & 0x3) << SPR0);
  m_spsr = (m_spsr & ~(1 << SPI2X)) | (((rate & 0x04) != 0) << SPI2X);
}

void 
SPI::Slave::on_interrupt(uint16_t arg) 
{ 
  // Sanity check that a buffer is defined
  if (m_buf == NULL) return;

  // Append to buffer and push event when buffer is full
  m_buf[m_put++] = arg;
  if (m_put != m_max) return;
  
  // Push receive completed to event dispatch
  Event::push(Event::RECEIVE_COMPLETED_TYPE, this, m_put);
  m_put = 0;
}

// Current slave device. Should be a singleton
SPI::Slave* SPI::Slave::s_device = NULL;

ISR(SPI_STC_vect)
{
  SPI::Slave* device = SPI::Slave::s_device;
  if (device != NULL) device->on_interrupt(SPDR);
}

#elif defined(USIDR)

// Create mapping to USI data direction register and port for ATtiny variants
#if defined(BOARD_ATTINYX4) || defined(BOARD_ATTINYX61)
#define DDR DDRA
#define PORT PORTA
#elif defined(BOARD_ATTINYX5)
#define DDR DDRB
#define PORT PORTB
#endif

SPI::Driver::Driver(Board::DigitalPin cs, Pulse pulse,
		    Clock rate, uint8_t mode, Order order,
		    Interrupt::Handler* irq) :
  m_next(NULL),
  m_irq(irq),
  m_cs(cs, (pulse == 0)),
  m_pulse(pulse),
  m_cpol(mode)
{
  UNUSED(rate);
  UNUSED(order);

  // USI command for hardware supported bit banging 
  m_usicr = (_BV(USIWM0) | _BV(USICS1) | _BV(USICLK) | _BV(USITC));
  if (mode == 1 || mode == 2) m_usicr |= _BV(USICS0);

  // Set ports
  synchronized {
    bit_mask_set(DDR, _BV(Board::MOSI) | _BV(Board::SCK));
    bit_clear(DDR, Board::MISO);
    bit_set(PORT, Board::MISO);
    USICR = m_usicr;
  }
}

SPI::SPI(uint8_t mode, Order order) :
  m_list(NULL),
  m_dev(NULL)
{
  UNUSED(order);

  // Set port data direction. Note ATtiny MOSI/MISO are DI/DO.
  // Do not confuse with SPI chip programming pins
  synchronized {
    bit_mask_set(DDR, _BV(Board::MOSI) | _BV(Board::SCK));
    bit_clear(DDR, Board::MISO);
    bit_set(PORT, Board::MISO);
    USICR = (_BV(USIWM0) | _BV(USICS1) | _BV(USICLK) | _BV(USITC));
    if (mode == 1 || mode == 2) USICR |= _BV(USICS0);
  }
}

SPI::SPI() :
  m_list(NULL),
  m_dev(NULL)
{
  // Set port data direction. Note ATtiny MOSI/MISO are DI/DO.
  // Do not confuse with SPI chip programming pins
  synchronized {
    bit_mask_set(DDR, _BV(Board::MOSI) | _BV(Board::SCK));
    bit_clear(DDR, Board::MISO);
    bit_set(PORT, Board::MISO);
  }
}

bool
SPI::begin(Driver* dev)
{
  synchronized {
    if (m_dev != NULL) synchronized_return (false);

    // Acquire the driver controller
    m_dev = dev;

    // Set clock polarity
    if (dev->m_cpol & 0x02) 
      bit_set(PORT, Board::SCK);
    else
      bit_clear(PORT, Board::SCK);

    // Enable device
    if (dev->m_pulse < PULSE_LOW) dev->m_cs.toggle();

    // Disable all interrupt sources on SPI bus
    for (dev = spi.m_list; dev != NULL; dev = dev->m_next)
      if (dev->m_irq != NULL) dev->m_irq->disable();
  }
  return (true);
}

void
SPI::Driver::set_clock(Clock rate)
{
  UNUSED(rate);
}
#endif

/*
 * Prefetch optimized variants of the block transfer member functions.
 * These implementation allow higher transfer speed for block by using
 * the available cycles while a byte transfer is in progress for data
 * prefetch and store. There are at least 16 instruction cycles available
 * (CLOCK_DIV_2). 
 */
#if defined(USE_SPI_PREFETCH)
void 
SPI::transfer(void* buf, size_t count)
{
  if (count == 0) return;
  uint8_t* dp = (uint8_t*) buf;
  uint8_t data = *dp;
  transfer_start(data);
  while (--count) {
    uint8_t* tp = dp + 1;
    data = *tp;
    *dp = transfer_next(data);
    dp = tp;
  }
  *dp = transfer_await();
}

void 
SPI::transfer(void* dst, const void* src, size_t count)
{
  if (count == 0) return;
  uint8_t* dp = (uint8_t*) dst;
  const uint8_t* sp = (const uint8_t*) src;
  uint8_t data = *sp++;
  transfer_start(data);
  while (--count) {
    uint8_t* tp = dp + 1;
    data = *sp++;
    *dp = transfer_next(data);
    dp = tp;
  }
  *dp = transfer_await();
}

void 
SPI::read(void* buf, size_t count)
{
  if (count == 0) return;
  uint8_t* dp = (uint8_t*) buf;
  transfer_start(0xff);
  while (--count) *dp++ = transfer_next(0xff);
  *dp = transfer_await();
}

void 
SPI::write(const void* buf, size_t count)
{
  if (count == 0) return;
  const uint8_t* sp = (const uint8_t*) buf;
  uint8_t data = *sp++;
  transfer_start(data);
  while (--count) {
    data = *sp++;
    transfer_next(data);
  }
  transfer_await();
}

void 
SPI::write_P(const void* buf, size_t count)
{
  if (count == 0) return;
  const uint8_t* sp = (const uint8_t*) buf;
  uint8_t data = pgm_read_byte(sp++);
  transfer_start(data);
  while (--count) {
    data = pgm_read_byte(sp++);
    transfer_next(data);
  }
  transfer_await();
}

#else

void 
SPI::transfer(void* buf, size_t count)
{
  if (count == 0) return;
  uint8_t* bp = (uint8_t*) buf;
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

#endif

void 
SPI::write(const iovec_t* vec)
{
  for (const iovec_t* vp = vec; vp->buf != NULL; vp++)
    write(vp->buf, vp->size);
}

bool 
SPI::attach(Driver* dev)
{
  if (dev->m_next != NULL) return (false);
  dev->m_next = m_list;
  m_list = dev;
  return (true);
}

bool
SPI::end()
{ 
  synchronized {
    if (m_dev == NULL) synchronized_return (false);

    // Disable the device or give pulse if required
    m_dev->m_cs.toggle();
    if (m_dev->m_pulse > ACTIVE_HIGH) m_dev->m_cs.toggle();

    // Enable the bus devices with interrupts
    for (Driver* dev = spi.m_list; dev != NULL; dev = dev->m_next)
      if (dev->m_irq != NULL) dev->m_irq->enable();

    // Release the driver controller
    m_dev = NULL;
  }
  return (true);
}
