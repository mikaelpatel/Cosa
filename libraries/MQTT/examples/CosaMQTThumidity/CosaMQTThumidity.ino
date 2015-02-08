/**
 * @file CosaMQTThumidity.ino
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
 * MQTT client demonstration; Publish DHT11 digital humidity and
 * temperature sensor reading on MQTT server.
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

#include "MQTT.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/Driver/DHT.hh"
#include "Cosa/Socket/Driver/W5100.hh"

// DHT11 sensor
DHT11 sensor(Board::EXT0);

// Network configuration
#define MOSQUITTO "test.mosquitto.org"
#define RABBITMQ "dev.rabbitmq.com"
#define ECLIPSE "iot.eclipse.org"
#define QM2M "q.m2m.io"
#define SERVER QM2M

static const char CLIENT[] __PROGMEM = "CosaMQTThumidity";
MQTT::Client client;

// W5100 Ethernet Controller with MAC-address
const uint8_t mac[6] __PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);

void setup()
{
  // Start watchdog and real-time clock (timer)
  Watchdog::begin();
  RTC::begin();

  // Start ethernet controller and request network address for hostname
  ethernet.begin_P(CLIENT);

  // Start MQTT client with socket and connect to server
  client.begin(ethernet.socket(Socket::TCP));
  client.connect(SERVER, CLIENT);

  // Publish data with the different quality of service levels
  client.publish_P(PSTR("public/cosa/client"), CLIENT, sizeof(CLIENT));
}

void loop()
{
  // Request a conversion and read the humidity and temperature
  sensor.sample();
  int16_t temperature = sensor.get_temperature();
  int16_t humidity = sensor.get_humidity();

  // Use an iobuffer and iostream to convert humidity/temperature to string
  IOBuffer<16> buf;
  IOStream cout(&buf);
  cout << humidity / 10 << PSTR(" %") << ends;
  client.publish(PSTR("public/cosa/humidity"), buf, buf.available());

  // Publish humidity and temperature
  buf.empty();
  cout << temperature / 10 << PSTR(" C") << ends;
  client.publish(PSTR("public/cosa/temperature"), buf, buf.available());
  sleep(5);
}
