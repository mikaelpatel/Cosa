/**
 * @file Cosa/Keypad.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
#include "Cosa/Linkage.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/Watchdog.hh"

/**
 * Handling of keypad using resistor net and analog reading. Periodically
 * samples the analog pin and maps to key code. Callback on_key() is called
 * when a key down/up is detected.
 */
class Keypad : private Link {
public:
  /**
   * Construct keypad handler with given analog pin and mapping.
   * The mapping should be a decending sequence of thresholds and
   * should be stored in program memory.
   * @param[in] pin analog pin.
   * @param[in] map between analog value and key.
   */
  Keypad(Board::AnalogPin pin, const uint16_t* map) :
    Link(),
    m_key(pin, this, map)
  {
  }

  /**
   * Start the keypad handler.
   */
  void begin()
  {
    Watchdog::attach(this, SAMPLE_MS);
  }

  /**
   * Stop the keypad handler
   */
  void end()
  {
    detach();
  }

  /**
   * @override Keypad
   * Callback method when a key down is detected. Must override.
   * @param[in] nr key number (index in map).
   */
  virtual void on_key_down(uint8_t nr) 
  {
    UNUSED(nr);
  }

  /**
   * @override Keypad
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
  private:
    Keypad* m_keypad;
    const uint16_t* m_map;
    uint8_t m_latest;

    /**
     * @override AnalogPin
     * Callback member function when the analog pin value changes.
     * @param[in] value new reading.
     */
    virtual void on_change(uint16_t value);

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
    {
    }
  };

  // Keypad sample rate
  static const uint16_t SAMPLE_MS = 64;
  
  // The key sampler and mapper
  Key m_key;

  /**
   * @override Event::Handler
   * Periodic sampling of analog pin.
   * @param[in] type the event type.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);
};

/**
 * LCD Keypad shield, keypad handler. The class represents the
 * necessary configuration; keypad sensor on analog pin A0 and 
 * mapping vector.
 */
class LCDKeypad : public Keypad {
public:
  // Key index
  enum {
    NO_KEY = 0,
    SELECT_KEY,
    LEFT_KEY,
    DOWN_KEY,
    UP_KEY,
    RIGHT_KEY
  } __attribute__((packed));

  // LCD Keypad constructor with internal key map
  LCDKeypad() : 
    Keypad(Board::A0, m_map) 
  {
  }

private:
  // Analog reading to key index map
  static const uint16_t m_map[] PROGMEM;
};

#endif
