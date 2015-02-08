/**
 * @file LEDCommand.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#ifndef LED_COMMAND_H
#define LED_COMMAND_H

#include "CommandHandler.h"
#include "Cosa/OutputPin.hh"

template<uint8_t STATE>
class LEDCommand : public Command {
public:
  LEDCommand(ThingSpeak::TalkBack* talkback, const char* string, Board::DigitalPin pin) :
    Command(talkback, string),
    m_led(pin)
  {}
  virtual void execute();
private:
  OutputPin m_led;
};

template<uint8_t STATE>
void
LEDCommand<STATE>::execute()
{
  Command::execute();
  m_led.set(STATE);
}

#endif

