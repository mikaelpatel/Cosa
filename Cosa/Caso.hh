/**
 * @file Cosa/Caso.hh
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
 * The Cosa class hierarchy root object; basic event handler (virtual
 * method).  
 * 
 * @section See Also
 * Things.hh for collection of Things, and Event.hh for details on
 * event types and parameter passing.  
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_CASO_HH__
#define __COSA_CASO_HH__

#include "Cosa/Types.h"

class Caso {

public:
  /**
   * Default null event handler. Should be redefined by sub-classes.
   * Called by Event::dispatch(). See Event.hh for details on event
   * types and value passing.
   * @param[in] type the event type.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value) {}
};

#endif
