/**
 * @file Cosa/BitSet.hh
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

#ifndef COSA_BITSET_HH
#define COSA_BITSET_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

/**
 * Bitset implemented as a template class with a byte vector for the
 * elements as bits.
 * @param[in] N max number of elements in bitset.
 */
template<uint16_t N>
class BitSet {
public:
  /**
   * Construct bitset and empty.
   */
  BitSet()
    __attribute__((always_inline))
  {
    empty();
  }

  /**
   * Return max number of elements in the bitset.
   */
  uint16_t members() const
  {
    return (N);
  }

  /**
   * Return bit vector.
   * @return bit vector.
   */
  const uint8_t* bits() const
  {
    return (m_set);
  }

  /**
   * Empty bitset.
   */
  void empty()
    __attribute__((always_inline))
  {
    memset(m_set, 0, sizeof(m_set));
  }

  /**
   * Return true if the bitset is empty.
   */
  bool is_empty() const
  {
    for (uint16_t i = 0; i < sizeof(m_set); i++)
      if (m_set[i] != 0) return (false);
    return (true);
  }

  /**
   * Check if the given element index is a member of the bitset.
   * @return bool
   */
  bool operator[](uint16_t ix) const
  {
    return ((ix < N) ? ((m_set[ix / CHARBITS] & _BV(ix & MASK)) != 0) : false);
  }

  /**
   * Add element index to the bitset.
   * @param[in] ix element to add (0..N-1).
   */
  void operator+=(uint16_t ix)
  {
    if (ix < N) m_set[ix / CHARBITS] |= _BV(ix & MASK);
  }

  /**
   * Remove element index from the bitset.
   * @param[in] ix element to remove (0..N-1).
   */
  void operator-=(uint16_t ix)
  {
    if (ix < N) m_set[ix / CHARBITS] &= ~_BV(ix & MASK);
  }

  /**
   * Assign bitset with given value. Bitset must be the same size.
   * @param[in] rhs bitset to assign from.
   */
  void operator=(BitSet& rhs)
  {
    if (UNLIKELY(rhs.members() != N)) return;
    for (uint16_t i = 0; i < sizeof(m_set); i++)
      m_set[i] = rhs.m_set[i];
  }

  /**
   * Add element from the given bitset. Bitset must be the same size.
   * @param[in] rhs bitset to add from.
   */
  void operator+=(BitSet& rhs)
  {
    if (UNLIKELY(rhs.members() != N)) return;
    for (uint16_t i = 0; i < sizeof(m_set); i++)
      m_set[i] |= rhs.m_set[i];
  }

  /**
   * Remove elements from the given bitset. Bitset must be the same size.
   * @param[in] rhs bitset to remove.
   */
  void operator-=(BitSet& rhs)
  {
    if (UNLIKELY(rhs.members() != N)) return;
    for (uint16_t i = 0; i < sizeof(m_set); i++)
      m_set[i] &= ~rhs.m_set[i];
  }

  /**
   * Check if the bitsets are equal. Return false if they are not of same
   * size.
   * @param[in] rhs bitset to assign from.
   */
  bool operator==(BitSet& rhs)
  {
    if (UNLIKELY(rhs.members() != N)) return (false);
    return (memcmp(m_set, rhs.m_set, sizeof(m_set)) == 0);
  }

  /**
   * Print bitset to the given output stream.
   * @param[in] outs output stream.
   * @param[in] rhs bit set to print.
   * @return output stream.
   */
  friend IOStream& operator<<(IOStream& outs, BitSet& rhs)
  {
    for (uint16_t i = 0; i < N; i++)
      outs << ((rhs.m_set[i / CHARBITS] & _BV(i & MASK)) != 0);
    return (outs);
  }

private:
  /** Mask bit address. */
  static const uint8_t MASK = (CHARBITS - 1);

  /** Bitset size in bytes. */
  static const size_t SET_MAX = (N + (CHARBITS/2)) / CHARBITS;

  /** Bitset storage. */
  uint8_t m_set[SET_MAX];
};
#endif
