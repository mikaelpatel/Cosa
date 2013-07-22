/**
 * @file Cosa/Fai/set_mode_t.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Fai.hh"

#if defined(NREFLECTION)
#define descr_name 0
#define pin_name 0
#define mode_name 0
#else
static const char descr_name[] PROGMEM = "Ciao::Fai::set_mode_t";
static const char pin_name[] PROGMEM = "pin";
static const char mode_name[] PROGMEM = "mode";
#endif
static const Ciao::Descriptor::member_t descr_members[] PROGMEM = {
  {
    Ciao::UINT8_TYPE,
    1,
    pin_name,
    0
  },
  {
    Ciao::UINT8_TYPE,
    1,
    mode_name,
    0
  }
};
const Ciao::Descriptor::user_t Fai::Descriptor::set_mode_t PROGMEM = {
  Fai::Descriptor::SET_MODE_ID,
  descr_name,
  descr_members,
  membersof(descr_members)
};

