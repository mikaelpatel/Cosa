/**
 * @file Cosa/RTCMeasure.hh
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_RTCMeasure_HH__
#define __COSA_RTCMeasure_HH__

#include "Cosa/Timer.hh"

/**
 * This class is used to gather performance timings of the RTC::Timer class.
 *   See also RTC.CPP and CosaBenchmarkRTCTimer.ino
 */
class RTCMeasure : public Timer {
  volatile bool m_dispatched;
public:
  RTCMeasure() : Timer(), m_dispatched(false) {};
  
  bool is_dispatched() const { return m_dispatched; }
  
  virtual void start();
  virtual void on_expired();

  static uint8_t start_immediate_cycles;
  static uint8_t start_queued_cycles;
  static uint8_t setup_cycles;
  static uint8_t dispatch_cycles;
  
  static const uint16_t I_PER_CYCLE;
};

#endif
