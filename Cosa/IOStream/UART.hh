/**
 * @file Cosa/IOStream/UART.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Basic UART device handler with internal buffering.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_IOSTREAM_UART_HH__
#define __COSA_IOSTREAM_UART_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"
#include "Cosa/Board.hh"

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define USART_UDRE_vect USART0_UDRE_vect
#endif

extern "C" void USART_UDRE_vect(void) __attribute__ ((signal));

class UART : public IOStream::Device {
private:
  static const uint32_t CYCLES_MAX = 1000000;
  static const uint8_t BUFFER_MAX = 64;
  static const uint8_t BUFFER_MASK = BUFFER_MAX - 1;
  volatile char m_buffer[BUFFER_MAX];
  volatile uint8_t m_head;
  volatile uint8_t m_tail;
  volatile uint8_t* const m_sfr;
  friend void USART_UDRE_vect(void);
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  friend void USART1_UDRE_vect(void);
  friend void USART2_UDRE_vect(void);
  friend void USART3_UDRE_vect(void);
#endif

  /**
   * Return pointer to UART Control and Status Register A (UCSRnA).
   * @return UCSRAn register pointer.
   */
  volatile uint8_t* UCSRnA() 
  { 
    return (m_sfr); 
  }

  /**
   * Return pointer to UART Control and Status Register B (UCSRnB).
   * @return UCSRnB register pointer.
   */
  volatile uint8_t* UCSRnB() 
  { 
    return (m_sfr + 1); 
  }

  /**
   * Return pointer to UART Control and Status Register C (UCSRnC).
   * @return UCSRnC register pointer.
   */
  volatile uint8_t* UCSRnC() 
  { 
    return (m_sfr + 2); 
  }

  /**
   * Return pointer to UART Baud Rate Register (UBRRn).
   * @return UBRRn register pointer.
   */
  volatile uint16_t* UBRRn() 
  { 
    return ((volatile uint16_t*) (m_sfr + 4)); 
  }

  /**
   * Return pointer to UART I/O Data Register (UDRn).
   * @return UDRn register pointer.
   */
  volatile uint8_t* UDRn() 
  { 
    return (m_sfr + 6); 
  }

public:
  /**
   * Construct serial port handler for given UART.
   * @param[in] port number.
   */
  UART(uint8_t port = 0) : 
    IOStream::Device(),
    m_head(0),
    m_tail(0),
    m_sfr(Board::UART(port))
  {
  }

  /**
   * @override
   * Write character to serial port.
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

  /**
   * @override
   * Flush internal device buffers. Wait for device to become idle.
   * @return zero(0) or negative error code.
   */
  virtual int flush();

  /**
   * Start UART device driver.
   * @param[in] baudrate serial bitrate (default 9600).
   * @param[in] format serial frame format (default async, 8data, 2stop bit)
   * @return true(1) if successful otherwise false(0)
   */
  bool begin(uint32_t baudrate = 9600, 
	     uint8_t format = (_BV(USBS0) | (3 << UCSZ00)));

  /**
   * Stop UART device driver.
   * @return true(1) if successful otherwise false(0)
   */
  bool end();
};

/**
 * Default serial port(0).
 */
extern UART uart;

#endif
