/**
 * @file OWI.hh
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

#ifndef COSA_OWI_HH
#define COSA_OWI_HH

#include "Cosa/Types.h"
#include "Cosa/IOPin.hh"
#include "Cosa/IOStream.hh"

/**
 * 1-wire device driver support class. Allows device rom search
 * and connection to multiple devices on one-wire bus.
 *
 * @section Limitations
 * The driver will turn off interrupt handling during data read
 * from the device.
 */
class OWI : private IOPin {
public:
  /**
   * Standard ROM Commands.
   */
  enum {
    SEARCH_ROM = 0xF0,
    READ_ROM = 0x33,
    MATCH_ROM = 0x55,
    SKIP_ROM = 0xCC,
    ALARM_SEARCH = 0xEC
  } __attribute__((packed));

  /** ROM size in bytes. */
  static const uint8_t ROM_MAX = 8;

  /** ROM size in bits. */
  static const uint8_t ROMBITS = ROM_MAX * CHARBITS;

  /**
   * Driver for device connected to a one-wire bus.
   */
  class Driver {
  public:
    enum {
      FIRST = -1,
      ERROR = -1,
      LAST = ROMBITS
    } __attribute__((packed));

    /** Name of device driver instance. */
    str_P NAME;

    /**
     * Construct one wire device driver. Use one wire bus on given pin.
     * @param[in] pin one wire bus.
     * @param[in] name of device driver instance.
     */
    Driver(OWI* pin, const char* name = NULL) :
      NAME((str_P) name),
      ROM(NULL),
      m_next(NULL),
      m_pin(pin)
    {
      memset(m_rom, 0, sizeof(m_rom));
    }

    /**
     * Construct one wire device driver. Use one wire bus on given pin,
     * given rom identity in EEPROM (or null), and device name.
     * @param[in] pin one wire bus.
     * @param[in] rom identity.
     * @param[in] name of device driver instance.
     */
    Driver(OWI* pin, const uint8_t* rom, const char* name = NULL);

    /**
     * Return pointer to device rom.
     * @return device rom buffer.
     */
    uint8_t* rom()
    {
      return (m_rom);
    }

    /**
     * Update the rom identity (in EEPROM). Return true(1) if
     * successful otherwise false(0). Typically used to save
     * configuration after connect().
     * return bool.
     */
    bool update_rom();

    /**
     * Search device rom given the last position of discrepancy.
     * Negative value for start from the beginning.
     * @param[in] last position of discrepancy.
     * @return position of difference or negative error code.
     */
    int8_t search_rom(int8_t last = FIRST);

    /**
     * Read device rom. This can only be used when there is only
     * one slave on the bus.
     * @return true(1) if successful otherwise false(0).
     */
    bool read_rom();

    /**
     * Match device rom. Address the slave device with the
     * rom code. Device specific function command should follow.
     * May be used to verify rom code.
     * @return true(1) if successful otherwise false(0).
     */
    bool match_rom();

    /**
     * Skip device rom for boardcast or single device access.
     * Device specific function command should follow.
     * @return true(1) if successful otherwise false(0).
     */
    bool skip_rom();

    /**
     * Search alarming device given the last position of discrepancy.
     * Negative value for start from the beginning.
     * @param[in] last position of discrepancy.
     * @return position of difference or negative error code.
     */
    int8_t alarm_search(int8_t last = FIRST);

    /**
     * Connect to one-wire device with given family code and index.
     * @param[in] family device family code.
     * @param[in] index device order.
     * @return true(1) if successful otherwise false(0).
     */
    bool connect(uint8_t family, uint8_t index);

    /**
     * @override{OWI::Driver}
     * Callback on alarm dispatch. Default is empty function.
     */
    virtual void on_alarm() {}

  protected:
    uint8_t m_rom[ROM_MAX];
    const uint8_t* ROM;
    Driver* m_next;
    OWI* m_pin;

    /**
     * Search device rom given the last position of discrepancy.
     * Negative value for start from the beginning.
     * @param[in] last position of discrepancy.
     * @return position of difference or negative error code.
     */
    int8_t search(int8_t last = FIRST);

    friend class OWI;
    friend IOStream& operator<<(IOStream& outs, OWI& owi);
    friend IOStream& operator<<(IOStream& outs, Driver& dev);
  };

  /**
   * Alarm search iterator class.
   */
  class Search : protected Driver {
  public:
    /**
     * Initiate an alarm search iterator for the given one-wire bus and
     * device family code.
     * @param[in] owi one-wire bus.
     * @param[in] family code (default all).
     */
    Search(OWI* owi, uint8_t family = 0) :
      Driver(owi),
      m_family(family),
      m_last(FIRST)
    {}

    /**
     * Get the next device with an active alarm.
     * @return pointer to driver or null(0).
     */
    Driver* next();

    /**
     * Reset iterator.
     */
    void reset()
    {
      m_last = FIRST;
    }

  protected:
    uint8_t m_family;
    int8_t m_last;
  };

public:
  /**
   * Construct one wire bus connected to the given pin.
   * @param[in] pin number.
   */
  OWI(Board::DigitalPin pin) :
    IOPin(pin),
    m_devices(0),
    m_device(NULL),
    m_crc(0)
  {}

  /**
   * Reset the one wire bus and check that at least one device is
   * presence.
   * @return true(1) if successful otherwise false(0).
   */
  bool reset();

  /**
   * Read the given number of bits from the one wire bus (slave).
   * Default number of bits is 8. Returns the value read LSB aligned.
   * @param[in] bits to be read.
   * @return value read.
   */
  uint8_t read(uint8_t bits = CHARBITS);

  /**
   * Read given number of bytes from one wire bus (slave) to given
   * buffer. Return true(1) if correctly read otherwise false(0).
   * @param[in] buf buffer pointer.
   * @param[in] size number of bytes to read.
   * @return bool.
   */
  bool read(void* buf, uint8_t size);

  /**
   * Write the given value to the one wire bus. The bits are written
   * from LSB to MSB. Pass true(1) for power parameter to allow
   * parasite devices to be powered. Should be turned off with power_off().
   * @param[in] value to write.
   * @param[in] bits to be written.
   * @param[in] power on for parasite device.
   */
  void write(uint8_t value, uint8_t bits = CHARBITS, bool power = false);

  /**
   * Write the given value and given number of bytes from buffer to
   * the one wire bus (slave).
   * @param[in] value to write.
   * @param[in] buf buffer pointer.
   * @param[in] size number of bytes to write.
   */
  void write(uint8_t value, void* buf, uint8_t size);

  /**
   * Turn off parasite powering of pin. See also write().
   */
  void power_off()
    __attribute__((always_inline))
  {
    mode(INPUT_MODE);
    clear();
  }

  /**
   * Lookup the driver instance with the given rom address.
   * @return driver pointer or null(0).
   */
  Driver* lookup(uint8_t* rom);

  /**
   * Search drivers with alarm setting and call on_alarm().
   * Return true(1) if there was at least one driver with an alarm,
   * otherwise when no alarms false(0).
   * @return bool.
   */
  bool alarm_dispatch();

private:
  /** Number of devices. */
  uint8_t m_devices;

  /** List of slave devices. */
  Driver* m_device;

  /** Intermediate CRC sum. */
  uint8_t m_crc;
};

/**
 * Print device driver name and rom to output stream.
 * @param[in] outs stream to print to.
 * @param[in] dev owi device driver.
 * @return output stream.
 */
IOStream& operator<<(IOStream& outs, OWI::Driver& dev);

/**
 * Print list of connected devices on given stream.
 * @param[in] outs stream to print device information to.
 * @param[in] owi one-wire bus.
 * @return output stream.
 */
IOStream& operator<<(IOStream& outs, OWI& owi);

#endif

