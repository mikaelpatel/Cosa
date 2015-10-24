/**
 * @file CosaWirelessReceiverLCD.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * Cosa Wireless interface demo; receiver messages from CosaWirelessSender
 * and others. Display message on LCD.
 *
 * @section Circuit
 * See Wireless drivers for circuit connections.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"

#include <OWI.h>
#include <DS18B20.h>

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
VWI::Receiver rx(Board::D1, &codec);
#else
VWI::Receiver rx(Board::D7, &codec);
#endif
VWI rf(NETWORK, DEVICE, SPEED, &rx);

// Select port type to use with the LCD device driver.
// LCD and communication port
#include <HD44780.h>

// HD44780 driver built-in adapters
// HD44780::Port4b port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4W port;

// I2C expander io port based adapters
#include <PCF8574.h>
#include <MJKDZ_LCD_Module.h>
MJKDZ_LCD_Module port;
// MJKDZ_LCD_Module port(0);
// #include <GY_IICLCD.h>
// GY_IICLCD port;
// #include <DFRobot_IIC_LCD_Module.h>
// DFRobot_IIC_LCD_Module port;
// #include <SainSmart_LCD2004.h>
// SainSmart_LCD2004 port;

// HD44780 based LCD with support for serial communication
// #include <ERM1602_5.h>
// ERM1602_5 port;

// HD44780 variants; 16X1, 16X2, 16X4, 20X4, default 16X2
// HD44780 lcd(&port, 20, 4);
// HD44780 lcd(&port, 16, 4);
HD44780 lcd(&port);
IOStream cout(&lcd);

void setup()
{
  Watchdog::begin();
  RTT::begin();
  lcd.begin();
  cout << clear << PSTR("CosaWirelessReceiver: started");
  rf.begin();
  sleep(2);
}

static const uint8_t IOSTREAM_TYPE = 0x00;

static const uint8_t PAYLOAD_MAX = 16;
struct payload_msg_t {
  uint8_t nr;
  uint8_t payload[PAYLOAD_MAX];
};
static const uint8_t PAYLOAD_TYPE = 0x01;

IOStream& operator<<(IOStream& outs, payload_msg_t* msg)
{
  outs.print(0L, msg->payload, 2, IOStream::hex);
  outs << msg->nr;
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
  DS18B20::print(outs, msg->indoors);
  outs << PSTR(" C,");
  DS18B20::print(outs, msg->outdoors);
  outs << PSTR(" C,");
  outs << msg->battery << PSTR(" mV,");
  outs << msg->nr;
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
  outs << msg->humidity / 10 << '.' << msg->humidity % 10 << PSTR(" %,")
       << msg->temperature / 10 << '.' << msg->temperature % 10 << PSTR(" C,")
       << msg->battery << PSTR(" mV,")
       << msg->nr;
  return (outs);
}

struct dlt_msg_t {
  uint8_t nr;
  uint16_t luminance;
  uint16_t temperature;
  uint16_t battery;
};
static const uint8_t DIGITAL_LUMINANCE_TEMPERATURE_TYPE = 0x04;

IOStream& operator<<(IOStream& outs, dlt_msg_t* msg)
{
  outs << msg->luminance << PSTR(" lux,")
       << msg->temperature << PSTR(" C,")
       << msg->battery << PSTR(" mV,")
       << msg->nr;
  return (outs);
}

void loop()
{
  // Receive a message
  static uint32_t recs = 0;
  static uint32_t errs = 0;
  const uint32_t TIMEOUT = 10000;
  const uint8_t MSG_MAX = 32;
  uint8_t msg[MSG_MAX];
  uint8_t src;
  uint8_t port;
  int res = rf.recv(src, port, msg, sizeof(msg), TIMEOUT);
  recs += 1;

  // Clear screen and display statistics
  cout << clear;

  // Check for message
  if (res >= 0) {
    // Print the message payload according to port/message type
    switch (port) {
    case IOSTREAM_TYPE:
      for (uint8_t i = 0; i < res; i++)
	if (msg[i] != '\f' && msg[i] != '\n')
	  cout << (char) msg[i];
      return;
    case PAYLOAD_TYPE:
      if (res != sizeof(payload_msg_t)) break;
      cout << (payload_msg_t*) msg;
      return;
    case DIGITAL_TEMPERATURE_TYPE:
      if (res != sizeof(dt_msg_t)) break;
      {
	static uint8_t dt_nr = 0;
	dt_msg_t* dt_msg = (dt_msg_t*) msg;
	cout << dt_msg;
	if (dt_nr != dt_msg->nr) {
	  cout << PSTR("?");
	  errs++;
	}
	dt_nr = dt_msg->nr + 1;
      }
      return;
    case DIGITAL_HUMIDITY_TEMPERATURE_TYPE:
      if (res != sizeof(dht_msg_t)) break;
      {
	static uint8_t dht_nr = 0;
	dht_msg_t* dht_msg = (dht_msg_t*) msg;
	cout << dht_msg;
	if (dht_nr != dht_msg->nr) {
	  cout << PSTR("?");
	  errs++;
	}
	dht_nr = dht_msg->nr + 1;
      }
      return;
    case DIGITAL_LUMINANCE_TEMPERATURE_TYPE:
      if (res != sizeof(dlt_msg_t)) break;
      {
	static uint8_t dlt_nr = 0;
	dlt_msg_t* dlt_msg = (dlt_msg_t*) msg;
	cout << dlt_msg;
	if (dlt_nr != dlt_msg->nr) {
	  cout << PSTR("?");
	  errs++;
	}
	dlt_nr = dlt_msg->nr + 1;
      }
      return;
    default:
      cout.print(0L, msg, res, IOStream::hex);
      return;
    }
    res = EMSGSIZE;
  }

  // Check error codes
  errs += 1;
  cout << PSTR("error(") << res << PSTR(")");
  if (res == EMSGSIZE) {
    cout << PSTR(":illegal frame size");
  }
  else if (res == ETIME) {
    cout << PSTR(":timeout");
  }
}
