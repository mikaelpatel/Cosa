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

#ifndef __COSA_DRIVER_DHT11_HH__
#define __COSA_DRIVER_DHT11_HH__

#include "Cosa/Driver/DHT.hh"

/**
 * DHT11 Humidity & Temperature Sensor device driver. Subclass
 * and implement the event handler, on_event(), to allow periodic
 * read of device (attach to watchdog timeout queue). 
 *
 * @section Circuit
 * Connect DHT11 to pin, VCC and ground. A pullup resistor from
 * the pin to VCC should be used. Most DHT11 modules have a built-in 
 * pullup resistor.
 *
 * @section Limitations
 * The driver will turn off interrupt handling during data read 
 * from the device. 
 *
 * @section See Also
 * [1] DHT11 Humidity & Temperature Sensor, Robotics UK,
 * www.droboticsonline.com, http://www.micro4you.com/files/sensor/DHT11.pdf<br>
 * [2] DHTxx Sensor Tutorial, http://learn.adafruit.com/dht<br>
 */
class DHT11 : public DHT {
private:
  /**
   * @override
   * Adjust data from the device not needed for DHT11.
   */
  virtual void adjust_data() {}

public:
  /**
   * Construct connection to a DHT11 device on given in/output-pin.
   * Set humidity and temperature calibration offsets to zero.
   * @param[in] pin data.
   */
  DHT11(Board::DigitalPin pin) : DHT(pin) {}
};

#endif
