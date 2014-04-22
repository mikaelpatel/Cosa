/**
 * @file Cosa/Soft/UART.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_SOFT_UART_HH__
#define __COSA_SOFT_UART_HH__

#include "Cosa/Types.h"
#include "Cosa/OutputPin.hh"
#include "Cosa/PinChangeInterrupt.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/IOBuffer.hh"

namespace Soft {

/**
 * Soft UART for transmission only (UAT) using the OutputPin serial 
 * write method. May be used for trace output from ATtiny devices. 
 * Has a very small footprint and requires only one pin. No timers.
 */
class UAT : public IOStream::Device {
public:
  // Serial formats; DATA + PARITY + STOP
  enum {
    DATA5 = 0,
    DATA6 = 1,
    DATA7 = 2,
    DATA8 = 3,
    DATA9 = 4,
    NO_PARITY = 0,
    EVEN_PARITY = 8,
    ODD_PARITY = 16,
    STOP1 = 0,
    STOP2 = 32
  } __attribute__((packed));

  /**
   * Construct Soft UART with transmitter (only) on given output pin.
   * @param[in] tx transmitter pin.
   */
  UAT(Board::DigitalPin tx);
  
  /**
   * @override IOStream::Device
   * Write character to serial port output buffer. Returns character
   * if successful otherwise on error or buffer full returns EOF(-1),
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

  /**
   * Start Soft UART device driver (transmitter only).
   * @param[in] baudrate serial bitrate (default 9600).
   * @param[in] format serial frame format (default 8 data, 2 stop bits)
   * @return true(1) if successful otherwise false(0).
   */
  bool begin(uint32_t baudrate = 9600UL, uint8_t format = DATA8 + STOP2);

  /**
   * Stop Soft UART device driver. 
   * @return true(1) if successful otherwise false(0)
   */
  bool end();
  
protected:
  static const uint8_t DATA_MASK = 7;
  OutputPin m_tx;
  uint8_t m_stops;
  uint8_t m_bits;
  uint16_t m_us;
};

/**
 * Soft UART using serial write and input sampling. The Output serial
 * write method is used for transmission and a pin change interrupt for 
 * detecting start condition and receiving data.
 */
class UART : public UAT {
public:
  // Default buffer size
  static const uint8_t BUFFER_MAX = 64;

  /**
   * Construct Soft UART with transmitter on given output pin and
   * receiver on given pin change interrupt pin. Received data is
   * captured by an interrupt service routine and put into given
   * buffer (i.e. io-stream).
   * @param[in] tx transmitter pin.
   * @param[in] rx receiver pin.
   * @param[in] ibuf input buffer.
   */
  UART(Board::DigitalPin tx, Board::InterruptPin rx, IOStream::Device* ibuf);
  
  /**
   * @override IOStream::Device
   * Number of bytes available in input buffer.
   * @return bytes.
   */
  virtual int available();

  /**
   * @override IOStream::Device
   * Peek next character from serial port input buffer.
   * Returns character if successful otherwise on error or buffer empty
   * returns EOF(-1),
   * @return character or EOF(-1).
   */
  virtual int peekchar();

  /**
   * @override IOStream::Device
   * Peek for given character from serial port input buffer.
   * @param[in] c character to peek for.
   * @return available or EOF(-1).
   */
  virtual int peekchar(char c);
    
  /**
   * @override IOStream::Device
   * Read character from serial port input buffer.
   * Returns character if successful otherwise on error or buffer empty
   * returns EOF(-1),
   * @return character or EOF(-1).
   */
  virtual int getchar();

  /**
   * Start Soft UART device driver.
   * @param[in] baudrate serial bitrate (default 9600).
   * @param[in] format serial frame format (default 8 data, 2 stop bits)
   * @return true(1) if successful otherwise false(0).
   */
  bool begin(uint32_t baudrate = 9600UL, uint8_t format = DATA8 + STOP2);

  /**
   * Stop Soft UART device driver. 
   * @return true(1) if successful otherwise false(0)
   */
  bool end();
  
protected:
  /** Handling start condition and receive data */
  class RXPinChangeInterrupt : public PinChangeInterrupt {
  public:
    RXPinChangeInterrupt(Board::InterruptPin pin, UART* uart);
    virtual void on_interrupt(uint16_t arg = 0);
    void set_baudrate(uint32_t baudrate);
  protected:
    UART* m_uart;
  };
  RXPinChangeInterrupt m_rx;
  IOStream::Device* m_ibuf;
  uint16_t m_count;
  friend class RXPinChangeInterrupt;
};

};

#endif
