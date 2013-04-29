/**
 * @file Cosa/Driver/DHT22.hh
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

#ifndef __COSA_DRIVER_DHT22_HH__
#define __COSA_DRIVER_DHT22_HH__

#include "Cosa/Driver/DHT11.hh"

/**
 * DHT22 Humidity & Temperature Sensor device driver. Subclass
 * and implement the event handler, on_event(), to allow periodic
 * read of device (attach to watchdog timeout queue). Note that the
 * values read from the device are scaled by a factor of 10, i.e. one
 * decimal accurracy. 
 *
 * @section Circuit
 * Connect DHT22 to pin, VCC and ground. A pullup resistor from
 * the pin to VCC should be used. Most DHT22 modules have a built-in 
 * pullup resistor.
 *
 * @section Limitations
 * The driver will turn off interrupt handling during data read 
 * from the device. 
 *
 * @section See Also
 * [1] http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Sensors/Weather/RHT03.pdf<br>
 */

class DHT22 : public DHT11 {
private:
  /**
   * @override
   * Adjust data from the device. Byte order and representation of 
   * negative temperature values.
   */
  virtual void adjust_data() 
  {
    m_data.humidity = swap(m_data.humidity);
    m_data.temperature = swap(m_data.temperature);
    if (m_data.temperature < 0) {
      m_data.temperature = -(m_data.temperature & 0x7ff);
    }
  }

public:
  /**
   * Construct connection to a DHT22 device on given in/output-pin.
   * Set humidity and temperature calibration offsets to zero.
   * @param[in] pin data.
   */
  DHT22(Board::DigitalPin pin) : DHT11(pin) {}
};

#endif
