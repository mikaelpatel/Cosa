/**
 * @file Cosa/Fai/event_t.cpp
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
 * The Cosa Fai data stream Cosa event descriptor.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Fai.hh"

#if defined(NREFLECTION)
#define descr_name 0
#define type_name 0
#define target_name 0
#define value_name 0
#else
static const char descr_name[] PROGMEM = "Cosa::Event";
static const char type_name[] PROGMEM = "type";
static const char target_name[] PROGMEM = "target";
static const char value_name[] PROGMEM = "value";
#endif
static const Ciao::Descriptor::member_t descr_members[] PROGMEM = {
  {
    Ciao::UINT8_TYPE,
    1,
    type_name,
    0
  },
  {
    Ciao::UINT16_TYPE,
    1,
    target_name,
    0
  },
  {
    Ciao::UINT16_TYPE,
    1,
    value_name,
    0
  }
};
const Ciao::Descriptor::user_t Fai::Descriptor::event_t PROGMEM = {
  Fai::Descriptor::EVENT_ID,
  descr_name,
  descr_members,
  membersof(descr_members)
};

