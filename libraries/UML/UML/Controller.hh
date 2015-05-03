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

#include "Capsule.hh"

namespace UML {

/**
 * The Controller class is responsible for the scheduling and
 * execution of capsule behavior. When a connector is updated the
 * capsules listening for change with be scheduled.
 */
class Controller {
public:
  /**
   * Construct Controller. Initiate capsule run-queue.
   */
  Controller() :
    m_put(0),
    m_get(0),
    m_cnt(0)
  {}

  /**
   * Execute behavior for all queued capsules.
   */
  void run();

  /**
   * Schedule all capsules in given NULL terminated vector.
   * @param[in] capsules null terminated vector of capsule references.
   * @return zero or negative error code.
   */
  int schedule(Capsule* const* capsules)
  {
    if (UNLIKELY(capsules == NULL)) return (0);
    Capsule* capsule;
    while((capsule = (Capsule*) pgm_read_word(capsules++)) != NULL) {
      int res = schedule(capsule);
      if (UNLIKELY(res < 0)) return (res);
    }
    return (0);
  }

  /**
   * Schedule given capsule. Append to controller capsule queue if
   * not already in the queue. Returns one if scheduled, zero if
   * already scheduled or negative error code. Can be called from
   * interrupt service routine.
   * @param[in] capsule pointer to capsule to append.
   * @return zero, one or negative error code.
   */
  int schedule(Capsule* capsule);

protected:
  static const uint8_t QUEUE_MAX = 32; //!< Default run-time queue size.
  Capsule* m_queue[QUEUE_MAX];	      //!< Run-time queue.
  uint8_t m_put;		      //!< Index of queue head.
  uint8_t m_get;		      //!< Index of queue tail.
  uint8_t m_cnt;		      //!< Number of capsules in queue.
};

/**
 * Controller Single-ton.
 */
extern UML::Controller controller;

};

#endif
