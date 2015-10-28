/**
 * @file CosaPCF8591.ino
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
 * @section Description
 * Cosa PCF8591 A/D, D/A conversion TWI driver example.
 *
 * @section Circuit
 * PCF8591 module with potentiometer(AIN0), photoresistor(AIN1)
 * and thermistor(AIN2).
 * @code
 *                          PCF8591
 *                       +------------+
 * (POT)---[ ]---------1-|AIN0     VCC|-16--------------(VCC)
 * (PHR)---[ ]---------2-|AIN1    AOUT|-15-------------(AOUT)
 * (THM)---[ ]---------3-|AIN2    VREF|-14--------------(VCC)
 * (AIN3)--------------4-|AIN3    AGND|-13--------------(GND)
 * (GND)---------------5-|A0       EXT|-12--------------(GND)
 * (GND)---------------6-|A1       OSC|-11
 * (GND)---------------7-|A2       SCL|-10-----------(SCL/A5)
 * (GND)---------------8-|GND      SDA|-9------------(SDA/A4)
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <PCF8591.h>

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Memory.h"

// The A/D, D/A converter
PCF8591 adc;

// Use the built-in led
OutputPin ledPin(Board::LED);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPCF8591: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(TWI));
  TRACE(sizeof(adc));

  // Start the watchdog ticks
  Watchdog::begin();
  RTT::begin();
}

void loop()
{
  periodic(timer, 2000) {
    ledPin.toggle();

    // Sample the four input channels and print values
    trace.print(PSTR("samples: "));
    for (uint8_t i = 0; i < 4; i++) {
      adc.begin(i | PCF8591::FOUR_INPUTS | PCF8591::OUTPUT_ENABLE);
      trace.print(adc.sample());
      trace.print(PSTR(" "));
      adc.end();
    }
    trace.println();

    // A sequence of samples from input channel(AIN2)
    uint8_t ain[16];
    adc.begin((PCF8591::AIN2) | PCF8591::FOUR_INPUTS | PCF8591::OUTPUT_ENABLE);
    TRACE(adc.sample(ain, sizeof(ain)));
    adc.end();
    trace.print(ain, sizeof(ain));

    // Put first value to output channel
    TRACE(adc.convert(ain[0]));

    ledPin.toggle();
  }
}
