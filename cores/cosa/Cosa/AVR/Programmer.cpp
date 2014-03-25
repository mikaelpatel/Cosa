/**
 * @file Cosa/AVR/Programmer.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/AVR/Programmer.hh"
#include "Cosa/Watchdog.hh"

bool
Programmer::begin()
{
  synchronized {
    // Initiate SPI module; Enable master, mode(0), prescale(128)
#if defined(SPCR)
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0);
    uint8_t status = SPSR;
    uint8_t data = SPDR;
    UNUSED(status);
    UNUSED(data);
#endif
    
    // Initiate SS/RESET and SCK as output
    bit_set(DDRB, Board::SS);
    bit_set(DDRB, Board::SCK);
    
    // And pulse a reset signal with clock low
    bit_set(PORTB, Board::SS);
    bit_clear(PORTB, Board::SCK);
    DELAY(50);
    bit_clear(PORTB, Board::SS);
    DELAY(50);

    // Initiate the other SPI pins
    bit_set(DDRB, Board::MOSI);
    bit_clear(DDRB, Board::MISO);
  }

  // Wait for the device, at least 20 ms, before Programming Enable instruction 
  Watchdog::delay(32);
  return (programming_enable());
}

void
Programmer::end()
{
  // Pull the reset high
  bit_set(PORTB, Board::SS);

  // And disconnect from the device; all pins as input
  bit_clear(DDRB, Board::MOSI);
  bit_clear(DDRB, Board::MISO);
  bit_clear(DDRB, Board::SCK);
  bit_clear(DDRB, Board::SS);
}

int 
Programmer::read_program_memory(uint8_t* dest, uint16_t src, size_t size) 
{
  for (uint16_t i = 0; i < size; i += 2) {
    *dest++ = read_program_memory_low_byte(src);
    *dest++ = read_program_memory_high_byte(src);
    src += 1;
  }
  return (size);
}

int 
Programmer::write_program_memory(uint16_t dest, uint8_t* src, size_t size)
{
  uint16_t page = get_flash_page(dest);
  for (uint16_t i = 0; i < size; i += 2) {
    if (page != get_flash_page(dest)) {
      write_program_memory_page(page);
      page = get_flash_page(dest);
    }
    load_program_memory_page_low_byte(dest, *src++);
    load_program_memory_page_high_byte(dest, *src++);
    dest += 1;
  }
  write_program_memory_page(page);
  return (size);
}

int 
Programmer::read_eeprom_memory(uint8_t* dest, uint16_t src, size_t size)
{
  for (uint16_t i = 0; i < size; i++) 
    *dest++ = read_eeprom_memory(src++);
  return (size);
}

int 
Programmer::write_eeprom_memory(uint16_t dest, uint8_t* src, size_t size)
{
  uint16_t page = get_eeprom_page(dest);
  for (uint16_t i = 0; i < size; i++) {
    if (page != get_eeprom_page(dest)) {
      write_eeprom_memory_page(page);
      page = get_eeprom_page(dest);
    }
    load_eeprom_memory_page(dest++, *src++);
  }
  write_eeprom_memory_page(page);
  return (size);
}

