/**
 * @file Cosa/Fai/digital_pins_t.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * The Cosa Fai data stream digital pin values descriptor.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Fai.h"

static const char values_name[] PROGMEM = "values";
static const Ciao::Descriptor::member_t members[] PROGMEM = {
  {
    Ciao::UINT32_TYPE,
    1,
    values_name,
    0
  }
};
static const char name[] PROGMEM = "Cosa::Fai::digital_pins_t";
const Ciao::Descriptor::user_t Fai::Descriptor::digital_pins_t PROGMEM = {
  Fai::Descriptor::DIGITAL_PINS_ID,
  name,
  members,
  membersof(members)
};

