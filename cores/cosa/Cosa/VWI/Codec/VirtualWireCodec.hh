/**
 * @file Cosa/VWI/Codec/VirtualWireCodec.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2008-2013, Mike McCauley (Author/VirtualWire)
 * Copyright (C) 2013, Mikael Patel (Cosa C++ port and refactoring)
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

#ifndef __COSA_VWI_CODEC_VIRTUALWIRECODEC_HH__
#define __COSA_VWI_CODEC_VIRTUALWIRECODEC_HH__

#include "Cosa/VWI.hh"

/**
 * VirtualWire 4-to-6 bit codec for the Cosa VWI (Virtual Wire
 * Interface). 
 */
class VirtualWireCodec : public VWI::Codec {
private:
  /** Symbol mapping table: 4 to 6 bits */
  static const uint8_t symbols[] PROGMEM;

  /** Message preamble with start symbol */
  static const uint8_t preamble[] PROGMEM;
  
public:
  /**
   * Construct VirtualWire codec with given bits per symbol, start symbol,
   * and preamble size.
   */
  VirtualWireCodec() : VWI::Codec(6, 0xb38, 8) {}
  
  /**
   * @override
   * Returns pointer to VirtualWire frame preamble in program memory.
   * @return pointer.
   */
  virtual const uint8_t* get_preamble() 
  { 
    return (preamble); 
  }
  
  /**
   * @override
   * Returns symbol for given 4-bit data.
   * @return 6-bit code.
   */
  virtual uint8_t encode4(uint8_t nibble) 
  { 
    return (pgm_read_byte(&symbols[nibble & 0xf]));
  };

  /**
   * @override
   * Returns 4-bit data for given symbol.
   * @return 4-bit data.
   */
  virtual uint8_t decode4(uint8_t symbol);
};

#endif
