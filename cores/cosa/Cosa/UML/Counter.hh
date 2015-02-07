/**
 * @file Cosa/UML/Counter.hh
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

#ifndef COSA_UML_COUNTER_HH
#define COSA_UML_COUNTER_HH

#include "Cosa/UML/Capsule.hh"
#include "Cosa/UML/Connector.hh"

namespace UML {

/**
 * Counter Capsule class. Count the number of signals on a given
 * connector.
 * @param[in] T connector type to count.
 *
 * @section Diagram
 *
 *              Counter<T>
 *            +------------+
 *            |    c1      |
 *  ---[T]--->|            |---[Count]--->
 *            |            |
 *            +------------+
 *
 */
template<typename T>
class Counter : public Capsule {
public:
  typedef Connector<uint32_t> Count;

  /**
   * Construct counter capsule with given trigger signal and counter
   * value connnector.
   * @param[in] signal input signal to count.
   * @param[in] count value connector.
   */
  Counter(T& signal, Count& count) :
    Capsule(),
    m_signal(signal),
    m_count(count)
  {}

  /**
   * Count number of calls.
   */
  virtual void behavior()
  {
    m_count = m_count + 1;
  }

  /**
   * Reset counter.
   */
  virtual void reset()
  {
    m_count = 0L;
  }

protected:
  T& m_signal;
  Count& m_count;
};

};

#endif
