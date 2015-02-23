/**
 * @file Cosa/UML/Capsule.hh
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

#ifndef COSA_UML_CAPSULE_HH
#define COSA_UML_CAPSULE_HH

#include "Cosa/Types.h"

namespace UML {

/**
 * The Capsule class is an abstract behavior. Typically the capsule
 * listens to Connectors. Connectors used as formal arguments to the
 * capsule capsules are also known as Ports. Capsules that generate
 * data to the connectors will also define a specific connector type.
 *
 * @section Diagram
 * @code
 *
 *     Capsule                             Capsule
 *   +---------+                         +---------+
 *   |   c1    |                         |   c2    |
 *   |         |                         |         |
 *   |      [Port]---[Connector<T>]--->[Port]      |
 *   |         |                         |         |
 *   +---------+                         +---------+
 *
 * @endcode
 */
class Capsule {
public:
  /**
   * Construct Capsule and initiate state.
   */
  Capsule() :
    is_scheduled(false)
  {}

  /**
   * @override UML::Capsule
   * The capsule behavior is run when any of the connectors it
   * listens on is changed.
   */
  virtual void behavior() = 0;

protected:
  bool is_scheduled;
  friend class Controller;
};

};
#endif
