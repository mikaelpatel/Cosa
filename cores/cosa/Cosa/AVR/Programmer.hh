/**
 * @file Cosa/AVR/Programmer.hh
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

#ifndef __COSA_AVR_PROGRAMMER_HH__
#define __COSA_AVR_PROGRAMMER_HH__

#include "Cosa/Types.h"
#include "Cosa/Bits.h"
#include "Cosa/Watchdog.hh"

/**
 * Serial programming using the SPI interface and RESET pin. Connect the 
 * device to the Arduino SPI pins; MOSI, MISO, SCK and SS/RESET. The member
 * functions implement the serial programming instruction set and
 * additional support functions for block read and write.
 */
class Programmer {
public:
  /**
   * Construct programmer with given page size for read/write of 
   * avr micro-controllers.
   * @param[in] pagesize number of words per program memory page.
   */
  Programmer(uint8_t pagesize = 32) : 
    m_flash_pagesize(pagesize),
    m_eeprom_pagesize(4)
  {
  }
  
  /**
   * Transfer data to and from the device.
   * @param[in] data to transfer.
   * @return data received.
   */
#if defined(USIDR)
  uint8_t transfer(uint8_t data)
  {
    USIDR = data;
    USISR = _BV(USIOIF);
    do {
      USICR = (_BV(USIWM0) | _BV(USICS1) | _BV(USICLK) | _BV(USITC));
    } while ((USISR & _BV(USIOIF)) == 0);
    return (USIDR);
  }
#else
  uint8_t transfer(uint8_t data) __attribute__((always_inline))
  {
    SPDR = data;
    loop_until_bit_is_set(SPSR, SPIF);
    return (SPDR);
  }
#endif

  /**
   * Transfer instruction to the device and receive possible value.
   * @param[in] ip pointer to instruction (4 bytes)
   * @return data received.
   */
  uint8_t transfer(uint8_t* ip)
  {
    transfer(ip[0]);
    transfer(ip[1]);
    transfer(ip[2]);
    return (transfer(ip[3]));
  }

  /**
   * Transfer instruction to the device and receive possible value.
   * @param[in] i0 to transfer (operation code).
   * @param[in] i1 to transfer (modifier or parameter).
   * @param[in] i2 to transfer (parameter).
   * @param[in] i3 to transfer (data).
   * @return data received.
   */
  uint8_t transfer(uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3)
  {
    transfer(i0);
    transfer(i1);
    transfer(i2);
    return (transfer(i3));
  }

  /**
   * Connect to the device over the SPI interface, reset and enable
   * programming. Returns true if the connection was successful
   * otherwise false.
   * @return bool.
   */
  bool begin();

  /**
   * Disconnect from the SPI interface.
   */
  void end();

  /**
   * Set the program memory page size in given number of bytes.
   * @param[in] bytes page size.
   */
  void set_flash_pagesize(uint8_t bytes)
  {
    m_flash_pagesize = bytes / 2;
  }

  /**
   * Set the eeprom memory page size in given number of bytes.
   * @param[in] bytes page size.
   */
  void set_eeprom_pagesize(uint8_t bytes)
  {
    m_eeprom_pagesize = bytes;
  }

  /**
   * Get the current program memory page size (number of words, 16-bits).
   * @return words page size.
   */
  uint16_t get_flash_page(uint16_t addr)
  {
    return (addr & ~(m_flash_pagesize - 1));
  }

  /**
   * Get the eeprom memory page size (number of bytes, 8-bits).
   * @return bytes page size.
   */
  uint16_t get_eeprom_page(uint16_t addr)
  {
    return (addr & ~(m_eeprom_pagesize - 1));
  }

  /**
   * Issue Programming Enable Serial Programming Instruction. Return
   * true if successful otherwise false.
   * @return bool.
   */
  bool programming_enable()
  { 
    transfer(0xAC);
    transfer(0x53);
    uint8_t res = transfer((uint8_t) 0x00); 
    transfer((uint8_t) 0x00); 
    return (res == 0x53);
  }
  
  /**
   * Issue Chip Erase (Program Memory/EEPROM) Serial Programming
   * Instruction. Waits for erase to complete.
   */
  void chip_erase() 
  { 
    transfer(0xAC, 0x80, 0x00, 0x00); 
    await();
  }

  /**
   * Issue Poll (RDY/BSY) Serial Programming Instruction and return
   * true if the device is busy otherwise false.
   * @return bool.
   */
  bool isbusy() 
  { 
    return (transfer(0xF0, 0x00, 0x00, 0x00) & 0x1); 
  }

  /**
   * Wait for the device to complete the issued instruction.
   */
  void await()
  {
    while (isbusy()) DELAY(1000);
  }

  /**
   * Issue Load Extended Address byte Serial Programming Instruction.
   * @param[in] addr extended program address.
   */
  void load_extended_address(uint8_t addr)
  {
    transfer(0x4D, 0x00, addr, 0x00);
  }

  /**
   * Issue Load Program Memory Page, High byte, Serial Programming
   * Instruction. 
   * @param[in] addr program word address.
   * @param[in] data high byte.
   */
  void load_program_memory_page_high_byte(uint8_t addr, uint8_t data)
  {
    transfer(0x48, 0x00, addr, data);
  }

  /**
   * Issue Load Program Memory Page, Low byte, Serial Programming
   * Instruction. 
   * @param[in] addr program word address.
   * @param[in] data low byte.
   */
  void load_program_memory_page_low_byte(uint8_t addr, uint8_t data)
  {
    transfer(0x40, 0x00, addr, data);
  }

  /**
   * Issue Load EEPROM Memory Page (page access) Serial Programming
   * Instruction. 
   * @param[in] addr eeprom byte address.
   * @param[in] data low byte.
   */
  void load_eeprom_memory_page(uint8_t addr, uint8_t data)
  {
    transfer(0xC1, 0x00, addr & 0x3, data);
  }

  /**
   * Issue Read Program Memory, High byte, Serial Programming
   * Instruction. 
   * @param[in] addr program word address.
   * @return byte read.
   */
  uint8_t read_program_memory_high_byte(uint16_t addr)
  {
    return (transfer(0x28, addr >> 8, addr, 0x00));
  }
  
  /**
   * Issue Read Program Memory, Low byte, Serial Programming
   * Instruction. 
   * @param[in] addr program word address.
   * @return byte read.
   */
  uint8_t read_program_memory_low_byte(uint16_t addr)
  {
    return (transfer(0x20, addr >> 8, addr, 0x00));
  }

  /**
   * Issue Read Program Memory, High and Low byte, Serial Programming
   * Instructions and return word (16-bit).
   * @param[in] addr program word address.
   * @return word read.
   */
  uint16_t read_program_memory(uint16_t addr)
  {
    univ16_t data;
    data.high = read_program_memory_high_byte(addr);
    data.low = read_program_memory_low_byte(addr);
    return (data.as_uint16);
  }

  /**
   * Issue Read EEPROM Memory Serial Programming Instruction. 
   * @param[in] addr eeprom byte address.
   * @return byte read.
   */
  uint8_t read_eeprom_memory(uint16_t addr)
  {
    return (transfer(0xA0, addr >> 8, addr, 0x00));
  }

  /**
   * Issue Read Lock bits Serial Programming Instruction. 
   * @return byte read.
   */
  uint8_t read_lock_bits()
  {
    return (transfer(0x58, 0x00, 0x00, 0x00));
  }

  /**
   * Issue Read Signature Byte Serial Programming Instruction with
   * given address.
   * @param[in] addr signature byte address (0..2).
   * @return byte read.
   */
  uint8_t read_signature_byte(uint8_t addr)
  {
    return (transfer(0x30, 0x00, addr & 0x3, 0x00));
  }

  /**
   * Issue Read Fuse bits Serial Programming Instruction. 
   * @return byte read.
   */
  uint8_t read_fuse_bits()
  {
    return (transfer(0x50, 0x00, 0x00, 0x00));
  }

  /**
   * Issue Read Fuse High bits Serial Programming Instruction. 
   * @return byte read.
   */
  uint8_t read_fuse_high_bits()
  {
    return (transfer(0x58, 0x08, 0x00, 0x00));
  }

  /**
   * Issue Read Extended Fuse bits Serial Programming Instruction. 
   * @return byte read.
   */
  uint8_t read_extended_fuse_bits()
  {
    return (transfer(0x50, 0x08, 0x00, 0x00));
  }

  /**
   * Issue Read Calibration byte Serial Programming Instruction. 
   * @return byte read.
   */
  uint8_t read_calibration_byte()
  {
    return (transfer(0x38, 0x00, 0x00, 0x00));
  }

  /**
   * Issue Write Program Memory Page Serial Programming Instruction
   * for given address.
   * @param[in] addr program word address.
   */
  void write_program_memory_page(uint16_t addr)
  {
    transfer(0x4C, addr >> 8, addr, 0x00);
    await();
  }

  /**
   * Issue Write EEPROM Memory Serial Programming Instruction at
   * given address with data.
   * @param[in] addr eeprom byte address (lsb).
   * @param[in] data to write.
   */
  void write_eeprom_memory(uint16_t addr, uint8_t data)
  {
    transfer(0xC0, addr >> 8, addr, data);
    await();
  }
    
  /**
   * Issue Write EEPROM Memory Page Serial Programming Instruction
   * for given address.
   * @param[in] page eeprom byte address (lsb).
   */
  void write_eeprom_memory_page(uint8_t page)
  {
    transfer(0xC2, page >> 8, page, 0x00);
    await();
  }

  /**
   * Issue Write Lock bits Serial Programming Instruction
   * for given data.
   * @param[in] page eeprom byte address (lsb).
   */
  void write_lock_bits(uint8_t data)
  {
    transfer(0xAC, 0xE0, 0x00, data);
    await();
  }

  /**
   * Issue Write Fuse bits Serial Programming Instruction for given data.
   * @param[in] data fuse.
   */
  void write_fuse_bits(uint8_t data)
  {
    transfer(0xAC, 0xA0, 0x00, data);
    await();
  }
    
  /**
   * Issue Write Fuse High bits Serial Programming Instruction for given data.
   * @param[in] data fuse.
   */
  void write_fuse_high_bits(uint8_t data)
  {
    transfer(0xAC, 0xA8, 0x00, data);
    await();
  }
    
  /**
   * Issue Write Extended Fuse bits Serial Programming Instruction for
   * given data. 
   * @param[in] data fuse.
   */
  void write_extended_fuse_bits(uint8_t data)
  {
    transfer(0xAC, 0xA4, 0x00, data);
    await();
  }

  /**
   * Read program memory from the given source word address to the
   * destination buffer with the given size in bytes. Return the
   * number of bytes read or negative error code.
   * @param[in] dest destination buffer pointer.
   * @param[in] src source program word address.
   * @param[in] size number of bytes to read.
   */
  int read_program_memory(uint8_t* dest, uint16_t src, size_t size);

  /**
   * Write program memory from the given source buffer with the given
   * size in bytes to the destination program word address. Return the 
   * number of bytes written or negative error code.
   * @param[in] dest destination program word address.
   * @param[in] src source buffer pointer.
   * @param[in] size number of bytes to write.
   */
  int write_program_memory(uint16_t dest, uint8_t* src, size_t size);

  /**
   * Read eeprom memory from the given source byte address to the
   * destination buffer with the given size in bytes. Return the
   * number of bytes read or negative error code.
   * @param[in] dest destination buffer pointer.
   * @param[in] src source eeprom memory byte address.
   * @param[in] size number of bytes to read.
   */
  int read_eeprom_memory(uint8_t* dest, uint16_t src, size_t size);

  /**
   * Write eeprom memory from the given source buffer with the given
   * size in bytes to the destination program word address. Return the 
   * number of bytes written or negative error code.
   * @param[in] dest destination eeprom memory byte address.
   * @param[in] src source buffer pointer.
   * @param[in] size number of bytes to write.
   */
  int write_eeprom_memory(uint16_t dest, uint8_t* src, size_t size);

private:
  /** Number of words (16-bits) per page */
  uint8_t m_flash_pagesize;

  /** Number of bytes (8-bits) per page */
  uint8_t m_eeprom_pagesize;
};

#endif
