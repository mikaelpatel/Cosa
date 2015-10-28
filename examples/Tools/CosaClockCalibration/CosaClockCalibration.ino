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
 * external interrupt pin. The source can for instance be square wave
 * from an external I2C RTC module such as DS1307.
 *
 * @section Calibration
 * 1. Comment out any calibration values.
 * 2. Run the tool and it will generate print of calibration settings.
 * 3. When values are stable stop the run and add the suggested settings.
 * 4. Rerun and verify that the error levels are reduced to max +-1.
 *
 * @section Circuit
 * External RTC connected to EXT1/D3, EXT2:Leonardo/D0 and
 * EXT3:Pro-Micro/D1,Mega 2560/D18.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OutputPin.hh"
#include "Cosa/Alarm.hh"
#include "Cosa/Clock.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

// -------------------------------------------------------------------
// Calibration values for some of the boards that are used in the Cosa
// build and tests. Note that there are variations between individual
// boards due to factory calibration and the quality of the crystal
// (+- 50 ppm) or ceramic resonator (+-.5%) and PCB layout.
// -------------------------------------------------------------------
// #define BOARD_INFO "Anarduino Miniwireless"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -66
// -------------------------------------------------------------------
// #define BOARD_INFO "Duemilanove Clone (Funduino 2012 May)"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -12
// -------------------------------------------------------------------
// #define BOARD_INO "Leonardo (Made in Italy)"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -95
// -------------------------------------------------------------------
// #define BOARD_INO "Lilypad Clone (GEtech)"
// #define RTT_CALIBRATION_MS -1
// #define WATCHDOG_CALIBRATION_MS -140
// -------------------------------------------------------------------
// #define BOARD_INFO "Mega 2560 Clone"
// #define RTT_CALIBRATION_MS -2
// #define WATCHDOG_CALIBRATION_MS -63
// -------------------------------------------------------------------
// #define BOARD_INFO "Mega 2560 Clone (Funduino)"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -108
// -------------------------------------------------------------------
// #define BOARD_INFO "Microduino Core"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -69
// -------------------------------------------------------------------
// #define BOARD_INFO "Microduino Core+"
// #define RTT_CALIBRATION_MS 1
// #define WATCHDOG_CALIBRATION_MS -112
// -------------------------------------------------------------------
// #define BOARD_INFO "Microduino Core32U4"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -8
// -------------------------------------------------------------------
// #define BOARD_INFO "Moteino"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -118
// -------------------------------------------------------------------
// #define BOARD_INFO "Nano Clone (DCCduino)"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -24
// -------------------------------------------------------------------
// #define BOARD_INFO "Pro-Micro Clone"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS 0
// -------------------------------------------------------------------
// #define BOARD_INFO "Pro-Mini Clone (Tinyos, 3.3V/8M)"
// #define RTT_CALIBRATION_MS -2
// #define WATCHDOG_CALIBRATION_MS -61
// -------------------------------------------------------------------
// #define BOARD_INFO "Pro-Mini Clone (white reset button)"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS 26
// -------------------------------------------------------------------
// #define BOARD_INFO "Pro-Mini Clone (Deek Robot, red reset button)"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -1
// -------------------------------------------------------------------
// #define BOARD_INFO "Pro-Mini Clone (Deek Robot, 10 top pins)"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS 0
// -------------------------------------------------------------------
// #define BOARD_INFO "Uno R3 Clone (GEtech)"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -112
// -------------------------------------------------------------------
// #define BOARD_INFO "Uno R3 Clone (VISduino)"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -22
// -------------------------------------------------------------------
// #define BOARD_INFO "Wildfire"
// #define RTT_CALIBRATION_MS 0
// #define WATCHDOG_CALIBRATION_MS -122
// -------------------------------------------------------------------

// Wall-clocks; Square Wave from external RTC, internal RTT and
// Watchdog based seconds level clocks
#if defined(ARDUINO_MEGA)					\
  || defined(ARDUINO_PRO_MICRO)
Alarm::Clock clock(Board::EXT3);
#elif defined(ARDUINO_LEONARDO)	   				\
  || defined(MICRODUINO_CORE_PLUS)				\
  || defined(MICRODUINO_CORE32U4)
Alarm::Clock clock(Board::EXT2);
#else
Alarm::Clock clock(Board::EXT1);
#endif
RTT::Clock wall;
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

// Pulse generators from the different clocks (logic analyzer channel colors)
Pulse black(&clock, 1, Board::LED);
Pulse brown(&wall, 1, Board::D12);
Pulse red(&bark, 1, Board::D11);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaClockCalibration: started"));

  // Print calibration
#if defined(BOARD_INFO)
  trace << PSTR("Board: ") << PSTR(BOARD_INFO) << endl;
#endif
#if defined(RTT_CALIBRATION_MS)
  trace << PSTR("RTT:calibration: ") << RTT_CALIBRATION_MS << endl;
#endif
#if defined(WATCHDOG_CALIBRATION_MS)
  trace << PSTR("Watchdog:calibration: ") << WATCHDOG_CALIBRATION_MS << endl;
#endif

  // Set calibration (from error measurement)
#if defined(RTT_CALIBRATION_MS)
  wall.calibration(RTT_CALIBRATION_MS);
#endif
#if defined(WATCHDOG_CALIBRATION_MS)
  bark.calibration(WATCHDOG_CALIBRATION_MS);
#endif

  // Start the clock and wait for next update
  clock.enable();
  uint32_t now = clock.await();

  // Start internal timers
  Watchdog::begin();
  RTT::begin();

  // Synchronize wall clocks
  delay(500);
  wall.time(now + 1);
  bark.time(now + 1);

  // Start the probe outputs
  black.start();
  brown.start();
  red.start();
}

void loop()
{
  // Count loop cycles
  static int32_t cycle = 1;

  // Wait for external clock update
  uint32_t now = clock.await();

  // Calculate the clock drift in seconds
  int32_t rtc = wall.time() - now;
  int32_t wdg = bark.time() - now;

#if defined(RTT_CALIBRATION_MS) || defined(WATCHDOG_CALIBRATION_MS)
  // Print the clocks, drift and error rate (milli-seconds per second)
  trace << cycle << ':' << now
	<< PSTR(":RTT:")
	<< (rtc < 0 ? PSTR("T") : PSTR("T+")) << rtc
	<< PSTR(",err=") << (1000.0 * rtc) / cycle
	<< PSTR(":Watchdog:")
	<< (wdg < 0 ? PSTR("T") : PSTR("T+")) << wdg
	<< PSTR(",err=") << (1000.0 * wdg) / cycle
	<< endl;
#else
  // Print number of cycles and calibration settings
  if ((cycle % 60) == 0) {
    trace << endl;
    trace << cycle << PSTR(" seconds") << endl;
    trace << PSTR("#define BOARD_INFO \"Board Info\"")
	  << endl;
    trace << PSTR("#define RTT_CALIBRATION_MS ")
	  << (1000 * rtc) / cycle
	  << endl;
    trace << PSTR("#define WATCHDOG_CALIBRATION_MS ")
	  << (1000 * wdg) / cycle
	  << endl;
  }
  else {
    trace << '.';
  }
#endif
  cycle += 1;
}
