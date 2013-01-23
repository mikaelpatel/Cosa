/**
 * @file Cosa/TWI/Driver/AT23CXX.cpp
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
 * Driver for the AT24CXX 2-Wire Serial EEPROM.
 * See Atmel Product description (Rev. 0336K-SEEPR-7/03).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI/Driver/AT24CXX.hh"

int
AT24CXX::read(void* buf, uint8_t size, uint16_t addr)
{
  if (!twi.begin()) return (-1);
  twi.write(m_addr, addr);
  int count = twi.read(m_addr, buf, size);
  twi.end();
  return (count);
}

int 
AT24CXX::write(void* buf, uint8_t size, uint16_t addr)
{
  if (!twi.begin()) return (-1);
  twi.write(m_addr, addr);
  int count = twi.write(m_addr, addr, buf, size);
  twi.end();
  if (count > 0) count -= sizeof(addr);
  return (count);
}
