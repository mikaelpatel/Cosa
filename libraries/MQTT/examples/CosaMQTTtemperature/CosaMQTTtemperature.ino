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
 *                           DS18B20/3
 *                       +------------+++
 * (GND)---------------1-|GND         |||\
 * (D7)------+---------2-|DQ          ||| |
 *           |       +-3-|VDD         |||/
 *          4K7      |   +------------+++
 *           |       | 
 * (VCC)-----+       +---(VCC/GND)
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "MQTT.hh"
#include "Cosa/Board.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/Socket/Driver/W5100.hh"

// Disable SD on Ethernet Shield
#define USE_ETHERNET_SHIELD
#if defined(USE_ETHERNET_SHIELD)
#include "Cosa/OutputPin.hh"
OutputPin sd(Board::D4, 1);
#endif

// One-wire pin and connected DS18B20 device
OWI owi(Board::D7);

// The devices connected to the one-wire bus
DS18B20 outdoors(&owi);
DS18B20 indoors(&owi);
DS18B20 basement(&owi);

DS18B20* thermometer[] = {
  &outdoors,
  &indoors,
  &basement
};

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

  // Connect the sensors and set resolution to 10-bits
#if (ARDUINO < 150)
  for (uint8_t i = 0; i < membersof(thermometer); i++) {
    thermometer[i]->connect(i);
    thermometer[i]->set_resolution(10);
    thermometer[i]->write_scratchpad();
  }
#else
  uint8_t i = 0;
  for (auto sensor : thermometer) {
    sensor->connect(i++);
    sensor->set_resolution(10);
    sensor->write_scratchpad();
  }
#endif

  // Start ethernet controller and request network address for hostname
  ethernet.begin_P(CLIENT);

  // Start MQTT client with socket and connect to server
  client.begin(ethernet.socket(Socket::TCP));
  client.connect(SERVER, CLIENT);

  // Publish the name of the client
  client.publish_P(PSTR("public/cosa/client"), CLIENT, sizeof(CLIENT));
}

void loop()
{
  static uint8_t i = 0;

  // Select the topic path for the current thermometer
  str_P topic;
  switch (i) {
  case 0: topic = PSTR("public/cosa/temperature/outdoors"); break;
  case 1: topic = PSTR("public/cosa/temperature/indoors"); break;
  case 2: topic = PSTR("public/cosa/temperature/basement"); break;
  };
    
  // Request a conversion and read the temperature
  thermometer[i]->convert_request();
  thermometer[i]->read_scratchpad();

  // Use an iobuffer and iostream to convert temperature value to a string
  IOBuffer<32> buf;
  IOStream cout(&buf);
  cout << *thermometer[i] << PSTR(" C") << ends;

  // Publish the value to the MQTT server and take a nap
  client.publish(topic, buf, buf.available());
  i += 1;
  if (i == membersof(thermometer)) i = 0;

  // Take a nap
  sleep(5);
}
