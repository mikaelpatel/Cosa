/**
 * @file CosaVWItermostat.ino
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
 * Demonstration of the Virtual Wire Interface (VWI) driver and
 * simple logic to turn relays on and off depending on received
 * message with temperature and humidity reading from CosaTinyDHT11.
 * Turn on heater (relay#1) @ 22 C and off @ 26 C, and fan (relay#2)
 * @ 70 %RH and off @ 50 %RH. The fan logic will also turn on and
 * off a remote vent using the NEXA driver.
 * 
 * @section Circuit
 * 1. Connect RF433/315 Receiver to Arduino/ATtiny D8/D1 => RX DATA.
 * 2. Relay modules to Arduino D2 => relay#1 (heater), 
 *    and D3 => relay#2 (fan).
 * 3. RF433/315 Transmitter to D4.
 * 4. UART module RX to D0 (ATtiny only).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/Driver/NEXA.hh"
#include "Cosa/Trace.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

class Relay {
private:
  OutputPin m_pin;
public:
  Relay(Board::DigitalPin pin) : m_pin(pin, 1) {}
  bool is_on() { return (m_pin.is_clear()); }
  bool is_off() { return (m_pin.is_set()); }
  void on() { m_pin.clear(); }
  void off() { m_pin.set(); }
};

class RemoteRelay {
private:
  NEXA::Transmitter m_tx;
  uint8_t m_device;
  void send(uint8_t value)
  {
    VWI::disable();
    m_tx.send(m_device, value);
    VWI::enable();
  }
public:
  RemoteRelay(Board::DigitalPin pin, uint32_t house, uint8_t device) :
    m_tx(pin, house),
    m_device(device)
  {}
  void open() { send(1); }
  void close() { send(0); }
};

VirtualWireCodec codec;
const uint16_t SPEED = 4000;
#if defined(__ARDUINO_TINY__)
VWI::Receiver rx(Board::D1, &codec);
#else
VWI::Receiver rx(Board::D8, &codec);
#endif
Relay heater(Board::D2);
Relay fan(Board::D3);
RemoteRelay vent(Board::D4, 0xc05a01L, 0);

void setup()
{
  // Start trace on UART. Print available free memory.
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVWItermostat: started"));
  TRACE(free_memory());

  // Start watchdog for low power delay mode
  Watchdog::begin();
  RTC::begin();

  // Start virtual wire interface and receiver
  VWI::begin(SPEED);
  rx.begin();

  // Close the remote vent
  vent.close();
}

// Message type to receive
struct msg_t {
  uint16_t nr;
  int16_t humidity;
  int16_t temperature;
};

void loop()
{
  // Wait for a message
  msg_t msg;
  int8_t len = rx.recv(&msg, sizeof(msg));
  if (len != sizeof(msg)) return;

  // Read temperature and humidity. Handle read errors
  int16_t humidity = msg.humidity;
  int16_t temperature = msg.temperature;

  // Check if heater should be turned on @ 22 C and off @ 26 C
  static const uint8_t TEMP_MIN = 22;
  static const uint8_t TEMP_MAX = 26;
  if (heater.is_on()) {
    if (temperature > TEMP_MAX) heater.off();
  }
  else if (temperature < TEMP_MIN) heater.on();

  // Check if fan/vent should be turned on @ 70 %RH and off @ 50 %RH.
  static const uint8_t RH_MIN = 50;
  static const uint8_t RH_MAX = 70;
  if (fan.is_on()) {
    if (humidity < RH_MIN) {
      fan.off();
      vent.close();
    }
  }
  else if (humidity > RH_MAX) {
    vent.open();
    fan.on();
  }

  // Print the new state
  trace << msg.nr << ':' << PSTR("RH = ") << humidity << PSTR("%, ");
  trace << PSTR("T = ") << temperature << PSTR(" C, ");
  trace << PSTR("Heater = ") << (heater.is_on() ? PSTR("ON, ") : PSTR("OFF, "));
  trace << PSTR("Fan = ") << (fan.is_on() ? PSTR("ON\n") : PSTR("OFF\n"));

  // Sample every 2 seconds
  SLEEP(2);
}
