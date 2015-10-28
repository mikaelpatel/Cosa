/**
 * @file CosaBenchmarkUART.ino
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
 * @section Description
 * Benchmarking IOStream and UART functions; measure time to print
 * characters, strings and numbers through the IOStream interface and
 * IOBuffer to the UART.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include <math.h>

// This benchmark can be run with a background interrupt load. Set the
// below symbol to enable a periodic job with the given period in
// micro-seconds.
// #define BACKGROUND_PULSE 150
#define PULSE_WIDTH 50

#if defined(BACKGROUND_PULSE)
#include "Cosa/OutputPin.hh"
#include "Cosa/Periodic.hh"

// Schedule from interrupt with time or fixed period
#define USE_ISR_TIME_PERIOD
// #define USE_ISR_RESCHEDULE

class Pulse : public Periodic {
public:
  Pulse(Job::Scheduler* scheduler, uint32_t delay, Board::DigitalPin pin) :
    Periodic(scheduler, delay),
    m_pin(pin)
  {
    expire_at(100000UL);
  }

#if defined(USE_ISR_TIME_PERIOD)
  virtual void on_expired()
  {
    run();
    expire_at(time() + period());
    start();
  }
#elif defined(USE_ISR_RESCHEDULE)
  virtual void on_expired()
  {
    run();
    reschedule();
  }
#else
#error "on_expired: not defined"
#endif

  virtual void run()
  {
    m_pin.toggle();
#if defined(PULSE_WIDTH)
    DELAY(PULSE_WIDTH);
#endif
    m_pin.toggle();
  }

private:
  OutputPin m_pin;
};

// The RTT job scheduler and background pulse generator
RTT::Scheduler scheduler;
Pulse background(&scheduler, BACKGROUND_PULSE, Board::D7);
#endif

void setup()
{
  // Start serial output with given baud-rate
  uart.begin(1000000);
  // uart.begin(2000000);
  // uart.begin(1000000);
  // uart.begin(500000);
  // uart.begin(250000);
  // uart.begin(230400);
  // uart.begin(115200);
  // uart.begin(57600);
  // uart.begin(38400);
  // uart.begin(28800);
  // uart.begin(19200);
  // uart.begin(14400);
  // uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkUART: started"));

#if defined(BACKGROUND_PULSE)
  ASSERT(background.start());
  trace << PSTR("Background:") << BACKGROUND_PULSE << PSTR(" us") << endl;
#endif

  // Start timers
  Watchdog::begin();
  RTT::begin();
}

void loop()
{
  // Measure time to print character, string and number
  MEASURE("one character (new-line):", 1) trace << '\n' << endl;
  MEASURE("one character:", 1) trace << '1' << endl;
  MEASURE("one character string:", 1) trace << PSTR("1") << endl;
  MEASURE("utf8 character string:", 1) trace << PSTR("€") << endl;

  MEASURE("integer:", 1) trace << 1 << endl;
  MEASURE("long integer:", 1) trace << 1L << endl;
  MEASURE("two characters:", 1) trace << '1' << '0' << endl;
  MEASURE("two character string:", 1) trace << PSTR("10") << endl;
  MEASURE("integer:", 1) trace << 10 << endl;
  MEASURE("long integer:", 1) trace << 10L << endl;
  MEASURE("floating point:", 1) trace << 10.0 << endl;
  MEASURE("three characters:", 1) trace << '1' << '0' << '0' << endl;
  MEASURE("three character string:", 1) trace << PSTR("100") << endl;
  MEASURE("integer:", 1) trace << 100 << endl;
  MEASURE("long integer:", 1) trace << 100L << endl;
  MEASURE("floating point:", 1) trace << 100.0 << endl;

  // Measure time to print min/max integer base(10)
  MEASURE("zero int16_t:", 1) trace << 0 << endl;
  MEASURE("min int16_t:", 1) trace << INT16_MIN << endl;
  MEASURE("max int16_t:", 1) trace << INT16_MAX << endl;
  MEASURE("max uint16_t:", 1) trace << UINT16_MAX << endl;
  MEASURE("zero int32_t:", 1) trace << 0L << endl;
  MEASURE("min int32_t:", 1) trace << INT32_MIN << endl;
  MEASURE("max int32_t:", 1) trace << INT32_MAX << endl;
  MEASURE("max uint32_t:", 1) trace << UINT32_MAX << endl;

  // Measure time to print min/max integer base(2)
  MEASURE("zero int16_t:", 1) trace << bin << 0 << endl;
  MEASURE("min int16_t:", 1) trace << bin << INT16_MIN << endl;
  MEASURE("max int16_t:", 1) trace << bin << INT16_MAX << endl;
  MEASURE("max uint16_t:", 1) trace << bin << UINT16_MAX << endl;
  MEASURE("zero int32_t:", 1) trace << bin << 0L << endl;
  MEASURE("min int32_t:", 1) trace << bin << INT32_MIN << endl;
  MEASURE("max int32_t:", 1) trace << bin << INT32_MAX << endl;
  MEASURE("max uint32_t:", 1) trace << bin << UINT32_MAX << endl;

  // Measure time to print min/max integer base(8)
  MEASURE("zero int16_t:", 1) trace << oct << 0 << endl;
  MEASURE("min int16_t:", 1) trace << oct << INT16_MIN << endl;
  MEASURE("max int16_t:", 1) trace << oct << INT16_MAX << endl;
  MEASURE("max uint16_t:", 1) trace << oct << UINT16_MAX << endl;
  MEASURE("zero int32_t:", 1) trace << oct << 0L << endl;
  MEASURE("min int32_t:", 1) trace << oct << INT32_MIN << endl;
  MEASURE("max int32_t:", 1) trace << oct << INT32_MAX << endl;
  MEASURE("max uint32_t:", 1) trace << oct << UINT32_MAX << endl;

  // Measure time to print min/max integer base(16)
  MEASURE("zero int16_t:", 1) trace << hex << 0 << endl;
  MEASURE("min int16_t:", 1) trace << hex << INT16_MIN << endl;
  MEASURE("max int16_t:", 1) trace << hex << INT16_MAX << endl;
  MEASURE("max uint16_t:", 1) trace << hex << UINT16_MAX << endl;
  MEASURE("zero int32_t:", 1) trace << hex << 0L << endl;
  MEASURE("min int32_t:", 1) trace << hex << INT32_MIN << endl;
  MEASURE("max int32_t:", 1) trace << hex << INT32_MAX << endl;
  MEASURE("max uint32_t:", 1) trace << hex << UINT32_MAX << endl;

  // Measure time to print some standar floating point numbers
  MEASURE("floating point (zero):", 1) trace << 0.0 << endl;
  MEASURE("floating point (nan):", 1) trace << NAN << endl;
  MEASURE("floating point (infinity):", 1) trace << INFINITY << endl;
  MEASURE("floating point (pi):", 1) trace << M_PI << endl;
  MEASURE("floating point (e):", 1) trace << M_E << endl;
  MEASURE("floating point (log2e):", 1) trace << M_LOG2E << endl;
  MEASURE("floating point (log10e):", 1) trace << M_LOG10E << endl;
  MEASURE("floating point (logn2):", 1) trace << M_LN2 << endl;
  MEASURE("floating point (logn10):", 1) trace << M_LN10 << endl;

  // Measure time to print unsigned integer; 16 and 32 bit.
  MEASURE("uint16_t(digits=6):", 1) {
    trace.print(100U, 6, IOStream::dec);
    trace.println();
  }
  MEASURE("uint32_t(digits=6):", 1) {
    trace.print(100UL, 6, IOStream::dec);
    trace.println();
  }

  // Measure time to print some special characters
  MEASURE("newline character:", 1) trace << '\n';
  MEASURE("tab:", 1) trace << '\t' << endl;
  MEASURE("newline string(1):", 1) trace << (char*) "\n";
  MEASURE("newline string(2):", 1) trace << (char*) "\n\n";

  // Measure time to print all characters
  str_P s = PSTR(" !\"#$%&'()*+,-./0123456789:;<=>?@"
		 "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
		 "abcdefghijklmnopqrstuvwxyz{|}~\r\n");
  const uint8_t LINES = 10;

  // Use 10 ms delay before and after as markers for Logic Analyzer
  delay(10);
  MEASURE("performance:", 1) {
    for (uint8_t i = 0; i < LINES; i++) trace << s;
    trace.flush();
  }
  delay(10);
  uint32_t BITS = 11;
  uint16_t chars = LINES * strlen_P(s);
  uint32_t Kbps = (BITS * chars * 1000UL) / trace.measure;
  trace << PSTR("effective baudrate (") << chars << PSTR(" characters):")
	<< Kbps << PSTR(" Kbps")
	<< endl;

#if defined(BACKGROUND_PULSE)
  uint16_t pulses = trace.measure / BACKGROUND_PULSE;
  trace << PSTR("background pulses (") << pulses << PSTR("):")
	<< (uint32_t) pulses * PULSE_WIDTH << PSTR(" us")
	<< endl;
#endif
  sleep(1);
}
