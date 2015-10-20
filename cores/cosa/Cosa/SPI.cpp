/**
 * @file Cosa/SPI.cpp
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
  m_cs(cs, ((pulse & 0x01) == 0)),
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

SPI::SPI() :
  m_list(NULL),
  m_dev(NULL),
  m_busy(false)
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
  m_cs(cs, ((pulse & 0x01) == 0)),
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

SPI::SPI() :
  m_list(NULL),
  m_dev(NULL),
  m_busy(false)
{
  // Set port data direction. Note ATtiny MOSI/MISO are DI/DO.
  // Do not confuse with SPI chip programming pins
  synchronized {
    bit_mask_set(DDR, _BV(Board::MOSI) | _BV(Board::SCK));
    bit_clear(DDR, Board::MISO);
    bit_set(PORT, Board::MISO);
  }
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
  if (UNLIKELY(count == 0)) return;
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
  if (UNLIKELY(count == 0)) return;
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
  if (UNLIKELY(count == 0)) return;
  uint8_t* dp = (uint8_t*) buf;
  transfer_start(0xff);
  while (--count) *dp++ = transfer_next(0xff);
  *dp = transfer_await();
}

void
SPI::write(const void* buf, size_t count)
{
  if (UNLIKELY(count == 0)) return;
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
  if (UNLIKELY(count == 0)) return;
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
  if (UNLIKELY(count == 0)) return;
  uint8_t* bp = (uint8_t*) buf;
  do {
    *bp = transfer(*bp);
    bp += 1;
  } while (--count);
}

void
SPI::transfer(void* dst, const void* src, size_t count)
{
  if (UNLIKELY(count == 0)) return;
  uint8_t* dp = (uint8_t*) dst;
  const uint8_t* sp = (const uint8_t*) src;
  do *dp++ = transfer(*sp++); while (--count);
}

void
SPI::read(void* buf, size_t count)
{
  if (UNLIKELY(count == 0)) return;
  uint8_t* bp = (uint8_t*) buf;
  do *bp++ = transfer(0); while (--count);
}

void
SPI::write(const void* buf, size_t count)
{
  if (UNLIKELY(count == 0)) return;
  const uint8_t* bp = (const uint8_t*) buf;
  do transfer(*bp++); while (--count);
}

void
SPI::write_P(const void* buf, size_t count)
{
  if (UNLIKELY(count == 0)) return;
  const uint8_t* bp = (const uint8_t*) buf;
  do transfer(pgm_read_byte(bp++)); while (--count);
}

#endif

bool
SPI::attach(Driver* dev)
{
  if (UNLIKELY(dev->m_next != NULL)) return (false);
  dev->m_next = m_list;
  m_list = dev;
  return (true);
}

void
SPI::acquire(Driver* dev)
{
  // Acquire the device driver. Wait if busy. Synchronized update
  uint8_t key = lock(m_busy);

  // Power up
  SPI::powerup();

  // Set current device driver
  m_dev = dev;

#if defined(SPDR)
  // Initiate SPI hardware with device settings
  SPCR = dev->m_spcr;
  SPSR = dev->m_spsr;
#else
  // Set clock polarity
  bit_write(dev->m_cpol & 0x02, PORT, Board::SCK);
#endif
  // Disable all interrupt sources on SPI bus
  for (SPI::Driver* dev = m_list; dev != NULL; dev = dev->m_next)
    if (dev->m_irq != NULL) dev->m_irq->disable();
  unlock(key);
}

void
SPI::release()
{
  synchronized {
    // Power down
    SPI::powerdown();

    // Release the device driver
    m_busy = false;
    m_dev = NULL;
    // Enable all interrupt sources on SPI bus
    for (SPI::Driver* dev = m_list; dev != NULL; dev = dev->m_next)
      if (dev->m_irq != NULL) dev->m_irq->enable();
  }
}

void
SPI::Driver::set_clock(Clock rate)
{
#if defined(SPDR)
  m_spcr = (m_spcr & ~(0x3 << SPR0)) | ((rate & 0x3) << SPR0);
  m_spsr = (m_spsr & ~(1 << SPI2X)) | (((rate & 0x04) != 0) << SPI2X);
#else
  UNUSED(rate);
#endif
}

IOStream& operator<<(IOStream& outs, SPI::Clock rate)
{
  switch (rate) {
  case SPI::DIV2_CLOCK:
    outs << PSTR("SPI::DIV2_CLOCK(") << F_CPU / 2000000.0;
    break;
  case SPI::DIV4_CLOCK:
    outs << PSTR("SPI::DIV4_CLOCK(") << F_CPU / 4000000.0;
    break;
  case SPI::DIV8_CLOCK:
    outs << PSTR("SPI::DIV8_CLOCK(") << F_CPU / 8000000.0;
    break;
  case SPI::DIV16_CLOCK:
    outs << PSTR("SPI::DIV16_CLOCK(") << F_CPU / 16000000.0;
    break;
  case SPI::DIV32_CLOCK:
    outs << PSTR("SPI::DIV32_CLOCK(") << F_CPU / 32000000.0;
    break;
  case SPI::DIV64_CLOCK:
    outs << PSTR("SPI::DIV64_CLOCK(") << F_CPU / 64000000.0;
    break;
  case SPI::DIV128_CLOCK:
    outs << PSTR("SPI::DIV128_CLOCK(") <<  F_CPU / 128000000.0;
    break;
  };
  outs << PSTR(" MHz)");
  return (outs);
}
