/**
 * @file Cosa/Queue.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
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

#include "Queue.h"

uint8_t
Queue::enqueue(void* data)
{
  if (_length == _nmemb) return (0);
  synchronized {
    memcpy(&_buffer[_put*_msize], data, _msize);
    _length += 1;
    _put += 1;
    if (_put == _nmemb) _put = 0;
  }
  return (1);
}

uint8_t
Queue::enqueue_P(const void* data)
{
  if (_length == _nmemb) return (0);
  synchronized {
    memcpy_P(&_buffer[_put*_msize], data, _msize);
    _length += 1;
    _put += 1;
    if (_put == _nmemb) _put = 0;
  }
  return (1);
}

uint8_t
Queue::dequeue(void* data)
{
  if (_length == 0) return (0);
  synchronized {
    memcpy(data, &_buffer[_get*_msize], _msize);
    _length -= 1;
    _get += 1;
    if (_get == _nmemb) _get = 0;
  }
  return (1);
}

uint8_t
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
  return (1);
}

