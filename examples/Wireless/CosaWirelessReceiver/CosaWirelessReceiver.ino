/**
 * @file CosaWirelessReceiver.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
 * Cosa Wireless interface demo; receiver messages from CosaWirelessSender.
 *
 * @section Circuit
 * See Wireless drivers for circuit connections.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

// Configuration; network and device addresses
#define NETWORK 0xC05A
#define DEVICE 0x01

// Select Wireless device driver
// #define USE_CC1101
#define USE_NRF24L01P
// #define USE_RFM69
// #define USE_VWI

#if defined(USE_CC1101)
#include "Cosa/Wireless/Driver/CC1101.hh"
CC1101 rf(NETWORK, DEVICE);

#elif defined(USE_NRF24L01P)
#include "Cosa/Wireless/Driver/NRF24L01P.hh"
NRF24L01P rf(NETWORK, DEVICE);

#elif defined(USE_RFM69)
#include "Cosa/Wireless/Driver/RFM69.hh"
RFM69 rf(NETWORK, DEVICE);

#elif defined(USE_VWI)
#include "Cosa/Wireless/Driver/VWI.hh"
#include "Cosa/Wireless/Driver/VWI/Codec/VirtualWireCodec.hh"
VirtualWireCodec codec;
#define SPEED 4000
#if defined(BAORD_TINY)
VWI rf(NETWORK, DEVICE, SPEED, Board::D1, Board::D0, &codec);
#else
VWI rf(NETWORK, DEVICE, SPEED, Board::D7, Board::D8, &codec);
#endif
#endif

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWirelessReceiver: started"));
  Watchdog::begin();
  RTC::begin();
  rf.begin();
}

static const uint8_t IOSTREAM_TYPE = 0x00;

static const uint8_t PAYLOAD_MAX = 14;
struct payload_msg_t {
  uint8_t nr;
  uint8_t payload[PAYLOAD_MAX];
};
static const uint8_t PAYLOAD_TYPE = 0x01;

IOStream& operator<<(IOStream& outs, payload_msg_t* msg)
{
  outs << PSTR("nr=") << msg->nr << PSTR(",payload=");
  outs.print(msg->payload, PAYLOAD_MAX, IOStream::hex);
  return (outs);
}

struct dt_msg_t {
  uint8_t nr;
  int16_t indoors;
  int16_t outdoors;
  uint16_t battery;
};
static const uint8_t DIGITAL_TEMPERATURE_TYPE = 0x02;

IOStream& operator<<(IOStream& outs, dt_msg_t* msg)
{
  outs << PSTR("nr=") << msg->nr
       << PSTR(",indoors=");
  DS18B20::print(outs, msg->indoors);
  outs << PSTR(",outdoors=");
  DS18B20::print(outs, msg->outdoors);
  outs << PSTR(",battery=") << msg->battery
       << endl;
  return (outs);
}

struct dht_msg_t {
  uint8_t nr;
  int16_t humidity;
  int16_t temperature;
  uint16_t battery;
};
static const uint8_t DIGITAL_HUMIDITY_TEMPERATURE_TYPE = 0x03;

IOStream& operator<<(IOStream& outs, dht_msg_t* msg)
{
  trace << PSTR("nr=") << msg->nr
	<< PSTR(",humidity=") << msg->humidity
	<< PSTR(",temperature=") << msg->temperature
	<< PSTR(",battery=") << msg->battery
	<< endl;
  return (outs);
}

struct dlt_msg_t {
  uint32_t timestamp;
  uint16_t luminance;
  uint16_t temperature;
  uint16_t battery;
};
static const uint8_t DIGITAL_LUMINANCE_TEMPERATURE_TYPE = 0x04;

IOStream& operator<<(IOStream& outs, dlt_msg_t* msg)
{
  trace << PSTR("timestamp=") << msg->timestamp
	<< PSTR(",luminance=") << msg->luminance
	<< PSTR(",temperature=") << msg->temperature
	<< PSTR(",battery=") << msg->battery
	<< endl;
  return (outs);
}

void loop()
{
  // Receive a message
  const uint32_t TIMEOUT = 5000;
  const uint8_t MSG_MAX = 32;
  uint8_t msg[MSG_MAX];
  uint8_t src;
  uint8_t port;
  int count = rf.recv(src, port, msg, sizeof(msg), TIMEOUT);

  // Print the message header
  if (count >= 0) {
    trace << PSTR("src=") << hex << src 
	  << PSTR(",port=") << hex << port
	  << PSTR(",dest=") 
	  << hex << (rf.is_broadcast() ? 0 : rf.get_device_address())
	  << PSTR(",len=") << count
#if defined(COSA_WIRELESS_DRIVER_CC1101_HH)
	  << PSTR(",rssi=") << rf.get_input_power_level() 
	  << PSTR(",lqi=") << rf.get_link_quality_indicator()
#endif
	  << PSTR(":");

    // Print the message payload according to port/message type
    switch (port) {
    case IOSTREAM_TYPE:
      for (uint8_t i = 0; i < count; i++)
	if (msg[i] != '\f' && msg[i] != '\n') 
	  trace << (char) msg[i];
      trace << endl;
      break;
    case PAYLOAD_TYPE: 
      trace << (payload_msg_t*) msg; 
      break;
    case DIGITAL_TEMPERATURE_TYPE: 
      trace << (dt_msg_t*) msg; 
      break;
    case DIGITAL_HUMIDITY_TEMPERATURE_TYPE: 
      trace << (dht_msg_t*) msg; 
      break;
    case DIGITAL_LUMINANCE_TEMPERATURE_TYPE: 
      trace << (dlt_msg_t*) msg; 
      break;
    default:
      trace << PSTR("msg=");
      trace.print(msg, count, IOStream::hex);
    }
  }

  // Check error codes
  else if (count == -1) {
    trace << PSTR("error:illegal frame size(-1)\n");
  }
  else if (count == -2) {
    trace << PSTR("error:timeout(-2)\n");
  }
  else if (count < 0) {
    trace << PSTR("error(") << count << PSTR(")\n");
  }
}
