/**
 * @file SensorCommand.h
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

#ifndef SENSOR_COMMAND_H
#define SENSOR_COMMAND_H

#include "CommandHandler.h"
#include "SensorHandler.h"

template<bool IS_ENABLE>
class SensorCommand : public Command {
public:
  SensorCommand(ThingSpeak::TalkBack* talkback, const char* string,
		SensorHandler* handler) :
    Command(talkback, string),
    m_handler(handler)
  {}
  virtual void execute();
private:
  SensorHandler* m_handler;
};

template<bool IS_ENABLE>
void
SensorCommand<IS_ENABLE>::execute()
{
  Command::execute();
  if (IS_ENABLE) {
    m_handler->set_alarm(Alarm::time());
    m_handler->enable();
  } else
    m_handler->disable();
}

#endif

