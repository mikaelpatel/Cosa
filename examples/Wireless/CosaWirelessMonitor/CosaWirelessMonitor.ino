/**
 * @file CosaWirelessMonitor.ino
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
 * @section Description
 * Demonstration of the Wireless IOStream driver. Print incoming
 * messages as a character stream. Select 1) wireless device driver,
 * and 2) LCD or UART. For LCD select 3) port realization.
 *
 * @section Circuit
 * See Wireless drivers for circuit connections.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"

// Configuration; network and device addresses
#define NETWORK 0xC05A
#define DEVICE 0x01

// Select Wireless device driver
// #include <CC1101.h>
// CC1101 rf(NETWORK, DEVICE);

// #include <NRF24L01P.h>
// NRF24L01P rf(NETWORK, DEVICE);

// #include <RFM69.h>
// RFM69 rf(NETWORK, DEVICE);

#include <VWI.h>
// #include <BitstuffingCodec.h>
// BitstuffingCodec codec;
// #include <Block4B5BCodec.h>
// Block4B5BCodec codec;
// #include <HammingCodec_7_4.h>
// HammingCodec_7_4 codec;
// #include <HammingCodec_8_4.h>
// HammingCodec_8_4 codec;
// #include <ManchesterCodec.h>
// ManchesterCodec codec;
#include <VirtualWireCodec.h>
VirtualWireCodec codec;
#define SPEED 4000
#if defined(BOARD_ATTINY)
VWI rf(NETWORK, DEVICE, SPEED, Board::D1, Board::D0, &codec);
#else
VWI rf(NETWORK, DEVICE, SPEED, Board::D7, Board::D8, &codec);
#endif

static const uint8_t IOSTREAM_TYPE = 0x00;

// Select IOStream device
// #include "Cosa/IOStream/Driver/UART.hh"

// LCD and communication port
#include "Cosa/LCD/Driver/HD44780.hh"
// HD44780::Port4b port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4W port;
HD44780::MJKDZ port;
// HD44780::GYIICLCD port;
// HD44780::DFRobot port;
// HD44780 lcd(&port);
HD44780 lcd(&port, 20, 4);

const uint8_t bitmaps[] __PROGMEM = {
  // Bar(1)
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  // Bar(2)
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  // Bar(3)
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  // Bar(4)
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  // Bar(5)
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
};
const uint8_t SIZEOF_BITMAP = 8;
const uint8_t BITMAPS_MAX = membersof(bitmaps) / SIZEOF_BITMAP;

void setup()
{
  Watchdog::begin();
  RTC::begin();
#if defined(COSA_IOSTREAM_DRIVER_UART_HH)
  uart.begin(9600);
  trace.begin(&uart, PSTR("Monitor: started"));
#elif defined(COSA_LCD_DRIVER_HD44780_HH)
  lcd.begin();
  lcd.set_tab_step(2);
  lcd.cursor_underline_off();
  lcd.cursor_blink_off();
  for (char c = 0; c < BITMAPS_MAX; c++)
    lcd.set_custom_char_P(c, &bitmaps[c*SIZEOF_BITMAP]);
  trace.begin(&lcd, PSTR("\fMonitor: started"));
#endif
  rf.begin();
  sleep(1);
}

void loop()
{
  const static uint8_t PAYLOAD_MAX = 30;
  char buffer[PAYLOAD_MAX + 1];
  uint8_t src;
  uint8_t port;
  int count = rf.recv(src, port, buffer, PAYLOAD_MAX);
  if (count <= 0 || port != IOSTREAM_TYPE) return;
  for (uint8_t i = 0; i < count; i++) trace << buffer[i];
}
