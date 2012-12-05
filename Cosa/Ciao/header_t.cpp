/**
 * @file Cosa/Ciao/header_t.cpp
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
#include <avr/pgmspace.h>

// Ciao configuration
static const uint16_t ID = 0x00;
static char MAGIC[] = "Cosa::Ciao";
static const uint8_t MAJOR = 1;
static const uint8_t MINOR = 0;

// Ciao header declaration 
static const char magic_name[] PROGMEM = "magic";
static const char major_name[] PROGMEM = "major";
static const char minor_name[] PROGMEM = "minor";
static const char endian_name[] PROGMEM = "endian";
static const Ciao::decl_member_t member[] PROGMEM = {
  {
    Ciao::UINT8_TYPE,		// type
    0,				// count
    magic_name,			// name
    0				// decl
  },
  {
    Ciao::UINT8_TYPE,		// type
    1,				// count
    major_name,			// name
    0				// decl
  },
  {
    Ciao::UINT8_TYPE,		// type
    1,				// count
    minor_name,			// name
    0				// decl
  },
  {
    Ciao::UINT8_TYPE,		// type
    1,				// count
    endian_name,		// name
    0				// decl
  }
};
static const char header_name[] PROGMEM = "Ciao::header_t";
const Ciao::decl_user_t Ciao::_header_decl PROGMEM = {
  ID,				// id
  header_name,			// name
  member,			// member
  membersof(member)		// count
};  

// Ciao header with magic string, revision and endian information
Ciao::header_t Ciao::_header = {
  MAGIC,
  MAJOR,
  MINOR,
  LITTLE_ENDIAN
};
