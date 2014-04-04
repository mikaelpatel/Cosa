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
 * identity. Sucessive commands are dispatched through the Listener.
 * The built-in LED is set on/off according to the command when the
 * address of the command matches.. See Also CosaNEXAsender if you 
 * wish to run the sketch without a NEXA remote control.
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
  LED(Board::DigitalPin pin) : NEXA::Receiver::Device(0L), m_pin(pin) {}
  virtual void on_event(uint8_t type, uint16_t value) { m_pin << value; }
};

#if defined(__ARDUINO_MEGA__)
NEXA::Receiver receiver(Board::EXT5);
#else
NEXA::Receiver receiver(Board::EXT0);
#endif
LED device(Board::LED);

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
