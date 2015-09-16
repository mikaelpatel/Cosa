/**
 * @file Cosa/EEPROM.hh
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

#ifndef COSA_EEPROM_HH
#define COSA_EEPROM_HH

#include "Cosa/Types.h"
#include "Cosa/Power.hh"

/**
 * Driver for the ATmega/ATtiny internal EEPROM and abstraction of
 * EEPROM devices. See AT24CXX for an example of driver for external
 * EEPROM memory. The default device is the internal EEPROM. The class
 * EEPROM delegates to the EEPROM:Device class instance.
 */
class EEPROM {
public:
  /**
   * EEPROM Device abstraction; default device is the processors
   * internal EEPROM data memory. New devices should sub-class and
   * implement virtual methods.
   */
  class Device {
  public:
    /**
     * @override{EEPROM::Device}
     * Return true(1) if the device is ready, write cycle is completed,
     * otherwise false(0).
     * @return bool.
     */
    virtual bool is_ready();

    /**
     * @override{EEPROM::Device}
     * Read rom block with the given size into the buffer from the address.
     * Return number of bytes read or negative error code.
     * @param[in] dest buffer to read from rom into.
     * @param[in] src address in rom to read from.
     * @param[in] size number of bytes to read.
     * @return number of bytes or negative error code.
     */
    virtual int read(void* dest, const void* src, size_t size);

    /**
     * @override{EEPROM::Device}
     * Write rom block at given address with the contents from the buffer.
     * Return number of bytes written or negative error code.
     * @param[in] dest address in rom to write to.
     * @param[in] src buffer to write to rom.
     * @param[in] size number of bytes to write.
     * @return number of bytes or negative error code.
     */
    virtual int write(void* dest, const void* src, size_t size);

    /**
     * Default EEPROM device; handling of internal EEPROM Data Memory.
     */
    static Device eeprom;
  };

public:
  /**
   * Construct access object for EEPROM given device. Default device
   * is the internal EEPROM memory.
   * @param[in] dev device.
   */
  EEPROM(Device* dev = &Device::eeprom) : m_dev(dev) {}

  /**
   * Return true(1) if the device is ready, write cycle is completed,
   * otherwise false(0).
   * @return bool.
   */
  bool is_ready()
    __attribute__((always_inline))
  {
    return (m_dev->is_ready());
  }

  /**
   * Wait for write to complete.
   * @param[in] mode of sleep.
   */
  void write_await()
    __attribute__((always_inline))
  {
    while (!is_ready()) yield();
  }

  /**
   * Read rom block with the given size into the buffer from the address.
   * Return number of bytes read or negative error code.
   * @param[out] dest buffer to write to.
   * @param[in] src address in rom to read from.
   * @param[in] size number of bytes to read.
   * @return number of bytes or negative error code.
   */
  int read(void* dest, const void* src, size_t size)
    __attribute__((always_inline))
  {
    return (m_dev->read(dest, src, size));
  }

  /**
   * Template function to read a rom block with the given type to
   * given value reference. Returns number of bytes read or negative error code.
   * @param[out] dest address variable.
   * @param[in] src address in rom to read from.
   * @return number of bytes or negative error code.
   */
  template<class T> int read(T* dest, const T* src)
  {
    return (m_dev->read(dest, src, sizeof(T)));
  }

  int read(unsigned char* dest, const unsigned char* src)
    __attribute__((always_inline))
  {
    return (m_dev->read(dest, src, sizeof(unsigned char)));
  }

  int read(unsigned short* dest, const unsigned short* src)
    __attribute__((always_inline))
  {
    return (m_dev->read(dest, src, sizeof(unsigned short)));
  }

  int read(unsigned int* dest, const unsigned int* src)
    __attribute__((always_inline))
  {
    return (m_dev->read(dest, src, sizeof(unsigned int)));
  }

  int read(unsigned long* dest, const unsigned long* src)
    __attribute__((always_inline))
  {
    return (m_dev->read(dest, src, sizeof(unsigned long)));
  }

  int read(char* dest, const char* src)
    __attribute__((always_inline))
  {
    return (m_dev->read(dest, src, sizeof(char)));
  }

  int read(short* dest, const short* src)
    __attribute__((always_inline))
  {
    return (m_dev->read(dest, src, sizeof(short)));
  }

  int read(int* dest, const int* src)
    __attribute__((always_inline))
  {
    return (m_dev->read(dest, src, sizeof(int)));
  }

  int read(long* dest, const long* src)
    __attribute__((always_inline))
  {
    return (m_dev->read(dest, src, sizeof(long)));
  }

  int read(float* dest, const float* src)
    __attribute__((always_inline))
  {
    return (m_dev->read(dest, src, sizeof(float)));
  }

  /**
   * Write rom block at given address with the contents from the buffer.
   * Return number of bytes written or negative error code.
   * @param[out] dest address in rom to read write to.
   * @param[in] src buffer to write to rom.
   * @param[in] size number of bytes to write.
   * @return number of bytes or negative error code.
   */
  int write(void* dest, const void* src, size_t size)
    __attribute__((always_inline))
  {
    return (m_dev->write(dest, src, size));
  }

  /**
   * Write rom block at given address with the given value of given
   * data type. Returns number of bytes written or negative error code.
   * @param[in] dest address in rom to read write to.
   * @param[in] src buffer to write to rom.
   * @param[in] size number of bytes to write.
   * @return number of bytes or negative error code.
   */
  template<class T> int write(T* dest, const T* src)
  {
    return (m_dev->write(dest, src, sizeof(T)));
  }

  int write(unsigned char* dest, unsigned char src)
    __attribute__((always_inline))
  {
    return (m_dev->write(dest, &src, sizeof(unsigned char)));
  }

  int write(unsigned short* dest, unsigned short src)
    __attribute__((always_inline))
  {
    return (m_dev->write(dest, &src, sizeof(unsigned short)));
  }

  int write(unsigned int* dest, unsigned int src)
    __attribute__((always_inline))
  {
    return (m_dev->write(dest, &src, sizeof(unsigned int)));
  }

  int write(unsigned long* dest, unsigned long src)
    __attribute__((always_inline))
  {
    return (m_dev->write(dest, &src, sizeof(unsigned long)));
  }

  int write(char* dest, char src)
    __attribute__((always_inline))
  {
    return (m_dev->write(dest, &src, sizeof(char)));
  }

  int write(short* dest, short src)
    __attribute__((always_inline))
  {
    return (m_dev->write(dest, &src, sizeof(short)));
  }

  int write(int* dest, int src)
    __attribute__((always_inline))
  {
    return (m_dev->write(dest, &src, sizeof(int)));
  }

  int write(long* dest, long src)
    __attribute__((always_inline))
  {
    return (m_dev->write(dest, &src, sizeof(long)));
  }

  int write(float* dest, float src)
    __attribute__((always_inline))
  {
    return (m_dev->write(dest, &src, sizeof(float)));
  }

private:
  Device* m_dev;		//!< Delegated device.
};

#endif
