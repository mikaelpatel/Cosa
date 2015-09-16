/**
 * @file Cosa/Keypad.hh
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_KEYPAD_HH
#define COSA_KEYPAD_HH

#include "Cosa/Types.h"
#include "Cosa/AnalogPin.hh"
#include "Cosa/Periodic.hh"

/**
 * Handling of keypad using resistor net and analog reading. Periodically
 * samples the analog pin and maps to key code. Callback on_key() is called
 * when a key down/up is detected.
 */
class Keypad : public Periodic {
public:
  /**
   * Construct keypad handler with given analog pin and mapping.
   * The mapping should be a decending sequence of thresholds and
   * should be stored in program memory.
   * @param[in] scheduler periodic job handler.
   * @param[in] pin analog pin.
   * @param[in] map between analog value and key.
   */
  Keypad(Job::Scheduler* scheduler, Board::AnalogPin pin, const uint16_t* map) :
    Periodic(scheduler, SAMPLE_MS),
    m_key(pin, this, map)
  {}

  /**
   * @override{Keypad}
   * Callback method when a key down is detected. Must override.
   * @param[in] nr key number (index in map).
   */
  virtual void on_key_down(uint8_t nr)
  {
    UNUSED(nr);
  }

  /**
   * @override{Keypad}
   * Callback method when a key up is detected. Default is null
   * function.
   * @param[in] nr key number (index in map).
   */
  virtual void on_key_up(uint8_t nr)
  {
    UNUSED(nr);
  }

protected:
  /**
   * Internal analog pin sampler to detect key down. Samples are maps
   * to key code.
   */
  class Key : public AnalogPin {
  public:
    /**
     * Construct sampler of analog pin with given zero terminated map.
     * @param[in] pin analog pin.
     * @param[in] keypad to callback on key down detect.
     * @param[in] map analog to key map.
     */
    Key(Board::AnalogPin pin, Keypad* keypad, const uint16_t* map) :
      AnalogPin(pin),
      m_keypad(keypad),
      m_map(map),
      m_latest(0)
    {}

  private:
    Keypad* m_keypad;
    const uint16_t* m_map;
    uint8_t m_latest;

    /**
     * @override{AnalogPin}
     * Callback member function when the analog pin value changes.
     * @param[in] value new reading.
     */
    virtual void on_change(uint16_t value);
  };

  /** Keypad sample rate. */
  static const uint16_t SAMPLE_MS = 64;

  /** The key sampler and mapper. */
  Key m_key;

  /**
   * @override{Job}
   * Periodic sampling of analog pin.
   */
  virtual void run();
};

#endif
