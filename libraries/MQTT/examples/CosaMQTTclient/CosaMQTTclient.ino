/**
 * @file CosaMQTT.ino
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
 * W5100 Ethernet Controller device driver example code; MQTT client.
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
 * This file is part of the Arduino Che Cosa project.
 */

#include <MQTT.h>
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Socket/Driver/W5100.hh"

// Network configuration
#define MOSQUITTO "test.mosquitto.org"
#define RABBITMQ "dev.rabbitmq.com"
#define ECLIPSE "iot.eclipse.org"
#define QM2M "q.m2m.io"
#define SERVER QM2M

// Semi-simple MQTT client; trace incoming topic/value and echo back
class MQTTClient : public MQTT::Client {
public:
  virtual void on_publish(char* topic, void* buf, size_t count);
};

void
MQTTClient::on_publish(char* topic, void* buf, size_t count)
{
  // Print the topic and value to trace stream
  trace << PSTR("on_publish::count = ") << count << endl;
  trace << PSTR("  topic = ") << topic << endl;
  trace << PSTR("  payload = ") << (char*) buf << endl;

  // Echo back the value to another topic
  publish(PSTR("public/cosa/a/e"), buf, count);
}

// MQTT client name
const char CLIENT[] __PROGMEM = "CosaMQTTclient";
MQTTClient client;

// W5100 Ethernet Controller with MAC-address
const uint8_t mac[6] __PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);


void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaMQTT: started"));
  Watchdog::begin();

  // Start ethernet controller and request network address for hostname
  ASSERT(ethernet.begin_P(CLIENT));

  // Start MQTT client with socket and connect to server
  ASSERT(client.begin(ethernet.socket(Socket::TCP)));
  ASSERT(!client.connect(SERVER, CLIENT));

  // Publish data with the different quality of service levels
  TRACE(client.publish_P(PSTR("public/cosa/client"), CLIENT, sizeof(CLIENT)));

  uint8_t buf[16];
  memset(buf, 'a', sizeof(buf));
  TRACE(client.publish(PSTR("public/cosa/a/a"), buf, sizeof(buf)));

  memset(buf, 'b', sizeof(buf));
  TRACE(client.publish(PSTR("public/cosa/a/b"), buf, sizeof(buf),
		       MQTT::ACKNOWLEDGED_DELIVERY, false));

  memset(buf, 'c', sizeof(buf));
  TRACE(client.publish(PSTR("public/cosa/a/c"), buf, sizeof(buf),
		       MQTT::ASSURED_DELIVERY, false));
}

void loop()
{
  // Subscribe to a topic
  TRACE(client.subscribe(PSTR("public/cosa/a/d")));

  // Service incoming publish messages. Wait max 10 seconds per message
  for (uint8_t i = 0; i < 6; i++) TRACE(client.service(10000L));

  // Unsubscribe and disconnect
  TRACE(client.unsubscribe(PSTR("public/cosa/a/d")));
  TRACE(client.disconnect());

  // And terminate
  ASSERT(true == false);
}
