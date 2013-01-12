/**
 * @file Cosa/Thing.hh
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
 * The Cosa class double linked circulic list item; Thing.
 * 
 * @section See Also
 * Things.hh for collection of Things.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_THING_HH__
#define __COSA_THING_HH__

#include "Cosa/Types.h"
#include "Cosa/Event.hh"

class Thing : public Event::Handler {

protected:
  /**
   * Double linked list pointers. 
   */
  Thing* m_succ;
  Thing* m_pred;

public:
  /**
   * Construct this thing and initiate to self reference.
   */
  Thing() : 
    m_succ(this),
    m_pred(this)
  {}
  
  /**
   * Return successor in sequence.
   * @return successor thing.
   */
  Thing* get_succ() 
  {
    return (m_succ);
  }

  /**
   * Return predecessor in sequence.
   * @return predecessor thing.
   */
  Thing* get_pred() 
  {
    return (m_pred);
  }

  /**
   * Attach given thing as predecessor. Will check and detach
   * if already attached.
   * @param[in] it thing to attach.
   */
  void attach(Thing* it);

  /**
   * Detach this thing from any-thing. Unlink from any collection.
   */
  void detach();
};

#endif
