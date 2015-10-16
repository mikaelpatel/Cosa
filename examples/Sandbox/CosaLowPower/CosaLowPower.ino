/**
 * @file CosaLowPower.ino
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
 * Low power mode testing.
 *
 * Arduino Mini Pro, approx. 3 mA idle, power LED on.
 * 1. Modifications; removed power LED resistor, 1 Kohm.
 * 2. Powered via FTDI USB/TTY adapter 5 V
 *    a. Connected to VCC/GND, 180 uA
 *    b. Connected to RAW/GND, 840 uA
 *    c. Connected to VCC/GND, + BOD disabled, 160 uA (See Power.hh)
 *    4. Connected to VCC/GND, + disable pins, 157 uA
 * 3. Powered with LiPo 3,7 V
 *    a. Connected to VCC/GND, 32 uA
 *    b. Connected to RAW/GND, 840 uA
 *
 * Arduino Lilypad, 8 Mhz, no modifications
 * 1. Powered via FTDI USB/TTY adapter 5 V, 18 uA
 * 2. Powered with LiPo 3,7 V, 16 uA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Event.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Power.hh"
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/Watchdog.hh"

#define USE_DISABLE_PINS
#define USE_EVENT_AWAIT
// #define USE_WATCHDOG_DELAY
#define USE_WATCHDOG_SHUTDOWN

OutputPin led(Board::LED);

class Button : public ExternalInterrupt {
  OutputPin* m_led;
public:
  Button(Board::ExternalInterruptPin pin, OutputPin* led) :
    ExternalInterrupt(pin, ExternalInterrupt::ON_LOW_LEVEL_MODE, true),
    m_led(led)
  {}

  virtual void on_interrupt(uint16_t arg = 0)
  {
    UNUSED(arg);
    if (m_led != NULL) m_led->on();
#ifdef USE_EVENT_AWAIT
    Event::push(Event::NULL_TYPE, NULL);
#endif
    disable();
  }
};

Button wakeup(Board::EXT0, &led);

void setup()
{
  Power::set(SLEEP_MODE_PWR_DOWN);
#if defined(USE_WATCHDOG_DELAY)
  Watchdog::begin();
#endif
#if defined(USE_DISABLE_PINS)
  // 2 uA, possible uart pin needed disconnecting
  // DDRB  = 0b11111111;
  // PORTB = 0b00000000;
  // DDRC  = 0b11111111;
  // PORTC = 0b00000000;
  // DDRD  = 0b11111011;
  // PORTD = 0b00000100;
  PORTB = 0b00000000;
  DDRB =  0b00100000;
  PORTC = 0b00000000;
  DDRC =  0b00000000;
  PORTD = 0b00000100;
  DDRD =  0b00000000;
#endif

  wakeup.enable();
}

void loop()
{
  // 1. Show that event wait draws as much as sleep
#if defined(USE_EVENT_AWAIT)
  // 180 uA - (BOD + PIN disable = 23 uA)
  Event event;
  Event::queue.await(&event);
#else
  // 180 uA - (BOD + PIN disable = 23 uA)
  Power::sleep();
#endif

  // 2. a) Show that watchdog delay during blink allows
  // low power sleep, b) shutdown of watchdog may save
  // some extra (7 uA)
#if defined(USE_WATCHDOG_SHUTDOWN)
  // 1,5 mA
  Watchdog::begin();
  delay = Watchdog::delay;
  while (wakeup.is_low()) {
    led.toggle();
    Watchdog::delay(64);
  }
  led.off();
  Watchdog::end();
#elif defined(USE_WATCHDOG_DELAY)
  // 1,5 mA
  while (wakeup.is_low()) {
    led.toggle();
    Watchdog::delay(64);
  }
  led.off();
#else
  // 15 mA
  while (wakeup.is_low()) {
    led.toggle();
    delay(64);
  }
  led.off();
#endif

  wakeup.enable();
}
