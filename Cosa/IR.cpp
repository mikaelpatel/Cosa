/**
 * @file Cosa/IR.cpp
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Cosa IR receiver/TSOP4838 driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IR.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"

void 
IR::Receiver::on_interrupt() 
{ 
  // Check if the buffer is full
  if (m_ix == m_max) return;

  // Check if the time should be set; i.e. queue for timeout events
  if (m_ix == 0) Watchdog::attach(this, TIMEOUT);

  // Measure the sample period
  uint32_t stop = RTC::micros();
  uint32_t us = (stop - m_start);
  m_start = stop;
  
  // Check if samples should be collected
  if (m_sample != 0) m_sample[m_ix] = us;

  // Fix: Check for repeat code

  // And generate binary code. Skip two first and two last samples
  if (m_ix > 1 && m_ix < m_max - 2)
    m_code = (m_code << 1) + (us > m_threshold);
  
  // Check if all samples have been received
  if (++m_ix != m_max) return;

  // Disable further interrupts and remove from timer queue
  disable();
  detach();

  // Push an event with the received code
  Event::push(Event::READ_COMPLETED_TYPE, (InterruptPin*) this, m_code);
}

void 
IR::Receiver::reset()
{
  // Remove from any queue
  detach();

  // Initial state
  m_ix = 0;
  m_code = 0;

  // Reset start time and enable the interrupt handler
  m_start = RTC::micros();
  enable();
}

void 
IR::Receiver::print(IOStream& out)
{
  if (m_sample == 0) return;
  for (uint8_t ix = 0; ix < m_ix; ix++) {
    out.printf_P(PSTR("%d: %ud\n"), ix, m_sample[ix]);
  }
}

int
IR::Receiver::lookup(uint16_t code)
{
  if (m_keymap == 0) return (-1);
  for (uint8_t i = 0; i < m_keys; i++)
    if (code == pgm_read_word(&m_keymap[i].code))
      return (pgm_read_word(&m_keymap[i].key));
  return (-1);
}
