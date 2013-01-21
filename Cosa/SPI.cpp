/**
 * @file Cosa/SPI.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * @section Description
 * Serial Peripheral Interface (SPI).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/SPI.hh"

SPI spi;

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
      bit_clear(PORTB, Board::SCK);
      bit_clear(PORTB, Board::MOSI);
      SPCR = (_BV(MSTR) | _BV(SPE));
    }
  }
  // Set control register according to parameters
  SPCR |= ((direction << DORD) 
	   | ((mode & 0x3) << CPHA) 
	   | ((clock & 0x3) << SPR0));
  SPSR = (((clock & 0x04) != 0) << SPI2X);
  return (1);
}

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

uint8_t
SPI::read(uint8_t cmd, void* buffer, uint8_t count)
{
  uint8_t status = exchange(cmd);
  for (uint8_t* bp = (uint8_t*) buffer; count != 0; bp++, count--) {
    *bp = exchange(0);
  }
  return (status);
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

bool
SPI::end()
{ 
  SPCR = 0;     
  return (1);
}

void 
SPI::Device::on_interrupt(uint8_t data) 
{ 
  // Check for no buffer
  if (spi.m_buffer == 0) {
    spi.m_put = 1;
    Event::push(Event::RECEIVE_COMPLETED_TYPE, this, spi.m_put);
    spi.m_put = 0;
    return;
  }

  // Append to buffer and call user interrupt handler on full
  spi.m_buffer[spi.m_put++] = data;
  if (spi.m_put == spi.m_max) {
    Event::push(Event::RECEIVE_COMPLETED_TYPE, this, spi.m_put);
    spi.m_put = 0;
  }
}

ISR(SPI_STC_vect)
{
  SPI::Device* device = spi.get_device();
  if (device != 0) device->on_interrupt(SPDR);
}

