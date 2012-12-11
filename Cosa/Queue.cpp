/**
 * @file Cosa/Queue.h
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
 * Ring-buffer for queueing data elements; events, serial data, etc.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Queue.h"

bool
Queue::enqueue(void* data)
{
  if (_length == _nmemb) return (0);
  synchronized {
    // memcpy(_putp, data, _msize);
    uint8_t* dp = (uint8_t*) data;
    uint8_t n = _msize;
    while (n--) *_putp++ = *dp++;
    _length += 1;
    // _putp += _msize;
    if (_putp == _lastp) _putp = _buffer;
  }
  return (1);
}

bool
Queue::enqueue_P(const void* data)
{
  if (_length == _nmemb) return (0);
  synchronized {
    // memcpy_P(_putp, data, _msize);
    uint8_t* dp = (uint8_t*) data;
    uint8_t n = _msize;
    while (n--) *_putp++ = pgm_read_byte(dp++);
    _length += 1;
    // _putp += _msize;
    if (_putp == _lastp) _putp = _buffer;
  }
  return (1);
}

bool
Queue::dequeue(void* data)
{
  if (_length == 0) return (0);
  synchronized {
    memcpy(data, _getp, _msize);
    uint8_t* dp = (uint8_t*) data;
    uint8_t n = _msize;
    while (n--) *dp++ = *_getp++;
    _length -= 1;
    // _getp += _msize;
    if (_getp == _lastp) _getp = _buffer;
  }
  return (1);
}

void
Queue::await(void* data, uint8_t mode)
{
  while (!dequeue(data)) {
    cli();
    set_sleep_mode(mode);
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
  }
}

