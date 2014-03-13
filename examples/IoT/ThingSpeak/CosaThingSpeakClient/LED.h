/**
 * @file LED.h
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef LED_H
#define LED_H

#include "CommandHandler.h"
#include "Cosa/Pins.hh"
#include "Cosa/IoT/ThingSpeak.hh"

class LED : public Command {
public:
  LED(ThingSpeak::TalkBack* talkback, const char* string, 
      Board::DigitalPin pin, uint8_t state) : 
    Command(talkback, string),
    m_led(pin),
    m_state(state)
  {}
  virtual void execute();
private:
  OutputPin m_led;
  uint8_t m_state;
};

void 
LED::execute()
{ 
  Command::execute();
  m_led.set(m_state);
}

#endif

