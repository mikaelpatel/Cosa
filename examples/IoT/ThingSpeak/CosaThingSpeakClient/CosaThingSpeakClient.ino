/**
 * @file CosaThingSpeakClient.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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
 * ThingSpeak client demonstration; post temperature and humidity
 * reading from DHT11 sensor to ThingSpeak server.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/Driver/DHT.hh"
#include "Cosa/IoT/ThingSpeak.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Socket/Driver/W5100.hh"

// Digital humidity and temperature sensor
DHT11 sensor(Board::EXT0);

// Ethernet controller
static const char HOSTNAME[] __PROGMEM = "CosaThingSpeakClient";
#define MAC 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed 
static const uint8_t mac[6] __PROGMEM = { MAC };
W5100 ethernet(mac);

// Thingspeak client
static const char KEY[] __PROGMEM = "I3U14KLWM1R1SDPR";
ThingSpeak::Client client;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaThingSpeakClient: started"));
  Watchdog::begin();
  RTC::begin();
  ASSERT(ethernet.begin_P(HOSTNAME));
  ASSERT(client.begin(ethernet.socket(Socket::TCP)));
}

void loop()
{
  ThingSpeak::Channel channel(&client, KEY);
  IOBuffer<64> buf;
  IOStream cout(&buf);
  sensor.sample();
  cout << PSTR("field1=") << sensor.get_temperature() / 10
       << PSTR("&field2=") << sensor.get_humidity() / 10
       << ends;
  ASSERT(!channel.post(buf));
  SLEEP(20);
}
