/**
 * @file CosaLowPower.ino
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

#define USE_DISABLE_MODULES
#define USE_DISABLE_PINS
#define USE_EVENT_AWAIT
#define USE_WATCHDOG_DELAY

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
#if defined(USE_DISABLE_MODULES)
  // 0 uA, already done by startup
  ACSR = _BV(ACD);
  ADCSRA = 0;  
#if !defined(__ARDUINO_STANDARD_USB__)
  UCSR0B = 0;
#endif
#endif
#if defined(USE_DISABLE_PINS)
  // 2 uA, possible uart pin needed disconnecting
  DDRB = 0b11111111;
  PORTB = 0b00000000;
  DDRC = 0b11111111;
  PORTC = 0b00000000;
  DDRD = 0b11111011;
  PORTD = 0b00000100;
#endif
  Power::all_disable();
  wakeup.enable();
}

void loop()
{
#ifdef USE_EVENT_AWAIT
  // 180 uA - (BOD + PIN disable = 23 uA)
  Event event;
  Event::queue.await(&event);
#else
  // 180 uA - (BOD + PIN disable = 23 uA)
  Power::sleep();
#endif

#ifdef USE_WATCHDOG_DELAY
  // 1,5 mA, 64 ms blink
  Watchdog::begin();
  while (wakeup.is_low()) {
    led.toggle();
    Watchdog::delay(64);
  }
  led.off();
  Watchdog::end();
#else
  // 15 mA, 16 ms blink
  while (wakeup.is_low()) {
    led.toggle();
    for (uint8_t i = 0; i < 4; i++) DELAY(16000U);
  }
  led.off();
#endif

  Power::all_disable();
  wakeup.enable();  
}
