/**
 * @file CosaMQTTtemperature.ino
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
 * MQTT client; Publish 1-Wire digital thermometer, DS18B20, 
 * temperature reading on MQTT server.
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
 *                       DS18B20/sensor
 *                       +------------+
 * (GND)---------------1-|GND         |
 * (D14)-----+---------2-|DQ          |
 *           |       +-3-|VDD         |
 *          4K7      |   +------------+
 *           |       | 
 * (VCC)-----+-------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Board.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/IoT/MQTT.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/Socket/Driver/W5100.hh"

// One-wire pin and connected DS18B20 device
OWI owi(Board::D14);
DS18B20 sensor(&owi);

// Network configuration
#define MOSQUITTO "test.mosquitto.org"
#define RABBITMQ "dev.rabbitmq.com"
#define ECLIPSE "iot.eclipse.org"
#define QM2M "q.m2m.io"
#define SERVER QM2M

static const char CLIENT[] __PROGMEM = "CosaMQTTtemperature";
MQTT::Client client;

// W5100 Ethernet Controller with MAC-address
const uint8_t mac[6] __PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);

void setup()
{
  Watchdog::begin();

  // Connect the sensor and set resolution to 10-bits
  sensor.connect(0);
  sensor.set_resolution(10);
  sensor.write_scratchpad();

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
  // Request a conversion and read the temperature
  sensor.convert_request();
  sensor.read_scratchpad();

  // Use an iobuffer and iostream to convert temperature to a string
  IOBuffer<16> buf;
  IOStream cout(&buf);
  cout << sensor << PSTR(" C") << ends;

  // Publish the value to the MQTT server and take a nap
  client.publish(PSTR("public/cosa/temperature"), buf, buf.available());
  sleep(5);
}
