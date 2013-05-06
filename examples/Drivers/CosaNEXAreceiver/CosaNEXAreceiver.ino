/**
 * @file CosaNEXAreceiver.ino
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
 * Simple sketch to demonstrate receiving Nexa Home Wireless Switch
 * Remote codes. First command received will be used as the device 
 * identity. Sucessive commands are compared against the device 
 * identity and if matches the built-in LED is set on/off according 
 * to the command. See Also CosaNEXAsender if you wish to run the 
 * sketch without a NEXA remote control.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Driver/NEXA.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"

class LED : public NEXA::Receiver::Listener {
private:
  OutputPin m_pin;
public:
  LED(Board::DigitalPin pin) : NEXA::Receiver::Listener(), m_pin(pin) {}
  virtual void on_change(uint8_t onoff) { m_pin << onoff; }
};

NEXA::Receiver receiver(Board::EXT0);
LED device(Board::LED);

void setup()
{
  // Initiate Real-Time Clock and Watchdog
  RTC::begin();
  Watchdog::begin();

  // Use polling version to receive the remote button to attach
  NEXA::code_t cmd;
  receiver.recv(cmd);
  device.set_unit(cmd);
  receiver.attach(device);
  
  // Enable the interrupt driven version of the receiver
  receiver.enable();
}

void loop()
{
  // Wait for the next event and dispatch to listeners
  Event event;
  Event::queue.await(&event);
  if (event.get_target() == &receiver) receiver.dispatch();
}
