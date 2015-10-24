/**
 * @file IR.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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

#include "IR.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"

void
IR::Receiver::on_interrupt(uint16_t arg)
{
  UNUSED(arg);

  // Check if the buffer is full
  if (UNLIKELY(m_ix == m_max)) return;

  // Check if the time should be set; i.e. queue for timeout events
  if (m_ix == 0) {
    expire_after(TIMEOUT);
    start();
  }

  // Measure the sample period
  uint32_t now = RTT::micros();
  uint32_t us = (now - m_start);
  m_start = now;

  // Check if samples should be collected
  if (m_sample != NULL) m_sample[m_ix] = us;

  // Fix: Check for repeat code

  // And generate binary code. Skip two first and two last samples
  if (m_ix > 1 && m_ix < m_max - 2)
    m_code = (m_code << 1) + (us > m_threshold);

  // Check if all samples have been received
  if (++m_ix != m_max) return;

  // Disable further interrupts and remove from timer queue
  disable();
  stop();

  // Push an event with the received code
  Event::push(Event::READ_COMPLETED_TYPE, this, m_code);
}

void
IR::Receiver::reset()
{
  // Remove from any queue
  stop();

  // Initial state
  m_ix = 0;
  m_code = 0;

  // Reset start time and enable the interrupt handler
  m_start = RTT::micros();
  enable();
}

int
IR::Receiver::lookup(uint16_t code)
{
  if (UNLIKELY(m_keymap == NULL)) return (-1);
  for (uint8_t i = 0; i < m_keys; i++)
    if (code == pgm_read_word(&m_keymap[i].code))
      return (pgm_read_word(&m_keymap[i].key));
  return (-1);
}

IOStream& operator<<(IOStream& outs, IR::Receiver& receiver)
{
  if (UNLIKELY(receiver.m_sample == NULL)) return (outs);
  for (uint8_t ix = 0; ix < receiver.m_ix; ix++)
    outs << ix << ':' << receiver.m_sample[ix] << endl;
  return (outs);
}
