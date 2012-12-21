/**
 * @file Cosa/Fai.cpp
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
 * The Cosa Ciao data stream of Arduino state (Cosa fai).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Fai.hh"

// Ciao configuration
static char MAGIC[] = "Cosa::Fai";
static const uint8_t MAJOR = 1;
static const uint8_t MINOR = 0;

// Ciao header with magic string, revision and endian information
Ciao::header_t Fai::header = {
  MAGIC,
  MAJOR,
  MINOR,
  LITTLE_ENDIAN
};

void 
Fai::write(uint32_t mask)
{
  digital_pins_t dgl;
  dgl.values = ((PINB << 8) | PIND) & mask;
  Ciao::write(&Descriptor::digital_pins_t, &dgl, 1);
}

void 
Fai::write(Pin* pin)
{
  digital_pin_t dgl;
  dgl.pin = pin->get_pin();
  dgl.value = pin->is_set();
  Ciao::write(&Descriptor::digital_pin_t, &dgl, 1);
}

void 
Fai::write(AnalogPin* pin)
{
  analog_pin_t ang;
  ang.pin = pin->get_pin();
  ang.value = pin->get_value();
  Ciao::write(&Descriptor::analog_pin_t, &ang, 1);
}
