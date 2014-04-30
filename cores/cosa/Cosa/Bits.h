/**
 * @file Cosa/Bits.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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
 * Bit mask, set and field access macro set.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_BITS_H
#define COSA_BITS_H

#define bit_mask(b) (1 << (b))
#define bit_mask_get(p,m) ((p) & (m))
#define bit_mask_set(p,m) ((p) |= (m))
#define bit_mask_clear(p,m) ((p) &= ~(m))
#define bit_mask_toggle(p,m) ((p) ^= (m))
#define bit_mask_write(c,p,m) (c ? bitmask_set(p,m) : bitmask_clear(p,m)) 

#define bit_get(p,b) ((p) & bit_mask(b))
#define bit_set(p,b) ((p) |= bit_mask(b))
#define bit_clear(p,b) ((p) &= ~bit_mask(b))
#define bit_toggle(p,b) ((p) ^= bit_mask(b))
#define bit_write(c,p,b) (c ? bit_set(p,b) : bit_clear(p,b)) 

#define bit_field_set(p,m,v) ((p) = ((p) & ~(m)) | ((v) & (m)))
#endif

