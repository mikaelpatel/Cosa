/**
 * @file Cosa/Driver/DHT.hh
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_DRIVER_DHT_HH__
#define __COSA_DRIVER_DHT_HH__

#include "Cosa/Types.h"
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/Linkage.hh"
#include "Cosa/Power.hh"
#include "Cosa/IOStream.hh"

/**
 * DHT11/22 Humidity & Temperature Sensor abstract device driver. 
 */
class DHT : public ExternalInterrupt, public Link {
protected:
  // States
  enum {
    INIT,			// Initial state
    IDLE,			// Periodic wait
    REQUEST,			// Issued a request
    RESPONSE,			// Waiting for response
    SAMPLING,			// Collecting samples
    COMPLETED			// Data transfer completed
  } __attribute__((packed));

  /** Minimum periodic wait */
  static const uint16_t MIN_PERIOD = 2048;

  /** Sample thresholds */
  static const uint16_t LOW_THRESHOLD = 50;
  static const uint16_t BIT_THRESHOLD = 100;
  static const uint16_t HIGH_THRESHOLD = 200;

  /** Size of data buffer */
  static const uint8_t DATA_MAX = 5;

  /** Last data elemement index */
  static const uint8_t DATA_LAST = DATA_MAX - 1;

  /**
   * Data read from the device. Allow mapping between received byte 
   * vector and data fields.
   */
  union data_t {
    uint8_t as_byte[DATA_MAX];
    struct {
      int16_t humidity;
      int16_t temperature;
      uint8_t chksum;
    };
  };
  
  /** State of device driver */
  volatile uint8_t m_state;

  /** Number of errors detected; transfer or checksum */
  volatile uint8_t m_errors;

  /** Micro-seconds since latest rising of data signal; pulse start */
  volatile uint32_t m_start;

  /** Number of milli-seconds between requests */
  volatile uint16_t m_period;

  /** Current byte being read from device */
  volatile uint8_t m_value;

  /** Current number of bits read */
  volatile uint8_t m_bits;

  /** Current data byte index stream */
  volatile uint8_t m_ix;

  /** Current data being transfered */
  data_t m_data;

  /* Latest valid reading */
  data_t m_latest;
  
  /**
   * @override
   * The device driver interrupt level state machine.
   * @param[in] arg argument from interrupt service routine.
   */
  virtual void on_interrupt(uint16_t arg = 0);

  /**
   * @override
   * The device driver event level state machine.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);

  /**
   * Adjust data from the device. Communication protocol is the same
   * for the DHT device family but data representation is different,
   * i.e. data resolution and accuracy. Overridden by DHT11 and DHT22.
   */
  virtual void adjust_data() {}

public:  
  /**
   * Construct DHT device connected to given pin.
   * @param[in] pin external interrupt pin (Default EXT0).
   */
  DHT(Board::ExternalInterruptPin pin = Board::EXT0) : 
    ExternalInterrupt(pin, ExternalInterrupt::ON_RISING_MODE),
    Link(),
    m_state(INIT),
    m_errors(0),
    m_start(0L),
    m_period(0),
    m_value(0),
    m_bits(0),
    m_ix(0)
  {
  }
  
  /**
   * Start the DHT device driver with the given sampling period.
   * @param[in] ms sampling period (Default 2048 ms).
   */
  void begin(uint16_t ms = MIN_PERIOD);

  /**
   * @override
   * Callback when data sample is completed.
   */
  virtual void on_sample_completed() {}

  /**
   * Stop the DHT device driver.
   */
  void end();

  /**
   * Return temperature from latest read.
   * @return temperature.
   */
  int16_t get_temperature()
  {
    return (m_latest.temperature);
  }

  /**
   * Return humidity from latest read.
   * @return humidity.
   */
  int16_t get_humidity()
  {
    return (m_latest.humidity);
  }

  /**
   * Initiate a sample request from the device. Return true(1) and
   * values if successful otherwise false(0).  
   * @return bool.
   */
  bool sample_request();

  /**
   * Wait for a sample request to complete. Return true(1) and
   * values if successful otherwise false(0).  
   * @return bool.
   */
  bool sample_await(uint8_t mode = SLEEP_MODE_IDLE);

  /**
   * Read temperature and humidity from the device. Return true(1) and
   * values if successful otherwise false(0).  
   * @return bool.
   */
  bool sample()
  {
    return (sample_request() && sample_await());
  }

  /**
   * Read temperature and humidity from the device. Return true(1) and
   * values if successful otherwise false(0).  
   * @param[out] humidity reading.
   * @param[out] temperature reading.
   * @return bool.
   */
  bool sample(int16_t& humidity, int16_t& temperature)
  {
    if (!sample()) return (false);
    humidity = get_humidity();
    temperature = get_temperature();
    return (true);
  }

  /**
   * Print latest humidity and temperature reading to the
   * given stream.
   * @param[in] outs output stream.
   * @param[in] dht device to print.
   * @return stream.
   */
  friend IOStream& operator<<(IOStream& outs, DHT& dht);
};

/**
 * DHT11 Humidity & Temperature Sensor device driver. 
 *
 * @section Circuit
 * Connect DHT11 to pin, VCC and ground. A pullup resistor from
 * the pin to VCC should be used. Most DHT11 modules have a built-in 
 * pullup resistor.
 *
 * @section See Also
 * [1] DHT11 Humidity & Temperature Sensor, Robotics UK,
 * www.droboticsonline.com, http://www.micro4you.com/files/sensor/DHT11.pdf<br>
 * [2] DHTxx Sensor Tutorial, http://learn.adafruit.com/dht<br>
 */
class DHT11 : public DHT {
public:
  /**
   * @override
   * Adjust data from the DHT11 device; scale by 10 for uniform 
   * number range as DHT22.
   */
  virtual void adjust_data();

  /**
   * Construct connection to a DHT11 device on given in/output-pin.
   * Set humidity and temperature calibration offsets to zero.
   * @param[in] pin data.
   */
  DHT11(Board::ExternalInterruptPin pin = Board::EXT0) : DHT(pin) {}
};

/**
 * DHT22 Humidity & Temperature Sensor device driver. Note that the
 * values read from the device are scaled by a factor of 10, i.e. one
 * decimal accurracy. 
 *
 * @section Circuit
 * Connect DHT22 to pin, VCC and ground. A pullup resistor from
 * the pin to VCC should be used. Most DHT22 modules have a built-in 
 * pullup resistor.
 *
 * @section See Also
 * [1] http://www.humiditycn.com/pic/20135318405067570.pdf
 */
class DHT22 : public DHT {
private:
  /**
   * @override
   * Adjust data from the DHT22 device. Byte order and representation of 
   * negative temperature values.
   */
  virtual void adjust_data();

public:
  /**
   * Construct connection to a DHT22 device on given in/output-pin.
   * Set humidity and temperature calibration offsets to zero.
   * @param[in] pin data.
   */
  DHT22(Board::ExternalInterruptPin pin = Board::EXT0) : DHT(pin) {}
};
#endif
