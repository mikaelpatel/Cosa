/**
 * @file Fai.cpp
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

static char MAGIC[] = "Cosa::Fai";
static const uint8_t MAJOR = 1;
static const uint8_t MINOR = 0;

void
Fai::begin()
{
  header_t header;
  header.magic = MAGIC;
  header.major = MAJOR;
  header.minor = MINOR;
  header.endian = LITTLE_ENDIAN;
  Ciao::write(&Ciao::Descriptor::header_t, &header, 1);
}

#if defined(BOARD_ATTINYX5)

void
Fai::write(uint32_t mask)
{
  digital_pins_t dgl;
  dgl.values = (PINB & mask);
  Ciao::write(&Descriptor::digital_pins_t, &dgl, 1);
}

#elif defined(BOARD_ATTINYX4) || defined(BOARD_ATTINYX61)

void
Fai::write(uint32_t mask)
{
  digital_pins_t dgl;
  dgl.values = ((PINB << 8) | PINA) & mask;
  Ciao::write(&Descriptor::digital_pins_t, &dgl, 1);
}

#else

void
Fai::write(uint32_t mask)
{
  digital_pins_t dgl;
  dgl.values = ((PINB << 8) | PIND) & mask;
  Ciao::write(&Descriptor::digital_pins_t, &dgl, 1);
}

#endif

void
Fai::write(Pin* pin)
{
  digital_pin_t dgl;
  dgl.pin = pin->pin();
  dgl.value = pin->is_set();
  Ciao::write(&Descriptor::digital_pin_t, &dgl, 1);
}

void
Fai::write(AnalogPin* pin)
{
  analog_pin_t ang;
  ang.pin = pin->pin();
  ang.value = pin->value();
  Ciao::write(&Descriptor::analog_pin_t, &ang, 1);
}
