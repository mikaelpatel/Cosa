/**
 * @file CosaIRreceiver.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * Cosa IR receiver for LG remote using a TSOP4838 for decoding
 * IR transmission. Uses the Watchdog to monitor the decoding.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// Enable to allow the IR receiver interrupt handler to collect time periods
#define COLLECT_SAMPLES

class IRreceiver : public InterruptPin, private Link {
private:
  static const uint16_t TIMEOUT = 512;
  static const uint16_t PRE_DATA = 0x20df;
  static const uint8_t PRE_DATA_BITS = 16;
  static const uint8_t SAMPLE_MAX = 40;
  volatile uint32_t m_threshold;
  volatile uint32_t m_start;
  volatile uint32_t m_code;
  volatile uint8_t m_ix;
  const uint8_t m_max;

#ifdef COLLECT_SAMPLES
  // Time period samples
  volatile uint16_t m_sample[SAMPLE_MAX];
#endif

  /**
   * @override
   * Interrupt pin handler: Measure time period in code sequence from
   * IR receiver circuit. Push an event when a valid code sequence has 
   * been recieved. 
   */
  virtual void on_interrupt() 
  { 
    // Check if the time should be set; i.e. queue for timeout events
    if (m_ix == 0) Watchdog::attach(this, TIMEOUT);
    if (m_ix == m_max) return;

    // Measure the sample period
    uint32_t stop = RTC::micros();
    uint32_t us = (stop - m_start);
    m_start = stop;

    // And generate binary code. Skip two first and two last samples
    if (m_ix > 1 && m_ix < m_max - 2)
      m_code = (m_code << 1) + (us > m_threshold);

#ifdef COLLECT_SAMPLES
    m_sample[m_ix] = us;
#endif
    
    // Push event when all samples have been received
    m_ix += 1;
    if (m_ix == m_max) {
      // Remove from timer queue; cancel the timer
      detach();
      // Check the header and push event
      if ((m_code >> PRE_DATA_BITS) == PRE_DATA) {
	Event::push(Event::READ_COMPLETED_TYPE, 0, m_code);
      }
    }
  }

public:
  /**
   * Construct an IRreceiver connected to the given interrupt pin,
   * capture given max number of samples, and decode to binary
   * with the given threshold.
   */
  IRreceiver(Board::InterruptPin pin, uint8_t max, uint32_t threshold) :
    InterruptPin(pin, InterruptPin::ON_FALLING_MODE),
    Link(),
    m_threshold(threshold),
    m_start(0),
    m_ix(0),
    m_max(max < SAMPLE_MAX ? max : SAMPLE_MAX),
    m_code(0)
  {}

  /**
   * Reset the IR receive for the next code sequence.
   */
  void reset()
  {
    detach();
    m_ix = 0;
    m_code = 0;
    m_start = RTC::micros();
  }

  /**
   * Print the captured samples to the given output stream.
   * @param[in] out stream.
   */
  void print(IOStream& out)
  {
#ifdef COLLECT_SAMPLES
    for (uint8_t ix = 0; ix < m_ix; ix++) {
      out.printf_P(PSTR("%d: %ud\n"), ix, m_sample[ix]);
    }
#endif
  }
};

// IR receiver for an LG is 36 samples and the binary threshold [1000..2000].
IRreceiver receiver(Board::EXT0, 36, 1500);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaIRreceiver: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(Link));
  TRACE(sizeof(InterruptPin));
  TRACE(sizeof(IRreceiver));

  // Print pin configuration
  receiver.Pin::print(trace);

  // Use the real-time clock for time measurement
  RTC::begin();

  // Use the watchdog for timeouts
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);

  // Enable the interrupt pin to capture the remote code sequence
  receiver.enable();
}

void loop()
{
  // Wait for an event from the IR receiver
  Event event;
  Event::queue.await(&event);
  uint8_t type = event.get_type();

  // Check if a new reading from the IR receiver was completed
  if (type == Event::READ_COMPLETED_TYPE) {
    receiver.print(trace);
    uint16_t code = event.get_value();
    trace.printf_P(PSTR("code = %hd\n"), code);
  } 
  receiver.reset();
}
