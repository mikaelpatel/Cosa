/**
 * @file Cosa/Driver/NEXA.hh
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

#ifndef COSA_DRIVER_NEXA_HH
#define COSA_DRIVER_NEXA_HH

#include "Cosa/Types.h"
#include "Cosa/OutputPin.hh"
#include "Cosa/Event.hh"
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Listener.hh"

/**
 * NEXA Wireless Lighting Control receiver and transmitter.
 */
class NEXA {
public:
  /**
   * Wireless command code; 32-bit, little endian order for AVR 
   */
  union code_t {
    int32_t as_long;
    struct {
      uint8_t device:4;		/** Device number, group:unit<2,2>. */
      uint8_t onoff:1;		/** Device mode, off(0), on(1). */
      uint8_t group:1;		/** Group command. */
      uint32_t house:26;	/** House code number. */
    };

    /**
     * Construct command code from given 32-bit number. Default constructor.
     * @param[in] value to construct as command code.
     */
    code_t(int32_t value = 0L) 
    { 
      as_long = value; 
    }

    /**
     * Construct unit address from given house and device numbers.
     * @param[in] h house.
     * @param[in] d device.
     */
    code_t(int32_t h, uint8_t d)
    { 
      device = d;
      onoff = 0;
      group = 0;
      house = h;
    }

    /**
     * Construct command code from given house, group, device and 
     * onoff flag.
     * @param[in] h house.
     * @param[in] g group.
     * @param[in] d device.
     * @param[in] f onoff.
     */
    code_t(int32_t h, uint8_t g, uint8_t d, uint8_t f)
    { 
      device = d;
      onoff = f;
      group = g;
      house = h;
    }

    /**
     * Cast to code to long value.
     * @return value.
     */
    operator int32_t()
    {
      return (as_long);
    }

    /**
     * Compare code with other received code. If group command
     * then compare channels (0..3) else compare house and device
     * numbers.
     * @param[in] other code.
     * @return bool.
     */
    bool operator==(const NEXA::code_t &other) const 
    {
      if (other.group) 
	return ((device & 0b1100) == (other.device & 0b1100));
      return ((house == other.house) && (device == other.device));
    }

    /**
     * Print command code fields to given output stream.
     * @param[in] outs output stream.
     * @param[in] code to print.
     * @return iostream.
     */
    friend IOStream& operator<<(IOStream& outs, code_t code);
  };

  /**
   * NEXA Wireless Remote Receiver. May be used in polling or
   * interrupt sampling mode. 
   *
   * @section Circuit
   * @code
   *                         RF433/RX                       V
   *                       +------------+                   |
   *                       |0-----------|-------------------+
   *                       |ANT         |       17.3 cm
   *                       |            |
   *                       |            |
   *                       |            |
   *                       |            |
   * (VCC)---------------1-|VCC         |
   *                     2-|DATA        |
   * (EXTn)--------------3-|DATA        |
   * (GND)---------------4-|GND         |
   *                       +------------+
   * @endcode
   */
  class Receiver : private ExternalInterrupt, public Event::Handler {
  public:
    /**
     * NEXA::Receiver::Listener with code as key. The virtual method
     * on_event() is called when a command code matches the listeners 
     * key. The listener will receive Event::CHANGE_TYPE and the value 
     * of the onoff member of the received command code.
     */
    typedef Listener<code_t> Device;

    /**
     * Create a NEXA::Receiver connected to the given external
     * interrupt pin. The interrupt handler must be enabled to become
     * active. Use enable() in setup().
     * @param[in] pin external interrupt pin.
     */
    Receiver(Board::ExternalInterruptPin pin) :
      ExternalInterrupt(pin, ExternalInterrupt::ON_CHANGE_MODE),
      m_listeners(),
      m_start(0),
      m_code(0),
      m_ix(0)
    {}

    /** 
     * Attach given device to list of listeners.
     * @param[in] device to attach.
     */
    void attach(Device* device) __attribute__((always_inline))
    {
      m_listeners.attach(device);
    }
    
    /**
     * Retrieve decoded command after Event::RECEIVE_COMPLETED_TYPE from
     * interrupt handler.
     * @return decoded command.
     */
    code_t get_code() const
    { 
      return (m_code); 
    }

    /**
     * Poll wireless receiver for incoming command. Will busy-wait on
     * signal change. Returns decoded command.
     * @param[out] cmd received decoded command.
     */
    void recv(code_t& cmd);

    /**
     * Enable interrupt driven command code receiving. Interrupt
     * handler will push an Event::RECEIVE_COMPLETED_TYPE when a
     * command has been received. 
     */
    void enable() __attribute__((always_inline))
    { 
      ExternalInterrupt::enable();
    }

    /**
     * Disable interrupt driven command code receiving.
     */
    void disable() __attribute__((always_inline))
    { 
      ExternalInterrupt::disable();
    }

  private:
    Head m_listeners;
    static const uint8_t SAMPLE_MAX = 4;
    static const uint8_t IX_MAX = 129;    
    static const uint8_t IX_MASK = SAMPLE_MAX - 1;
    static const uint16_t LOW_THRESHOLD = 200;
    static const uint16_t BIT_THRESHOLD = 500;
    static const uint16_t HIGH_THRESHOLD = 1500;
    volatile uint16_t m_sample[SAMPLE_MAX];
    volatile uint32_t m_start;
    volatile uint32_t m_code;
    volatile uint8_t m_ix;

    /**
     * @override Interrupt::Handler
     * Measures the pulse with and decodes the pulse stream. Will push
     * an Event::RECEIVE_COMPLETED_TYPE when completed decoding. Commands
     * should be retrieved with get_code(). The event will contain the
     * class instance as target. This allows sub-classes to override
     * the Event::Handler::on_event() method and use event dispatch.
     * Alternatively sub-class Listener with on_change() and call 
     * dispatch() after receiving the event.  
     * @param[in] arg argument from first level interrupt handler.
     */
    virtual void on_interrupt(uint16_t arg = 0);

    /**
     * @override Event::Handler
     * Handle events from interrupt handler; dispatch to listeners.
     * The incoming event should be Event::RECEIVE_COMPLETED_TYPE.
     * The event handler, on_event(), of each listener that matches
     * the address of the received command code will be called with 
     * Event::CHANGE_TYPE and the command onoff value. 
     * @param[in] type the event type.
     * @param[in] value the event value.
     */
    virtual void on_event(uint8_t type, uint16_t value)
    {
      UNUSED(type);
      UNUSED(value);
      code_t cmd(m_code);
      Device::dispatch(&m_listeners, cmd, Event::CHANGE_TYPE, cmd.onoff);
    }

    /**
     * Decode the current four samples. Return bit, zero(0) or one(1),
     * if successful otherwise negative error code(-1).
     * @return decoded bit(0/1) or negative error code.
     */
    int8_t decode_bit();
  };

  /**
   * NEXA Wireless Command Code Transmitter. Sends command codes to 
   * NEXA lighting control equipment or NEXA::Receiver. Delay based 
   * implementation; transmission will return when completed.
   *
   * @section Circuit
   * @code
   *                         RF433/TX
   *                       +------------+
   * (Dn)----------------1-|DATA        |
   * (VCC)---------------2-|VCC         |                    V
   * (GND)---------------3-|GND         |                    |
   *                       |ANT       0-|--------------------+
   *                       +------------+       17.3 cm
   * @endcode
   */
  class Transmitter : private OutputPin {
  public:
    /**
     * Construct NEXA Command Code Transmitter connected to RF433
     * Transmitter connected to given pin. The default house code 
     * is zero(0).
     * @param[in] pin output pin for transmitter.
     * @param[in] nr house number.
     */
    Transmitter(Board::DigitalPin pin, uint32_t nr = 0L) : 
      OutputPin(pin),
      m_house(nr)
    {
    }

    /**
     * Set house code to given number. 
     * @param[in] nr house number.
     */
    void set_house(uint32_t nr)
    {
      m_house = nr;
    }

    /**
     * Send command code to given device (0..15). Turn device on or 
     * off according to parameter. Device number is channel:unit<2:2>, 
     * channel(0..3), unit(0..3). Dimmer levels are onoff values -1..-15.
     * @param[in] device (0..15).
     * @param[in] onoff device mode (-15..-1..0..1).
     */
    void send(uint8_t device, int8_t onoff) __attribute__((always_inline))
    {
      code_t cmd(m_house, 0, device, onoff);
      send_code(cmd, onoff);
    }
    
    /**
     * Send command code to given group. Turn devices in group on 
     * or off according to parameter. Group number is (0..3).
     * @param[in] group (0..3).
     * @param[in] onoff device mode (0..1).
     */
    void broadcast(uint8_t group, int8_t onoff) __attribute__((always_inline))
    {
      code_t cmd(m_house, 1, group << 2, onoff);
      send_code(cmd, onoff != 0);
    }

  private:
    /** Number of code transmissions. */
    static const uint8_t SEND_CODE_MAX = 4;

    /** Pause between code transmissions (milli-second delay). */
    static const uint32_t PAUSE = 10L;

    /** Transmission pulse timing (micro-second delay). */
    static const uint16_t SHORT_PULSE = 275;
    static const uint16_t LONG_PULSE = 1225;
    static const uint16_t START_PULSE = 2675 - SHORT_PULSE;

    /** Transmission house address: 26 bits. */
    uint32_t m_house;

    /**
     * Send a pulse followed by short delay for zero(0) and long for 
     * non-zero(1).
     * @param[in] value (0..1).
     */
    void send_pulse(uint8_t value) __attribute__((always_inline))
    {
      set();
      DELAY(SHORT_PULSE);
      clear();
      DELAY(value ? LONG_PULSE : SHORT_PULSE);
    }

    /**
     * Send a single bit as Manchester code (0 -> 01, 1 -> 10).
     * @param[in] value (0..1).
     */
    void send_bit(uint8_t value) __attribute__((always_inline))
    {
      send_pulse(value);
      send_pulse(!value);
    }

    /**
     * Send a command code. Transmitted SEND_CODE_MAX times with
     * pause between each transmission. Dimmer levels are onoff 
     * values -1..-15.
     * @param[in] cmd to transmit.
     * @param[in] onoff device mode (-15..-1..0..1).
     */
    void send_code(code_t cmd, int8_t onoff);
  };
};

#endif
