/**
 * @file Ping.h
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

#ifndef PING_H
#define PING_H

#include "CommandHandler.h"
#include "Cosa/IoT/ThingSpeak.hh"

class Ping : public Command {
public:
  Ping(ThingSpeak::TalkBack* talkback, Command* pong) : 
    Command(talkback, PSTR("PING")),
    m_pong(pong)
  {}
  virtual void execute();
private:
  Command* m_pong;
};

#endif
