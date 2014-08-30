/**
 * @file Commandhandler.h
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

#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "Cosa/Alarm.hh"
#include "Cosa/IoT/ThingSpeak.hh"

class Command : public ThingSpeak::TalkBack::Command {
public:
  Command(ThingSpeak::TalkBack* talkback, const char* string) : 
    ThingSpeak::TalkBack::Command(talkback, string)
  {}
  Command(ThingSpeak::TalkBack* talkback, str_P string) : 
    ThingSpeak::TalkBack::Command(talkback, string)
  {}
  virtual void execute();
};

class CommandHandler : public Alarm {
public:
  CommandHandler(ThingSpeak::TalkBack* talkback, uint16_t period) : 
    Alarm(period), m_talkback(talkback)
  {}
  virtual void run();
private:
  ThingSpeak::TalkBack* m_talkback;
};

#endif
