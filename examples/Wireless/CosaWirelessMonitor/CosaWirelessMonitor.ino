/**
 * @file CosaWirelessMonitor.ino
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
 * @section Description
 * Demonstration of the Wireless IOStream driver. Print incoming
 * messages as a character stream. Select 1) wireless device driver,
 * and 2) LCD or UART. For LCD select 3) port realization. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"

// Select Wireless device driver (network = 0xC05A, device = 0x01)
// #include "Cosa/Wireless/Driver/CC1101.hh"
// CC1101 rf(0xC05A, 0x01);

#include "Cosa/Wireless/Driver/NRF24L01P.hh"
NRF24L01P rf(0xC05A, 0x01);

// #include "Cosa/Wireless/Driver/VWI.hh"
// #include "Cosa/Wireless/Driver/VWI/Codec/VirtualWireCodec.hh"
// VirtualWireCodec codec;
// #if defined(__ARDUINO_TINYX5__)
// VWI rf(0xC05A, 0x01, 4000, Board::D1, Board::D0, &codec);
// #else
// VWI rf(0xC05A, 0x01, 4000, Board::D7, Board::D8, &codec);
// #endif

static const uint8_t IOSTREAM_TYPE = 0x00;

// Select IOStream device
// #include "Cosa/IOStream/Driver/UART.hh"

// LCD and communication port
#include "Cosa/LCD/Driver/HD44780.hh"
// HD44780::Port4b port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4W port;
// HD44780::MJKDZ port;
HD44780::GYIICLCD port;
// HD44780::DFRobot port;
HD44780 lcd(&port);
// HD44780 lcd(&port, 20, 4);

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
#if defined(__COSA_IOSTREAM_DRIVER_UART_HH__)
  uart.begin(9600);
  trace.begin(&uart, PSTR("Monitor: started"));
#elif defined(__COSA_LCD_DRIVER_HD44780_HH__)
  lcd.begin();
  lcd.set_tab_step(2);
  lcd.cursor_underline_off();
  lcd.cursor_blink_off();
  for (char c = 0; c < BITMAPS_MAX; c++) 
    lcd.set_custom_char_P(c, &bitmaps[c*SIZEOF_BITMAP]);
  trace.begin(&lcd, PSTR("\fMonitor: started"));
#endif
  rf.begin();
  SLEEP(1);
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
