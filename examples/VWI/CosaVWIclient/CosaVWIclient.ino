/**
 * @file CosaVWIclient.ino
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
 * Demonstration of the Virtual Wire Interface (VWI) driver.
 * Transmits a simple message with identity, message number,
 * and two data element; analog samples. Server should send back
 * an acknowledgement message with the identity and message number.
 * If an acknowledgement is not received a retransmission will occur.
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to Arduino D9, RF433/315
 * Receiver to Arduino D8. Connect VCC and GND. Connect Arduino
 * analog pins A2 and A3 to analog sensors.  
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/VWI/Codec/ManchesterCodec.hh"
#include "Cosa/VWI/Codec/BitstuffingCodec.hh"
#include "Cosa/VWI/Codec/Block4B5BCodec.hh"
#include "Cosa/Trace.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#if !defined(__ARDUINO_TINYX5__)
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"
#endif

// Select the codec to use for the Virtual Wire Interface. Should be the
// same as in CosaVWIserver.ino
// VirtualWireCodec codec;
// ManchesterCodec codec;
// Block4B5BCodec codec;
BitstuffingCodec codec;

// Virtual Wire Interface Transmitter and Receiver
#if defined(__ARDUINO_TINYX5__)
VWI::Receiver rx(Board::D0, &codec);
VWI::Transmitter tx(Board::D1, &codec);
#else
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"
VWI::Receiver rx(Board::D8, &codec);
VWI::Transmitter tx(Board::D9, &codec);
#endif

// Network configuration
const uint32_t ADDR = 0xC05A0001;
const uint8_t MASK = 0;
const uint16_t SPEED = 4000;
const uint16_t TIMEOUT = 2000000 / SPEED;

// Analog pins to sample for values to send
AnalogPin luminance(Board::A2);
AnalogPin temperature(Board::A3);

// Message type to send (should be in an include file for client and server)
const uint8_t SAMPLE_CMD = 17;
struct sample_t {
  uint16_t luminance;
  uint16_t temperature;
};

void setup()
{
#if !defined(__ARDUINO_TINYX5__)
  // Start trace on UART. Print available free memory and configuration
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVWIclient: started"));
  TRACE(free_memory());
  TRACE(ADDR);
  TRACE(SPEED);
  TRACE(TIMEOUT);
#endif

  // Start watchdog for delay
  Watchdog::begin();
  RTC::begin();

  // Start virtual wire interface in extended mode
  VWI::begin(ADDR, SPEED);
  rx.begin(MASK);
  tx.begin();
}

void loop()
{
  // Statistics; Number of messages and error count (retransmissions)
  static uint16_t cnt = 0;
  static uint16_t err = 0;

  // Message types (samples and acknowledgement)
  VWI::header_t ack;
  sample_t sample;
  
  // Initiate the message with id, sequence number and measurements
  sample.luminance = luminance.sample();
  sample.temperature = temperature.sample();

  // Send message and receive acknowledgement; retransmit until acknowledged
  uint8_t sendnr = 0;
  uint8_t next = tx.get_msg_nr();
  int8_t len; 
  do {
    sendnr += 1;
    tx.set_msg_nr(next);
    tx.send(&sample, sizeof(sample), SAMPLE_CMD);
    tx.await();
    len = rx.recv(&ack, sizeof(ack), TIMEOUT);
  } while (len != sizeof(ack) || (ack.nr != next) || (ack.addr != ADDR));
  cnt += 1;
  
  // Check if a retransmission did occur and print statistics
  if (sendnr > 1) {
    err += 1;
#if !defined(__ARDUINO_TINYX5__)
    trace << PSTR("cnt = ") << cnt;
    trace << PSTR(", err = ") << err;
    trace << PSTR(", nr = ") << sendnr;
    trace << PSTR(" (") << (err * 100) / cnt << PSTR("%)");
    trace << endl;
#endif
  }

  // Take a nap
  SLEEP(2);
}
