/**
 * @file SensorHandler.cpp
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

#include "SensorHandler.h"
#include "Cosa/Trace.hh"

void
SensorHandler::run()
{
  ThingSpeak::Entry update;
  m_sensor.sample();
  update.set_field(1, m_sensor.get_temperature(), 1);
  update.set_field(2, m_sensor.get_humidity(), 1);
  TRACE(m_channel->post(update));
}
