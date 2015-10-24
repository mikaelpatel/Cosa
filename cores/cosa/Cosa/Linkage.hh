/**
 * @file Cosa/Linkage.hh
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

#ifndef COSA_LINKAGE_HH
#define COSA_LINKAGE_HH

#include "Cosa/Types.h"
#include "Cosa/Event.hh"

/**
 * The Cosa collection handling class; double linked circulic list.
 *
 * @section Acknowledgements
 * These classes are inspired by the Simula-67 SIMSET Linkage classes.
 *
 * @section References
 * 1. Historical document, http://www.edelweb.fr/Simula/scb-14.pdf
 */
class Linkage : public Event::Handler {
public:
  /**
   * Construct this linkage and initiate to self reference.
   */
  Linkage() :
    Event::Handler(),
    m_succ(this),
    m_pred(this)
  {}

  /**
   * Return successor in sequence.
   * @return successor linkage.
   */
  Linkage* succ() const
  {
    return (m_succ);
  }

  /**
   * Return predecessor in sequence.
   * @return predecessor linkage.
   */
  Linkage* pred() const
  {
    return (m_pred);
  }

  /**
   * Attach given linkage as predecessor. Will check and detach
   * if already attached.
   * @param[in] pred linkage to attach.
   * @note atomic
   */
  void attach(Linkage* pred)
  {
    synchronized {

      // Check if it needs to be detached first
      if (UNLIKELY(pred->m_succ != pred)) {
	pred->m_succ->m_pred = pred->m_pred;
	pred->m_pred->m_succ = pred->m_succ;
      }

      // Attach pred as the new predecessor
      pred->m_succ = this;
      pred->m_pred = this->m_pred;
      this->m_pred->m_succ = pred;
      this->m_pred = pred;
    }
  }

protected:
  /**
   * Double linked list pointers.
   */
  Linkage* m_succ;
  Linkage* m_pred;

  /**
   * Detach this linkage. Unlink from any list.
   * @note atomic
   */
  void detach()
  {
    // Check that the detach is necessary
    if (UNLIKELY(m_succ == this)) return;

    // Unlink and initiate to self reference
    synchronized {
      m_succ->m_pred = m_pred;
      m_pred->m_succ = m_succ;
      m_succ = this;
      m_pred = this;
    }
  }
};

class Link : public Linkage {
public:
  /**
   * Construct a link.
   */
  Link() : Linkage() {}

  /**
   * Detach this link. Unlink from any list.
   */
  void detach()
    __attribute__((always_inline))
  {
    Linkage::detach();
  }
};

class Head : public Linkage {
public:
  /**
   * Construct a double linked list queue head.
   */
  Head() : Linkage() {}

  /**
   * Return number of elements in double linked list.
   * @return elements.
   */
  int available()
  {
    int res = 0;
    // Iterate through the list and count the length of the queue
    for (Linkage* link = m_succ; link != this; link = link->succ()) res++;
    return (res);
  }

  /**
   * Return true(1) if the queue is empty otherwise false(0).
   * @return bool.
   */
  bool is_empty() const
  {
    return (m_succ == this);
  }

private:
  /**
   * @override{Event::Handler}
   * Event handler. Default event handler for collections.
   * Will boardcase the event to the collection.
   * @param[in] type the event type.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value)
  {
    // Iterate through the list and dispatch the event
    Linkage* link = m_succ;
    while (link != this) {
      // Get the successor as the current event call may detach itself
      Linkage* succ = link->succ();
      link->on_event(type, value);
      link = succ;
    }
  }
};

#endif
