/**
 * @file CosaMQTT.ino
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
 * W5100 Ethernet Controller device driver example code; MQTT client.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/INET/MQTT.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Socket/Driver/W5100.hh"

// Network configuration
#define MOSQUITTO "test.mosquitto.org"
#define QM2M "q.m2m.io"
#define SERVER QM2M

class MQTTClient : public MQTT::Client {
public:
  virtual void on_publish(char* topic, void* buf, size_t count);
};

void 
MQTTClient::on_publish(char* topic, void* buf, size_t count)
{
  trace << PSTR("on_publish::count = ") << count << endl;
  trace << PSTR("  topic = ") << topic << endl;
  trace << PSTR("  payload = ") << (char*) buf << endl;
}

// W5100 Ethernet Controller with MAC-address
const uint8_t mac[6] PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);
MQTTClient client;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaMQTT: started"));
  Watchdog::begin();

  // Start ethernet controller and request network address for hostname
  ASSERT(ethernet.begin_P(PSTR("CosaMQTTclient")));

  // Start MQTT client with socket and connect to server
  ASSERT(client.begin(ethernet.socket(Socket::TCP)));
  ASSERT(!client.connect(SERVER, PSTR("CosaMQTT")));

  // Publish data with the different quality of service levels
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
  TRACE(client.subscribe(PSTR("public/cosa/a/d")));
  for (uint8_t i = 0; i < 6; i++) client.service(10000L);
  TRACE(client.unsubscribe(PSTR("public/cosa/a/d")));
  client.disconnect();
  ASSERT(true == false);
}
