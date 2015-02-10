/**
 * @file CosaNEXAreceiver.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Simple sketch to demonstrate receiving Nexa Home Wireless Switch
 * Remote codes. First command received will be used as the device
 * identity. Sucessive commands are dispatched through the Listener.
 * The built-in LED is set on/off according to the command when the
 * address of the command matches.. See Also CosaNEXAsender if you
 * wish to run the sketch without a NEXA remote control.
 *
 * @section Circuit
 * @code
 *                         RF433/RX                       V
 *                       +------------+                   |
 *                       |0-----------|-------------------+
 *                       |ANT         |       17.3 cm
 *                       |            |
 *                       |            |
 *                       |            |
 *                       |            |
 * (VCC)---------------1-|VCC         |
 *                     2-|DATA        |
 * (EXT0/D2)-----------3-|DATA        |
 * (GND)---------------4-|GND         |
 *                       +------------+
 * @endcode
 *
 * On an ATtinyX5 please connect a LED to pin D4 in serie with a
 * resistor (approx 300-500 ohm) to ground.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OutputPin.hh"
#include "Cosa/Driver/NEXA.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"

class LED : public NEXA::Receiver::Device {
private:
  OutputPin m_pin;
public:
  LED(Board::DigitalPin pin) :
    NEXA::Receiver::Device(0L),
    m_pin(pin)
  {
  }
  virtual void on_event(uint8_t type, uint16_t value)
  {
    UNUSED(type);
    m_pin << value;
  }
};
LED device(Board::LED);

#if defined(BOARD_ATMEGA2560)
NEXA::Receiver receiver(Board::EXT5);
#else
NEXA::Receiver receiver(Board::EXT0);
#endif

void setup()
{
  // Initiate Real-Time Clock and Watchdog
  RTC::begin();
  Watchdog::begin();

  // Use polling version to receive the remote button to attach
  NEXA::code_t cmd;
  receiver.recv(cmd);
  device.set_key(cmd);
  receiver.attach(&device);

  // Enable the interrupt driven version of the receiver
  receiver.enable();
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
