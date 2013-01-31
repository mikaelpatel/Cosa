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
 * IR transmission.
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
// #define COLLECT_SAMPLES

class IRreceiver : public InterruptPin {
private:
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
   * been recieved. Further code sequences are rejected until reset()
   * is called.
   */
  virtual void on_interrupt() 
  { 
    // Check if max samples have been recieved. Reject further samples
    if (m_ix == m_max) return;

    // Measure the sample period
    uint32_t stop = RTC::micros();
    uint32_t us = (stop - m_start);
    m_start = stop;

    // And generate the binary code. Skip two first and last samples
    if (m_ix > 1 && m_ix < m_max - 2)
      m_code = (m_code << 1) + (us > m_threshold);

#ifdef COLLECT_SAMPLES
    m_sample[m_ix] = us;
#endif
    
    m_ix += 1;
    // Push event when all samples have been received
    if (m_ix == m_max) {
      if ((m_code >> PRE_DATA_BITS) == PRE_DATA) 
	Event::push(Event::READ_COMPLETED_TYPE, this, m_code);
      else
	reset();
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
    m_threshold(threshold),
    m_start(0),
    m_ix(0),
    m_max(max < SAMPLE_MAX ? max : SAMPLE_MAX),
    m_code(0)
  {}

  /**
   * Print the captured binary code to the given output stream.
   * @param[in] out stream.
   */
  void print(IOStream& out)
  {
#ifdef COLLECT_SAMPLES
    for (uint8_t ix = 0; ix < m_ix; ix++) {
      out.printf_P(PSTR("%d: %ud\n"), ix, m_sample[ix]);
    }
#endif
    out.printf_P(PSTR("code = %hl\n"), m_code);
  }

  /**
   * Reset the IR receive for the next code sequence.
   */
  void reset()
  {
    m_ix = 0;
    m_code = 0;
    m_start = RTC::micros();
  }
};

// IR receiver for an LG is 36 samples and the binary code is (1000..2000).
IRreceiver receiver(Board::EXT0, 36, 1500);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaIRreceiver: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(InterruptPin));
  TRACE(sizeof(IRreceiver));

  // Print pin configuration
  receiver.Pin::print(trace);

  // Use the real-time clock for time measurement
  Watchdog::begin();
  RTC::begin();

  // Enable the interrupt pin to capture the remote code sequence
  receiver.enable();
}

void loop()
{
  // Wait for an event from the IR receiver
  Event event;
  Event::queue.await(&event);

  // Print the received remote code
  receiver.print(trace);

  // Print the posted event code. This is the code that should be used
  uint16_t code = event.get_value();
  INFO("event.value:code = %hd", code);
  Watchdog::delay(512);

  // Reset to allow the next code sequence
  receiver.reset();
}
