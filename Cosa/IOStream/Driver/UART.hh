/**
 * @file Cosa/IOStream/Driver/UART.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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

#ifndef __COSA_IOSTREAM_DRIVER_UART_HH__
#define __COSA_IOSTREAM_DRIVER_UART_HH__

#if defined(__ARDUINO_TINYX5__)

#include "Cosa/Types.h"
#include "Cosa/Pins.hh"
#include "Cosa/Board.hh"
#include "Cosa/IOStream.hh"

/**
 * Simple software UART for ATtinyX5. Only realizes the putchar, 
 * transmitter. 
 */
class UART : public IOStream::Device {
private:
  OutputPin m_pin;
  uint16_t m_period;
  uint8_t m_format;

public:

  // Serial formats; DATA + PARITY + STOP
  enum {
    DATA5 = 5,
    DATA6 = 6,
    DATA7 = 7,
    DATA8 = 8,
    DATA_MASK = 15,
    NO_PARITY = 0,
    EVEN_PARITY = 16,
    ODD_PARITY = 32,
    STOP1 = 0,
    STOP2 = 64
  } __attribute__((packed));

  /**
   * Construct software serial output device driver handler.
   * @param[in] pin number.
   */
  UART(Board::DigitalPin pin = Board::D0) : 
    IOStream::Device(),
    m_pin(pin, 1),
    m_period(1000000 / 9600),
    m_format(DATA8 + NO_PARITY + STOP2)
  {
  }

  /**
   * @override
   * Write character to software serial output device driver.
   * Returns character if successful otherwise returns EOF(-1),
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

  /**
   * Start software serial output device driver.
   * @param[in] baudrate serial bitrate (default 9600).
   * @param[in] format serial frame format (default 8data, 2stop bit)
   * @return true(1) if successful otherwise false(0)
   */
  bool begin(uint32_t baudrate = 9600, uint8_t format = DATA8 + STOP2);

  /**
   * Stop software serial output device driver.
   * @return true(1) if successful otherwise false(0)
   */
  bool end()
  {
    return (true);
  }
};

#else

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/Board.hh"

/**
 * Basic UART device handler with internal buffering. IOStream
 * devices may be piped with the IOBuffer class. The UART class
 * requires an input- and output IOBuffer instance.
 */
class UART : public IOStream::Device {
private:
  volatile uint8_t* const m_sfr;
  IOBuffer* m_ibuf;
  IOBuffer* m_obuf;

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

  // Interrupt handlers are friends
  friend void USART_UDRE_vect(void);
  friend void USART_RX_vect(void);
#if defined(__ARDUINO_MIGHTY__)
  friend void USART1_UDRE_vect(void);
  friend void USART1_RX_vect(void);
#elif defined(__ARDUINO_MEGA__)
  friend void USART1_UDRE_vect(void);
  friend void USART1_RX_vect(void);
  friend void USART2_UDRE_vect(void);
  friend void USART2_RX_vect(void);
  friend void USART3_UDRE_vect(void);
  friend void USART3_RX_vect(void);
#endif

  /**
   * Common UART transmit interrupt handler.
   */
  void on_udre_interrupt();

  /**
   * Common UART receive interrupt handler.
   */
  void on_rx_interrupt();

public:
  // Default buffer size for standard UART0
  static const uint8_t BUFFER_MAX = 64;

  // Serial formats; DATA + PARITY + STOP
  enum {
    DATA5 = 0,
    DATA6 = _BV(UCSZ00),
    DATA7 = _BV(UCSZ01),
    DATA8 = _BV(UCSZ01) | _BV(UCSZ00),
    DATA9 = _BV(UCSZ02) | _BV(UCSZ01) | _BV(UCSZ00),
    NO_PARITY = 0,
    EVEN_PARITY = _BV(UPM01),
    ODD_PARITY = _BV(UPM01) | _BV(UPM00),
    STOP1 = 0,
    STOP2 = _BV(USBS0)
  } __attribute__((packed));

  /**
   * Construct serial port handler for given UART.
   * @param[in] port number.
   * @param[in] ibuf input stream buffer.
   * @param[in] obuf output stream buffer.
   */
  UART(uint8_t port, IOBuffer* ibuf, IOBuffer* obuf) : 
    IOStream::Device(),
    m_sfr(Board::UART(port)),
    m_ibuf(ibuf),
    m_obuf(obuf)
  {
  }

  /**
   * @override
   * Number of bytes available in input buffer.
   * @return bytes.
   */
  virtual int available()
  {
    return (m_ibuf->available());
  }

  /**
   * @override
   * Write character to serial port output buffer.
   * Returns character if successful otherwise on error or buffer full
   * returns EOF(-1),
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

  /**
   * @override
   * Read character from serial port input buffer.
   * Returns character if successful otherwise on error or buffer empty
   * returns EOF(-1),
   * @return character or EOF(-1).
   */
  virtual int getchar()
  {
    return (m_ibuf->getchar());
  }

  /**
   * @override
   * Flush internal device buffers. Wait for device to become idle.
   * @return zero(0) or negative error code.
   */
  virtual int flush()
  {
    return (m_ibuf->flush() | m_obuf->flush());
  }

  /**
   * Start UART device driver.
   * @param[in] baudrate serial bitrate (default 9600).
   * @param[in] format serial frame format (default async, 8data, 2stop bit)
   * @return true(1) if successful otherwise false(0)
   */
  bool begin(uint32_t baudrate = 9600, uint8_t format = DATA8 + STOP2 + NO_PARITY);

  /**
   * Stop UART device driver.
   * @return true(1) if successful otherwise false(0)
   */
  bool end();

  /**
   * Serial port references. Only uart0 is predefined (reference to global
   * uart). Others are assuped to be allocated by applications and setup
   * with UART_SETUP().
   */
  static UART* uart0;
#if defined(__ARDUINO_MIGHTY__)
  static UART* uart1;
#elif defined(__ARDUINO_MEGA__)
  static UART* uart1;
  static UART* uart2;
  static UART* uart3;
#endif

};

#endif

/**
 * Default serial port(0).
 */
extern UART uart;

/**
 * Setup handling of serial port with given port number and UART instance.
 * @param[in] nr serial port number.
 * @param[in] obj address of UART instance.
 */
#define UART_SETUP(nr,obj) UART::uart ## nr = &obj

#endif
