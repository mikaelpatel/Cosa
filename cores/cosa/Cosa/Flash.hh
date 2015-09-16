/**
 * @file Cosa/Flash.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#ifndef COSA_FLASH_HH
#define COSA_FLASH_HH

#include "Cosa/Types.h"

class Flash {
public:
  /**
   * Cosa Flash memory device driver interface.
   */
  class Device {
  public:
    /** Size of sector in bytes. */
    const uint32_t SECTOR_BYTES;

    /** Default size of sector in bytes. */
    static const uint32_t DEFAULT_SECTOR_BYTES = 4096;

    /** Sector address mask. */
    const uint32_t SECTOR_MASK;

    /** Number of sectors. */
    const uint16_t SECTOR_MAX;

    /** Number of bytes of device. */
    const uint32_t DEVICE_BYTES;

    /**
     * Construct flash memory device driver with given sector size and
     * count.
     * @param[in] size of sector in bytes.
     * @param[in] count number of sector.
     */
    Device(uint32_t bytes, uint16_t count) :
      SECTOR_BYTES(bytes),
      SECTOR_MASK(bytes - 1),
      SECTOR_MAX(count),
      DEVICE_BYTES(count * bytes)
    {}

    /**
     * @override{Flash::Device}
     * Initiate the flash memory device driver. Return true(1) if the
     * successful otherwise false(0).
     * @return bool.
     */
    virtual bool begin()
    {
      return (true);
    }

    /**
     * @override{Flash::Device}
     * Terminate the flash memory device driver. Return true(1) if the
     * successful otherwise false(0).
     * @return bool.
     */
    virtual bool end()
    {
      return (true);
    }

    /**
     * @override{Flash::Device}
     * Return true(1) if the device is ready, write cycle is completed,
     * otherwise false(0).
     * @return bool.
     */
    virtual bool is_ready() = 0;

    /**
     * @override{Flash::Device}
     * Read flash block with the given size into the buffer from the
     * source address. Return number of bytes read or negative error
     * code.
     * @param[in] dest buffer to read from flash into.
     * @param[in] src address in flash to read from.
     * @param[in] size number of bytes to read.
     * @return number of bytes or negative error code.
     */
    virtual int read(void* dest, uint32_t src, size_t size) = 0;

    /**
     * @override{Flash::Device}
     * Erase given flash block for given byte address. The actual
     * sector and number of bytes in sector is defined by the
     * implementation device. Returs zero(0) if successful otherwise
     * an negative error code(-1).
     * @param[in] dest destination block byte address to erase.
     * @param[in] size of sector to erase in Kbyte.
     * @return zero or negative error code.
     */
    virtual int erase(uint32_t dest, uint8_t size) = 0;

    /**
     * @override{Flash::Device}
     * Write flash block at given destination address with the contents
     * of the source buffer. Return number of bytes written or negative
     * error code.
     * @param[in] dest address in flash to write to.
     * @param[in] src buffer to write to flash.
     * @param[in] size number of bytes to write.
     * @return number of bytes or negative error code.
     */
    virtual int write(uint32_t dest, const void* src, size_t size) = 0;

    /**
     * @override{Flash::Device}
     * Write flash block at given destination address with contents
     * of the source buffer in program memory. Return number of bytes
     * written or negative error code.
     * @param[in] dest address in flash to write to.
     * @param[in] src buffer in program memory to write to flash.
     * @param[in] size number of bytes to write.
     * @return number of bytes written or EOF(-1).
     */
    virtual int write_P(uint32_t dest, const void* scr, size_t size) = 0;
  };
};

#endif
