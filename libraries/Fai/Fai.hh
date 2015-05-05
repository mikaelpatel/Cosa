/**
 * @file Fai.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#ifndef COSA_FAI_HH
#define COSA_FAI_HH

#include "Cosa/Types.h"
#include "Cosa/Pin.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/Event.hh"

#include <Ciao.h>

/**
 * The Cosa Ciao data stream of Arduino status and commands (Cosa fai).
 */
class Fai : public Ciao {
public:
  /**
   * Cosa fai: data type descriptors and prototypes.
   */
  class Descriptor {
  public:
    enum {
      ANALOG_PIN_ID = Ciao::Descriptor::COSA_FAI_ID,
      DIGITAL_PIN_ID,
      DIGITAL_PINS_ID,
      EVENT_ID,
      SAMPLE_REQUEST_ID,
      SET_MODE_ID
    };
    static const Ciao::Descriptor::user_t analog_pin_t PROGMEM;
    static const Ciao::Descriptor::user_t digital_pin_t PROGMEM;
    static const Ciao::Descriptor::user_t digital_pins_t PROGMEM;
    static const Ciao::Descriptor::user_t event_t PROGMEM;
    static const Ciao::Descriptor::user_t sample_request_t PROGMEM;
    static const Ciao::Descriptor::user_t set_mode_t PROGMEM;
  };

  /**
   * Stream analog pin value. The identity code is ANALOG_PIN_ID(0x10).
   */
  struct analog_pin_t {
    uint8_t pin;
    uint16_t value;
  };

  /**
   * Stream digital pin value. The identity code is DIGITAL_PIN_ID(0x11).
   */
  struct digital_pin_t {
    uint8_t pin;
    uint8_t value;
  };

  /**
   * Stream digital pins value. The identity code is DIGITAL_PINS_ID(0x12).
   */
  struct digital_pins_t {
    uint32_t values;
  };

  /**
   * Stream sample request. The identity code is SAMPLE_REQUEST_ID(0x20).
   */
  struct sample_request_t {
    uint32_t pins;
    uint16_t period;
  };

  /**
   * Stream set mode request. The identity code is SET_MODE_ID(0x21).
   */
  struct set_mode_t {
    uint8_t pin;
    uint8_t mode;
  };

  /**
   * Construct data streaming for given device.
   * @param[in] dev output device.
   */
  Fai(IOStream::Device* dev = NULL) : Ciao(dev) {}

  /**
   * Start the data stream with a version string and endian information.
   */
  void begin();

  /**
   * Write digital pins value to data stream.
   * @param[in] mask digital pins to write to data stream.
   */
  void write(uint32_t mask);

  /**
   * Write digital pin value to data stream.
   * @param[in] pin to write to data stream.
   */
  void write(Pin* pin);

  /**
   * Write analog pin value to data stream.
   * @param[in] pin to write to data stream.
   */
  void write(AnalogPin* pin);

  /**
   * Write event to data stream.
   * @param[in] event to write to data stream.
   */
  void write(Event* event)
  {
    Ciao::write(&Descriptor::event_t, event, 1);
  }
};

#endif
