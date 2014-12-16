/**
 * @file Cosa/IOEvent.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#ifndef COSA_IOEVENT_HH
#define COSA_IOEVENT_HH

#include "Cosa/Types.h"
#include "Cosa/Event.hh"
#include "Cosa/IOStream.hh"

/**
 * IOEvent is an IOStream filter class that may push events when
 * data has been received and send completed. Extends a given
 * IOStream::Device class. Can be used to map an interrupt driven
 * device such as UART to event driven on completed input lines.
 * @param[in] T IOStream::Device class to filter.
 */
template <typename T>
class IOEvent : public T {
public:
  /**
   * Constuct IOEvent object for filtered stream operations. 
   */
  IOEvent(Event::Handler* handler) : T(), m_handler(handler) {}

  /**
   * @override IOStream::Device<T>
   * Write character to device.
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c)
  {
    int res = T::putchar(c);
    if (c == '\n' || T::room() == 0) 
      Event::push(Event::RECEIVE_COMPLETED_TYPE, m_handler, this);
    return (res);
  }

  /**
   * @override IOStream::Device<T>
   * Read character from device.
   * @return character or EOF(-1).
   */
  virtual int getchar()
  {
    int res = T::getchar();
    if (res == IOStream::EOF) 
      Event::push(Event::SEND_COMPLETED_TYPE, m_handler, this);
    return (res);
  }

protected:
  Event::Handler* m_handler;
};

#endif
