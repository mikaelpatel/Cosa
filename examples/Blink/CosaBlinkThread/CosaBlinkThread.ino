/**
 * @file CosaBlinkThread.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
 * Demonstration of Cosa Nucleo Threads; Blink LED with thread
 * delay and use an additional thread to control the LED blink
 * frequency. The main thread will power down while waiting for
 * Watchdog ticks (ATtiny, LED on 2 mA, LED off <15 uA @ 3.7V).
 *
 * @section Circuit
 * No special circuit. Uses the built-in LED (Arduino/D13).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Nucleo.h>

#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"

// This class is a thread that turns a LED on and off given a
// delay period. The delay may be changed dynamically.
class LED : public Nucleo::Thread {
public:

  // This class is a thread that periodically changes a LED delay
  // from low to high with a given increment
  template <uint16_t LOW, uint16_t HIGH, uint16_t INC, uint16_t PERIOD>
  class Controller : public Nucleo::Thread {
  public:
    Controller(LED* led) : Thread(), m_led(led) {}

    virtual void run()
    {
      for (uint16_t ms = LOW; ms < HIGH; ms += INC) {
	m_led->set_delay(ms);
	delay(PERIOD);
      }
      for (uint16_t ms = HIGH; ms > LOW; ms -= INC) {
	m_led->set_delay(ms);
	delay(PERIOD);
      }
    }

  private:
    LED* m_led;
  };

  LED(Board::DigitalPin pin) : Thread(), m_pin(pin, 0), m_delay(200) {}

  void set_delay(uint16_t ms)
  {
    m_delay = ms;
  }

  virtual void run()
  {
    m_pin.toggle();
    delay(10);
    m_pin.toggle();
    delay(m_delay);
  }

private:
  OutputPin m_pin;
  uint16_t m_delay;
};

// The instances of the LED and the Controller
LED buildin(Board::LED);
LED::Controller<25,500,5,200> controller(&buildin);

void setup()
{
  // Use power down idle mode for low power
  Power::set(SLEEP_MODE_PWR_DOWN);
  Watchdog::begin();

  // Allocate the threads with given stack size
  Nucleo::Thread::begin(&buildin, 64);
  Nucleo::Thread::begin(&controller, 64);

  // Start the threads
  Nucleo::Thread::begin();
}

void loop()
{
 // Service the nucleos
  Nucleo::Thread::service();
}
