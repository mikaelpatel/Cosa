/**
 * @file UML/Capsule.hh
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
 * listens to Connectors. The connector formal arguments to the
 * capsule are called ports. Capsules that generate data to the
 * connectors will also define the connector type.
 */
class Capsule {
public:
  /**
   * @override Capsule
   * The capsule behavior is run when any of the connectors it
   * listens on is changed.
   */
  virtual void behavior() = 0;
};

};
#endif
