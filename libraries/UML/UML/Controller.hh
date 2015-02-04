/**
 * @file UML/Controller.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#ifndef UML_CONTROLLER_HH
#define UML_CONTROLLER_HH

#include "UML/Capsule.hh"
#include "Cosa/Event.hh"

namespace UML {

/**
 * The Controller class is responsible for the scheduling and
 * execution of capsule behavior. With a connector is assigned the
 * capsules listening for change with be scheduled. 
 */
class Controller {
public:
  /** 
   * Construct Controller. Initiate capsule queue.
   */
  Controller() :
    m_put(0),
    m_get(0),
    m_available(0)
  {}

  /**
   * Execute behavior for all queued capsules.
   */
  void run()
  {
    Event::service();
    while (m_available) {
      uint16_t ix;
      synchronized {
	ix = m_get;
	m_get += 1;
	if (m_get == QUEUE_MAX) m_get = 0;
	m_available -= 1;
      }
      m_queue[ix]->behavior();
    }
    yield();
  }

  /**
   * Schedule all capsules in given NULL terminated vector.
   * @param[in] capsules null terminated vector of capsule references.
   * @return zero or negative error code.
   */
  int schedule(Capsule* const* capsules)
  {
    Capsule* capsule;
    while((capsule = (Capsule*) pgm_read_word(capsules++)) != NULL) {
      if (schedule(capsule) < 0) return (-1);
    }
    return (0);
  }

  /**
   * Schedule given capsule. Append to controller capsule queue if
   * not already in the queue. Returns zero or negative error code.
   * @param[in] capsule pointer to capsule to append.
   * @return zero or negative error code.
   */
  int schedule(Capsule* capsule)
  {
    if (m_available == QUEUE_MAX) return (-1);
    synchronized {
      uint16_t ix = m_get;
      while (ix != m_put) {
	if (m_queue[ix] == capsule) synchronized_return (0);
	ix += 1;
	if (ix == QUEUE_MAX) ix = 0;
      }
      m_queue[m_put++] = capsule;
      m_available += 1;
      if (m_put == QUEUE_MAX) m_put = 0;
    }
    return (1);
  }
  
protected:
  static const size_t QUEUE_MAX = 32;
  Capsule* m_queue[QUEUE_MAX];
  uint16_t m_put;
  uint16_t m_get;
  uint16_t m_available;
};

extern UML::Controller controller;

};

#endif
