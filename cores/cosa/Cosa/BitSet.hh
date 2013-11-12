/**
 * @file Cosa/BitSet.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_BITSET_HH__
#define __COSA_BITSET_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

/**
 * Bitset implemented as a template class with a byte vector for the 
 * elements as bits.
 * @param[in] N number of elements.
 */
template<uint16_t N>
class BitSet {
private:
  // Mask bit address
  static const uint8_t MASK = (CHARBITS - 1);
  static const size_t SET_MAX = (N + (CHARBITS/2)) / CHARBITS;
 
 // Bitset storage
  uint8_t m_set[SET_MAX];

public:
  /**
   * Construct bitset and empty.
   */
  BitSet() 
  {
    empty();
  }

  /**
   * Return number of elements in the bitset.
   */
  uint16_t members() 
  {
    return (N);
  }

  /**
   * Empty bitset.
   */
  void empty()
  {
    memset(m_set, 0, sizeof(m_set));
  }

  /**
   * Check if the given element index is a member of the bitset.
   * @return bool
   */
  bool operator[](uint16_t ix) 
  {
    if (ix < N) 
      return ((m_set[ix / CHARBITS] & _BV(ix & MASK)) != 0);
    return (false);
  }
  
  /**
   * Add element index to the bitset.
   */
  void operator+=(uint16_t ix)
  {
    if (ix < N) m_set[ix / CHARBITS] |= _BV(ix & MASK);
  }

  /**
   * Remove element index from the bitset.
   */
  void operator-=(uint16_t ix)
  {
    if (ix < N) m_set[ix / CHARBITS] &= ~_BV(ix & MASK);
  }

  /**
   * Assign bitset with given value. Bitset must be the same size.
   * @param[in] x bitset to assign from.
   */
  void operator=(BitSet& x)
  {
    if (x.members() != N) return;
    for (uint16_t i = 0; i < sizeof(m_set); i++) 
      m_set[i] = x.m_set[i];
  }

  /**
   * Add element from the given bitset. Bitset must be the same size.
   * @param[in] x bitset to add from.
   */
  void operator+=(BitSet& x)
  {
    if (x.members() != N) return;
    for (uint16_t i = 0; i < sizeof(m_set); i++) 
      m_set[i] |= x.m_set[i];
  }

  /**
   * Remove elements from the given bitset. Bitset must be the same size.
   * @param[in] x bitset to remove.
   */
  void operator-=(BitSet& x)
  {
    if (x.members() != N) return;
    for (uint16_t i = 0; i < sizeof(m_set); i++) 
      m_set[i] &= ~x.m_set[i];
  }

  void print(IOStream& outs)
  {
    for (uint16_t i = 0; i < N; i++) 
      outs << ((m_set[i / CHARBITS] & _BV(i & MASK)) != 0);
    outs << endl;
  }
};
#endif
