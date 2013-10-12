/**
 * @file Cosa/Wireless/Driver/VWI/Codec/BitstuffingCodec.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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

#ifndef __COSA_WIRELESS_DRIVER_VWI_CODEC_BITSTUFFINGCODEC_HH__
#define __COSA_WIRELESS_DRIVER_VWI_CODEC_BITSTUFFINGCODEC_HH__

#include "Cosa/Wireless/Driver/VWI.hh"

/**
 * Fixed bitstuffing 4 to 5 bit codec for the Cosa VWI (Virtual Wire
 * Interface). 
 */
class BitstuffingCodec : public VWI::Codec {
private:
  /** Message preamble */
  static const uint8_t preamble[] PROGMEM;
  
public:
  /**
   * Construct fixed bitstuffing codec with given bits per symbol,
   * start symbol, and preamble size.
   */
  BitstuffingCodec() : VWI::Codec(5, 0x34a, 8) {}
  
  /**
   * @override
   * Returns pointer to Cosa fixed bitstuffing frame preamble in program memory.
   * @return pointer.
   */
  virtual const uint8_t* get_preamble() 
  { 
    return (preamble); 
  }
  
  /**
   * @override
   * Returns fixed bitstuffed symbol for given 4-bit data.
   * @return 5-bit bitstuffed code.
   */
  virtual uint8_t encode4(uint8_t nibble) 
  { 
    return (((nibble & 0xf) << 1) + ((nibble & 0x1) == 0));
  };

  /**
   * @override
   * Returns 4-bit data for given fixed bitstuffed symbol.
   * @return 4-bit data.
   */
  virtual uint8_t decode4(uint8_t symbol)
  {
    return ((symbol >> 1) & 0xf);
  }
};

#endif
