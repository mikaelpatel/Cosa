/**
 * @file Fai/set_mode_t.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#include "Fai.hh"

#if defined(NREFLECTION)
#define descr_name 0
#define pin_name 0
#define mode_name 0
#else
static const char descr_name[] __PROGMEM = "Ciao::Fai::set_mode_t";
static const char pin_name[] __PROGMEM = "pin";
static const char mode_name[] __PROGMEM = "mode";
#endif
static const Ciao::Descriptor::member_t descr_members[] __PROGMEM = {
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
const Ciao::Descriptor::user_t Fai::Descriptor::set_mode_t __PROGMEM = {
  Fai::Descriptor::SET_MODE_ID,
  descr_name,
  descr_members,
  membersof(descr_members)
};

