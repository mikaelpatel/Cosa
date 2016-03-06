/**
 * @file DHT.hh
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

#ifndef COSA_DHT_HH
#define COSA_DHT_HH

#include "Cosa/Types.h"
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/IOStream.hh"

/**
 * DHT11/22 Humidity & Temperature Sensor common device driver.
 * Uses external interrupt on high to low transition to capture
 * serial data from the device. Please note that excessive interrupt
 * sources may affect the capture.
 */
class DHT : public ExternalInterrupt {
public:
  /** Initial humidity; 100.0 % RH. */
  static const int16_t INIT_HUMIDITY_SAMPLE = 1000;

  /** Initial temperature; 85 C. */
  static const int16_t INIT_TEMPERATURE_SAMPLE = 850;

  /**
   * Construct DHT device connected to given pin.
   * @param[in] pin external interrupt pin (Default EXT0).
   */
  DHT(Board::ExternalInterruptPin pin = Board::EXT0) :
    ExternalInterrupt(pin, ExternalInterrupt::ON_FALLING_MODE),
    m_state(INIT),
    m_start(0),
    m_value(0),
    m_bits(0),
    m_ix(0),
    m_humidity(INIT_HUMIDITY_SAMPLE),
    m_temperature(INIT_TEMPERATURE_SAMPLE)
  {}

  /**
   * Return humidity from latest read.
   * @return humidity.
   */
  int16_t humidity() const
  {
    return (m_humidity);
  }

  /**
   * Return temperature from latest read.
   * @return temperature.
   */
  int16_t temperature() const
  {
    return (m_temperature);
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
  bool sample_await();

  /**
   * Read temperature and humidity from the device. Return true(1) and
   * values if successful otherwise false(0).
   * @return bool.
   */
  bool sample()
    __attribute__((always_inline))
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
    __attribute__((always_inline))
  {
    bool res = sample();
    humidity = m_humidity;
    temperature = m_temperature;
    return (res);
  }

protected:
  /**
   * @override{Interrupt::Handler}
   * The device driver interrupt level state machine.
   * @param[in] arg argument from interrupt service routine.
   */
  virtual void on_interrupt(uint16_t arg = 0);

  /**
   * @override{DHT}
   * Callback when data sample is completed. Called from interrupt
   * service routine. Typically used to push an event for further
   * processing. Default implementation is an empty function.
   * @param[in] valid data received and adjusted.
   */
  virtual void on_sample_completed(bool valid)
  {
    UNUSED(valid);
  }

  /**
   * Validate received data block. Return true if valid data with
   * correct check sum otherwise false.
   * @return bool.
   */
  bool is_valid();

  /**
   * @override{DHT}
   * Adjust data from the device. Communication protocol is the same
   * for the DHT device family but data representation is different,
   * i.e. data resolution and accuracy. Overridden by DHT11 and DHT22.
   */
  virtual void adjust_data() = 0;

  // States
  enum {
    INIT,			//!< Initial state.
    IDLE,			//!< Periodic wait.
    REQUEST,			//!< Issued a request.
    RESPONSE,			//!< Waiting for response.
    SAMPLING,			//!< Collecting samples.
    COMPLETED			//!< Data transfer completed.
  } __attribute__((packed));

  /** Minimum periodic wait (approx. 2 seconds). */
  static const uint16_t MIN_PERIOD = 2048;

  /** Sample thresholds. */
  static const uint16_t LOW_THRESHOLD = 50;
  static const uint16_t BIT_THRESHOLD = 100;
  static const uint16_t HIGH_THRESHOLD = 200;

  /** Size of data buffer */
  static const uint8_t DATA_MAX = 5;

  /** Last data element index */
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

  /** State of device driver. */
  volatile uint8_t m_state;

  /** Micro-seconds since latest rising of data signal; pulse start. */
  uint16_t m_start;

  /** Current byte being read from device. */
  uint8_t m_value;

  /** Current number of bits read. */
  uint8_t m_bits;

  /** Current data byte index stream. */
  uint8_t m_ix;

  /** Current data being transfered. */
  data_t m_data;

  /* Latest valid humidity reading. */
  int16_t m_humidity;

  /* Latest valid temperature reading. */
  int16_t m_temperature;

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
 * @code
 *                           DHT11
 *                       +------------+
 * (VCC)---------------1-|VCC  ====== |
 * (D2/EXT0)-----------2-|DATA ====== |
 *                     3-|     ====== |
 * (GND)---------------4-|GND  ====== |
 *                       +------------+
 * @endcode
 * Connect DHT11 to pin, VCC and ground. A pullup resistor from
 * the pin to VCC should be used. Most DHT11 modules have a built-in
 * pullup resistor.
 *
 * @section References
 * 1. DHT11 Humidity & Temperature Sensor, Robotics UK,
 * www.droboticsonline.com, http://www.micro4you.com/files/sensor/DHT11.pdf<br>
 * 2. DHTxx Sensor Tutorial, http://learn.adafruit.com/dht<br>
 */
class DHT11 : public DHT {
public:
  /**
   * Construct connection to a DHT11 device on given in/output-pin.
   * @param[in] pin data (Default EXT0).
   */
  DHT11(Board::ExternalInterruptPin pin = Board::EXT0) :
    DHT(pin)
  {
  }

protected:
  /**
   * @override{DHT}
   * Adjust data from the DHT11 device; scale by 10 for uniform
   * number range as DHT22.
   */
  virtual void adjust_data();
};

/**
 * DHT22 Humidity & Temperature Sensor device driver. Note that the
 * values read from the device are scaled by a factor of 10, i.e. one
 * decimal accurracy.
 *
 * @section Circuit
 * @code
 *                           DHT22
 *                       +------------+
 * (VCC)---------------1-|VCC  ====== |
 * (D2/EXT0)-----------2-|DATA ====== |
 *                     3-|     ====== |
 * (GND)---------------4-|GND  ====== |
 *                       +------------+
 * @endcode
 * Connect DHT22 to pin, VCC and ground. A pullup resistor from
 * the pin to VCC should be used. Most DHT22 modules have a built-in
 * pullup resistor.
 *
 * @section References
 * 1. http://www.humiditycn.com/pic/20135318405067570.pdf
 */
class DHT22 : public DHT {
public:
  /**
   * Construct connection to a DHT22 device on given in/output-pin.
   * @param[in] pin data (Default EXT0).
   */
  DHT22(Board::ExternalInterruptPin pin = Board::EXT0) :
    DHT(pin)
  {
  }

protected:
  /**
   * @override{DHT}
   * Adjust data from the DHT22 device. Byte order and representation of
   * negative temperature values.
   */
  virtual void adjust_data();
};
#endif
