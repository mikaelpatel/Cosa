/**
 * @file Cosa/Ciao/sample_request_desc.cpp
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
 * The Cosa Ciao data stream sample request descriptor.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Ciao.h"

static const char pins_name[] PROGMEM = "pins";
static const char period_name[] PROGMEM = "period";
static const Ciao::desc_member_t members[] PROGMEM = {
  {
    Ciao::UINT32_TYPE,
    1,
    pins_name,
    0
  },
  {
    Ciao::UINT16_TYPE,
    1,
    period_name,
    0
  }
};
static const char name[] PROGMEM = "Ciao::sample_request_t";
const Ciao::desc_user_t Ciao::sample_request_desc PROGMEM = {
  Ciao::SAMPLE_REQUEST_ID,
  name,
  members,
  membersof(members)
};

