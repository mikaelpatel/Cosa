/**
 * @file Library/Component.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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
 * @section Description
 * Library Component header file template.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef LIBRARY_COMPONENT_HH
#define LIBRARY_COMPONENT_HH

#include "Cosa/Types.h"

namespace Library {

class Component {
public:
  Component(uint16_t id);
  ~Component();
private:
  uint16_t m_id;
};

};

#endif
