/**
 * @file Cosa/Ciao/header_desc.cpp
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
 * The Cosa Ciao data stream handler header descriptor.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Ciao.h"

// Ciao configuration
static char MAGIC[] = "Cosa::Ciao";
static const uint8_t MAJOR = 1;
static const uint8_t MINOR = 0;

// Ciao header with magic string, revision and endian information
Ciao::header_t Ciao::header = {
  MAGIC,
  MAJOR,
  MINOR,
  LITTLE_ENDIAN
};

// Ciao header descriptor 
static const char magic_name[] PROGMEM = "magic";
static const char major_name[] PROGMEM = "major";
static const char minor_name[] PROGMEM = "minor";
static const char endian_name[] PROGMEM = "endian";
static const Ciao::Descriptor::member_t members[] PROGMEM = {
  {
    Ciao::UINT8_TYPE,
    0,
    magic_name,
    0
  },
  {
    Ciao::UINT8_TYPE,
    1,
    major_name,
    0
  },
  {
    Ciao::UINT8_TYPE,
    1,
    minor_name,
    0
  },
  {
    Ciao::UINT8_TYPE,
    1,
    endian_name,
    0
  }
};
static const char name[] PROGMEM = "Ciao::header_t";
const Ciao::Descriptor::user_t Ciao::Descriptor::header_t PROGMEM = {
  Ciao::Descriptor::HEADER_ID,
  name,
  members,
  membersof(members)
};  

