/**
 * @file CosaClockCalibration.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * Calibrate RTC and Watchdog clocks with external clock source on
 * external interrupt pin (EXT1/D3). The source can for instance be
 * square wave from an external I2C RTC module such as DS1307.
 *
 * @section Calibration
 * 1. Run without calibration settings.
 * 2. Run for at least 10-20 minutes (600-1200 seconds) alternatively
 *    connect a Logic Analyzer and probe D7(Clock), D8(RTC),
 *    D9(Watchdog).
 * 3. Use the integer part of the error as calibration value or
 *    collect the period time error from the Logic Analyzer.
 * 4. Rerun and verify that the error level reduced.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OutputPin.hh"
#include "Cosa/Alarm.hh"
#include "Cosa/Clock.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Example calibration values for internal clocks
// 1. Duemilanove Clone (Funduino 2012 May)
// #define RTC_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -12
// 2. Leonardo (Made in Italy)
// #define RTC_CALIBRATION_MS -2
// #define WATCHDOG_CALIBRATION_MS -98
// 3. Mega 2560 Clone
// #define RTC_CALIBRATION_MS -1
// #define WATCHDOG_CALIBRATION_MS -108
// 4. Mega 2560 Clone (Funduino)
// #define RTC_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -110
// 5. Nano Clone (DCCduino)
// #define RTC_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -24
// 6. Pro-Mini Clone (white reset button)
// #define RTC_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS 26
// 7. Pro-Mini Clone (Deek Robot, red reset button)
// #define RTC_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -1
// 8. Uno R3 Clone (GEtech)
// #define RTC_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -112
// 9. Uno R3 Clone (VISduino)
// #define RTC_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -24
// 10. Anarduino Miniwireless
#define RTC_CALIBRATION_MS 0
#define WATCHDOG_CALIBRATION_MS -66
// 11. Your Arduino
// #define RTC_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS 0

// Wall-clocks
Alarm::Clock clock(Board::EXT1);
RTC::Clock wall;
Watchdog::Clock bark;

// Pulse generator class
class Pulse : public Periodic {
public:
  Pulse(Job::Scheduler* scheduler, uint32_t period, Board::DigitalPin pin) :
    Periodic(scheduler, period),
    m_pin(pin)
  {}

  virtual void on_expired()
  {
    m_pin.toggle();
    expire_after(period());
    start();
  }

private:
  OutputPin m_pin;
};

// Pulse generators
Pulse red(&clock, 1, Board::D7);
Pulse green(&wall, 1, Board::D8);
Pulse blue(&bark, 1, Board::D9);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaClockCalibration: started"));

  // Print calibration
#if defined(RTC_CALIBRATION_MS)
  trace << PSTR("RTC:calibration: ") << RTC_CALIBRATION_MS << endl;
#endif
#if defined(WATCHDOG_CALIBRATION_MS)
  trace << PSTR("Watchdog:calibration: ") << WATCHDOG_CALIBRATION_MS << endl;
#endif

  // Start the watchdog and internal real-time clock
  Watchdog::begin();
  RTC::begin();

  // Set calibration (from error measurement)
#if defined(RTC_CALIBRATION_MS)
  wall.calibration(RTC_CALIBRATION_MS);
#endif
#if defined(WATCHDOG_CALIBRATION_MS)
  bark.calibration(WATCHDOG_CALIBRATION_MS);
#endif

  // Start the clock and adjust start time
  clock.enable();
  clock.time(0);
  wall.time(1);
  bark.time(1);

  // Start the probe outputs
  red.start();
  green.start();
  blue.start();
}

void loop()
{
  // Count loop cycles
  static int32_t cycle = 1;

  // Wait for clock tick
  clock_t start = clock.time();
  clock_t now;
  while (1) {
    now = clock.time();
    if (now != start) break;
    yield();
  }

  // Calculate the clock drift in seconds
  int32_t rtc = wall.time() - now;
  int32_t wdg = bark.time() - now;

  // Print the clocks, drift and error rate (milli-seconds per second)
  trace << cycle << ':' << now
	<< PSTR(":RTC:")
	<< (rtc < 0 ? PSTR("T") : PSTR("T+")) << rtc
	<< PSTR(",err=") << (1000.0 * rtc) / cycle
	<< PSTR(":Watchdog:")
	<< (wdg < 0 ? PSTR("T") : PSTR("T+")) << wdg
	<< PSTR(",err=") << (1000.0 * wdg) / cycle
	<< endl;
  cycle += 1;
}
