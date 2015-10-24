/**
 * @file CosaDHTMAX72XX.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * @section Description
 * Cosa demonstration of the DHT11 and MAX72XX 8-digit 7-segment
 * LED display drivers.
 *
 * @section Circuit
 * @code
 *                       DHT11/sensor
 *                       +------------+
 * (VCC)---------------1-|VCC  ====== |
 * (D2/EXT0)-----------2-|DATA ====== |
 *                     3-|     ====== |
 * (GND)---------------4-|GND  ====== |
 *                       +------------+
 *
 *                       MAX72XX/display
 *                       +--------------------------+
 * (GND)---------------1-|GND  ======  ...  ======  |
 * (VCC)---------------2-|VCC  =    =  ...  =    =  |
 * (MOSI/D11)----------3-|DIN  ======  ...  ======  |
 * (D9)----------------4-|CS   =    =  ...  =    =  |
 * (SCK/D13)-----------5-|CLK  ======  ...  ======  |
 *                       |      dig0   ...   dig7   |
 *                       +--------------------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <DHT.h>
#include <MAX72XX.h>

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/AnalogPin.hh"

// Humidity and temperature sensor
DHT11 sensor(Board::EXT0);

// 8-digit 7-segment LCD display with 3 wire SPI adapter
LCD::SPI3W port;
MAX72XX display(&port);

// Use the display as an iostream
IOStream cout(&display);

// Wall-clock
RTT::Clock clock;

void setup()
{
  Watchdog::begin();
  RTT::begin();
  display.begin();
}

void loop()
{
  // Run with a period of 5 seconds
  periodic(timer, 5000) {

    // Print timestamp; minutes and seconds
    cout << clear << clock.time() << 'E';
    sleep(1);

    // Print voltage
    cout << clear << AnalogPin::bandgap() / 1000.0 << 'U';
    sleep(1);

    // Sample sensor and display humidity and temperature
    int16_t humidity;
    int16_t temperature;
    if (sensor.sample(humidity, temperature)) {
      cout << clear
	   << humidity/10 << PSTR("H ")
	   << temperature/10 << PSTR("C ");
    }
    else {
      cout << clear << PSTR("Error");
    }
  }
}
