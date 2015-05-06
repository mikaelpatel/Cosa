/**
 * @file CosaThingSpeakChannel.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
 * ThingSpeak channel demonstration; post temperature and humidity
 * reading from DHT11 sensor to ThingSpeak server.
 *
 * @section Circuit
 * This sketch is designed for the Ethernet Shield.
 *
 *                       W5100/ethernet
 *                       +------------+
 * (D10)--------------29-|CSN         |
 * (D11)--------------28-|MOSI        |
 * (D12)--------------27-|MISO        |
 * (D13)--------------30-|SCK         |
 * (D2)-----[ ]-------56-|IRQ         |
 *                       +------------+
 *
 *                       DHT11/sensor
 *                       +------------+
 * (VCC)---------------1-|VCC  ====== |
 * (D2/EXT0)-----------2-|DATA ====== |
 *                     3-|     ====== |
 * (GND)---------------4-|GND  ====== |
 *                       +------------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

// Uncomment to remove debug output
// #define NDEBUG

#include <DHT.h>
#include <DHCP.h>
#include <DNS.h>
#include <W5100.h>
#include <ThingSpeak.h>

#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"

#ifndef NDEBUG
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#else
#define TRACE(x) x
#endif

// Digital humidity and temperature sensor
#if defined(BOARD_ATMEGA2560)
DHT11 sensor(Board::EXT4);
#else
DHT11 sensor(Board::EXT0);
#endif

// Ethernet controller
static const char HOSTNAME[] __PROGMEM = "CosaThingSpeakChannel";
W5100 ethernet;

// Thingspeak client
static const char KEY[] __PROGMEM = "I3U14KLWM1R1SDPR";
ThingSpeak::Client client;

void setup()
{
#ifndef NDEBUG
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaThingSpeakChannel: started"));
#endif
  Watchdog::begin();
  RTC::begin();
  TRACE(ethernet.begin_P(HOSTNAME));
  TRACE(client.begin(ethernet.socket(Socket::TCP)));
}

void loop()
{
  ThingSpeak::Channel channel(&client, KEY);
  ThingSpeak::Entry update;
  sensor.sample();
  update.set_field(1, sensor.get_temperature(), 1);
  update.set_field(2, sensor.get_humidity(), 1);
  TRACE(channel.post(update));
  sleep(20);
}
