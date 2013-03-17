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
 * Connect RF433/315 Transmitter Data to Arduino D12, RF433/315
 * Receiver to Arduino D11. Connect VCC and GND. Connect Arduino
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
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// Analog pins to sample for values to send
AnalogPin luminance(Board::A2);
AnalogPin temperature(Board::A3);

// Select the codec to use for the Virtual Wire Interface. Should be the
// same as in CosaVWIserver.ino
VirtualWireCodec codec;
// ManchesterCodec codec;
// BitstuffingCodec codec;
// Block4B5BCodec codec;

// Virtual Wire Interface Transmitter and Receiver
VWI::Transmitter tx(Board::D12, &codec);
VWI::Receiver rx(Board::D11, &codec);
const uint16_t SPEED = 4000;

void setup()
{
  // Start trace on UART. Print available free memory.
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVWIclient: started"));
  TRACE(free_memory());

  // Start watchdog for delay
  Watchdog::begin();
  RTC::begin();

  // Start virtual wire interface, transmitter and receiver
  VWI::begin(SPEED);
  tx.begin();
  rx.begin();
}

// Message type to send
struct msg_t {
  uint32_t id;
  uint8_t nr;
  uint16_t data[12];
};

// Acknowledge message type
struct ack_t {
  uint32_t id;
  uint8_t nr;
};

void loop()
{
  // Statistics; Number of messages and error count (retransmissions)
  static uint16_t cnt = 0;
  static uint16_t err = 0;

  // Message types (data and acknowledgement)
  msg_t msg;
  ack_t ack;

  // Initiate the message with id, sequence number and measurements
  msg.id = 0xC05A0001;
  msg.nr = cnt++;
  msg.data[0] = luminance.sample();
  msg.data[1] = temperature.sample();
  for (uint8_t i = 2; i < membersof(msg.data); i++)
    msg.data[i] = 0xa5a5;

  // Send message and receive acknowledgement
  uint8_t nr = 0;
  int8_t len; 
  do {
    nr += 1;
    tx.send(&msg, sizeof(msg));
    tx.await();
    len = rx.recv(&ack, sizeof(ack), 64);
    if (len != sizeof(ack))
      DELAY(300);
  } while (len != sizeof(ack) || (ack.nr != msg.nr) || (ack.id != msg.id));

  // Check if a retransmission did occur and print statistics
  if (nr > 1) {
    err += 1;
    INFO("cnt = %ud, err = %ud, nr = %ud (%ud%%)", 
	 cnt, err, nr, (err * 100) / cnt);
  }

  // Short delay before sending the next message
  Watchdog::delay(128);
}
