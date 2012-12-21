/**
 * @file Cosa/Things.hh
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
 * The Cosa low level callback set handling.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_THINGS_HH__
#define __COSA_THINGS_HH__

#include "Cosa/Types.h"
#include "Cosa/Thing.hh"

class Things : public Thing {

public:
  /**
   * Default event handler for thing collection. Boardcase event.
   * @param[in] things the collection.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  static void broadcast(Thing* things, uint8_t type, uint16_t value);

  /**
   * Construct a thing collection.
   */
  Things() : Thing(broadcast) {}

  /**
   * Return number of things.
   * @return length
   */
  uint8_t length();

  /**
   * Return true(1) if there are no things otherwise false(0).
   * @return bool
   */
  bool is_empty()
  {
    return (_succ == this);
  }
};

#endif
