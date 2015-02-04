/**
 * @file UML/Connector.hh
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
 * @section Description
 * A simple component based language with Capsules, Connectors and
 * Controller.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_UML_CONNECTOR_HH
#define COSA_UML_CONNECTOR_HH

#include "UML/Controller.hh"

namespace UML {

/**
 * The Connector class for a given type. Implements value set and
 * get. The setting of the connector value will force the scheduling
 * of capsules that are listening for value change. 
 */
template<typename T>
class Connector {
public:
  /**
   * Construct Connector with given null terminated vector of
   * listeners, and initial value.
   * @param[in] listeners null terminated vector of capsule references.
   * @param[in] value initial value of connector.
   */
  Connector(Capsule* const* listeners, T value) : 
    m_value(value),
    m_listeners(listeners)
  {}

  /**
   * Set the connector with given value. Schedule listener
   * capsules. 
   * @param[in] value to update with.
   * @return value.
   */
  T operator=(T value)
  {
    m_value = value;
    controller.schedule(m_listeners);
    return (value);
  }

  /**
   * Get the connector value.
   * @return value.
   */
  operator T()
  {
    return (m_value);
  }
  
protected:
  T m_value;
  Capsule* const* m_listeners;
};

};
#endif
