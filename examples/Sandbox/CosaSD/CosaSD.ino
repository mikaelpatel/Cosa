/**
 * @file CosaSD.ino
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
 * @section Description
 * Demonstration and test of the SD card device driver.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Driver/SD.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"

SD sd;

void setup()
{
  uint8_t buf[SD::BLOCK_MAX];
  uint8_t save[16];

  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDS: started"));
  Watchdog::begin();
  RTC::begin();

  INFO("Connect to card and switch to a higher clock frequency", 0);
  ASSERT(sd.begin(SPI::DIV4_CLOCK));
  ASSERT(sd.get_type());

  INFO("Read block zero and print", 0);
  ASSERT(sd.read(0, buf));
  trace.print(buf, sizeof(buf), IOStream::hex);

  INFO("Modify first 16 bytes and write to card", 0);
  for (uint8_t i = 0; i < sizeof(save); i++) {
    save[i] = buf[i];
    buf[i] = 0xa5;
  }
  ASSERT(sd.write(0, buf));

  INFO("Read back and restore content", 0);
  ASSERT(sd.read(0, buf));
  trace.print(buf, sizeof(buf), IOStream::hex);
  for (uint8_t i = 0; i < sizeof(save); i++) {
    buf[i] = save[i];
  }
  ASSERT(sd.write(0, buf));
  
  INFO("Read four first blocks and print", 0);
  for (uint8_t block = 0; block < 4; block++) {
    TRACE(block);
    ASSERT(sd.read(block, buf));
    trace.print(buf, sizeof(buf), IOStream::hex);
  }

  ASSERT(sd.end());
}

void loop()
{
  ASSERT(true == false);
}
