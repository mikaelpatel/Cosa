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

#include "Cosa/Board.hh"
#include "Cosa/SPI.hh"

SPI spi  __attribute__ ((weak));

#if defined(__ARDUINO_TINY__)
#include "Cosa/Pins.hh"

bool
SPI::begin(Clock clock, uint8_t mode, Direction direction)
{
  if (clock == MASTER_CLOCK) return (false);
  // Mode value to be used when clocking data
  m_mode = _BV(USIWM0) | _BV(USICS1) | _BV(USICLK) | _BV(USITC);
  // Check for data capture on falling edge
  if (mode == 1 || mode == 2) m_mode |= _BV(USICS0);
  synchronized {
#if defined(__ARDUINO_TINYX4__)
    bit_set(DDRA, Board::MOSI);
    bit_set(DDRA, Board::SCK);
    bit_clear(DDRA, Board::MISO);
    bit_set(PORTA, Board::MISO);
    // Check for inversed clock
    if (mode & 0x02) bit_set(PORTB, Board::SCK);
#elif defined(__ARDUINO_TINYX5__)
    bit_set(DDRB, Board::MOSI);
    bit_set(DDRB, Board::SCK);
    bit_clear(DDRB, Board::MISO);
    bit_set(PORTB, Board::MISO);
    // Check for inversed clock
    if (mode & 0x02) bit_set(PORTB, Board::SCK);
#endif
  };
  return (true);
}

bool
SPI::end()
{ 
#if defined(__ARDUINO_TINYX4__)
  bit_clear(DDRA, Board::MOSI);
  bit_clear(DDRA, Board::SCK);
#elif defined(__ARDUINO_TINYX5__)
  bit_clear(DDRB, Board::MOSI);
  bit_clear(DDRB, Board::SCK);
#endif
  return (true);
}

#else

bool
SPI::begin(Clock clock, uint8_t mode, Direction direction)
{
  // Check for slave pin setting; input(MOSI, SS, SCK), output(MISO)
  synchronized {
    if (clock == MASTER_CLOCK) {
      m_put = 0;
      bit_clear(DDRB, Board::MOSI); 
      bit_set(DDRB, Board::MISO);
      bit_clear(DDRB, Board::SCK);
      bit_clear(DDRB, Board::SS);
      SPCR = (_BV(SPIE) | _BV(SPE));
    } 
    // Master pin setting; input(MISO), output(MOSI, SCK)
    else {
      bit_set(DDRB, Board::MOSI);
      bit_clear(DDRB, Board::MISO);
      bit_set(DDRB, Board::SCK);
      bit_set(DDRB, Board::SS);
      bit_clear(PORTB, Board::SCK);
      bit_clear(PORTB, Board::MOSI);
      bit_set(PORTB, Board::SS);
      SPCR = (_BV(MSTR) | _BV(SPE));
    }
  }
  // Set control register according to parameters
  SPCR |= ((direction << DORD) 
	   | ((mode & 0x3) << CPHA) 
	   | ((clock & 0x3) << SPR0));
  SPSR = (((clock & 0x04) != 0) << SPI2X);
  return (true);
}

bool
SPI::end()
{ 
  SPCR = 0;     
  return (true);
}

ISR(SPI_STC_vect)
{
  SPI::Device* device = spi.get_device();
  if (device != 0) device->on_interrupt(SPDR);
}
#endif

void
SPI::exchange(void* buffer, uint8_t count)
{
  uint8_t* bp = (uint8_t*) buffer;
  for(; count != 0; bp++, count--) {
    *bp = exchange(*bp);
  }
}

void 
SPI::exchange_P(const void* buffer, uint8_t count)
{
  for (const uint8_t* bp = (const uint8_t*) buffer; count != 0; bp++, count--) {
    exchange(pgm_read_byte(*bp));
  }
}

void
SPI::read(void* buffer, uint8_t count)
{
  for (uint8_t* bp = (uint8_t*) buffer; count != 0; bp++, count--) {
    *bp = exchange(0);
  }
}

uint8_t
SPI::read(uint8_t cmd, void* buffer, uint8_t count)
{
  uint8_t status = exchange(cmd);
  for (uint8_t* bp = (uint8_t*) buffer; count != 0; bp++, count--) {
    *bp = exchange(0);
  }
  return (status);
}

void
SPI::write(const void* buffer, uint8_t count)
{
  for (uint8_t* bp = (uint8_t*) buffer; count != 0; bp++, count--) {
    exchange(*bp);
  }
}

uint8_t
SPI::write(uint8_t cmd, const void* buffer, uint8_t count)
{
  uint8_t status = exchange(cmd);
  for (uint8_t* bp = (uint8_t*) buffer; count != 0; bp++, count--) {
    exchange(*bp);
  }
  return (status);
}

uint8_t
SPI::write_P(uint8_t cmd, const void* buffer, uint8_t count)
{
  uint8_t status = exchange(cmd);
  for (uint8_t* bp = (uint8_t*) buffer; count != 0; bp++, count--) {
    exchange(pgm_read_byte(bp));
  }
  return (status);
}

void 
SPI::Device::on_interrupt(uint16_t arg) 
{ 
  // Sanity check that a buffer is defined
  if (spi.m_buffer == 0) return;

  // Append to buffer and push event when buffer is full
  spi.m_buffer[spi.m_put++] = arg;
  if (spi.m_put != spi.m_max) return;
  
  Event::push(Event::RECEIVE_COMPLETED_TYPE, this, spi.m_put);
  spi.m_put = 0;
}
