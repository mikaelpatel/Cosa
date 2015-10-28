/**
 * @file Cosa/UART.cpp
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

#include "Cosa/Board.hh"
#include "Cosa/UART.hh"

#if defined(BOARD_ATTINY)
// Default is serial output only (UAT)
// static IOBuffer<Soft::UART::RX_BUFFER_MAX> ibuf;
// Soft::UART  __attribute__ ((weak)) uart(Board::D2, Board::PCI1, &ibuf);
Soft::UAT  __attribute__ ((weak)) uart(Board::D2);
#else

#include <avr/power.h>

#if defined(USBCON)
#undef uart
#else
#include "Cosa/IOBuffer.hh"
static IOBuffer<UART::RX_BUFFER_MAX> ibuf;
static IOBuffer<UART::TX_BUFFER_MAX> obuf;
UART __attribute__ ((weak)) uart(0, &ibuf, &obuf);
#endif

UART* UART::uart[Board::UART_MAX] = { NULL };

bool
UART::begin(uint32_t baudrate, uint8_t format)
{
  // Power up the device
  powerup();

  // Check if double rate is not possible
  uint16_t setting = ((F_CPU / 4 / baudrate) - 1) / 2;
  if (setting > 4095) {
    setting = ((F_CPU / 8 / baudrate) - 1) / 2;
    *UCSRnA() = 0;
  } else {
    *UCSRnA() = _BV(U2X0);
  }

  // Set baudrate and format
  *UBRRn() = setting;
  *UCSRnC() = format;

  // Enable receiver and transmitter interrupt
  *UCSRnB() = (_BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0));
  return (true);
}

bool
UART::end()
{
  // Flush an output
  flush();

  // Disable receiver and transmitter interrupt
  *UCSRnB() &= ~(_BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0));

  // Powerdown the device
  powerdown();
  return (true);
}

// A fast track direct to the hardware pipeline is possible when it is
// idle. At 500 Kbps the effective baud-rate increases from 84% to
// 99.9%, 1 Mbps from 42% to 88%, and 2 Mbps from 21% to 88%. Does not
// effect lower baud-rates more than the pipeline is started faster.
#define USE_FAST_TRACK

// A short delay improves synchronization with hardware pipeline at
// 1 Mbps. The effective baud-rate increases at 1 Mbps from 88% to
// 99.5% and at 2 Mbps from 88% to 90%. Note that the delay is tuned
// for program memory string write (at 1 Mbps).
#define USE_SYNC_DELAY

int
UART::putchar(char c)
{
  // Flag that transitter is used
  m_idle = false;

#if defined(USE_FAST_TRACK)
  // Fast track when transmitter is idle; put directly into the pipeline
  if (((*UCSRnB() & _BV(UDRIE0)) == 0) && ((*UCSRnA() & _BV(UDRE0)) != 0)) {
    synchronized {
      *UDRn() = c;
      *UCSRnA() |= _BV(TXC0);
    }
#if defined(USE_SYNC_DELAY)
    // A short delay to make things even faster; optimized for program
    // memory string write (approx. 5 us)
    if (UNLIKELY(*UBRRn() == 1)) {
      _delay_loop_1(25);
      nop();
      nop();
    }
#endif
    return (c & 0xff);
  }
#endif

  // Check if the buffer is full
  while (m_obuf->putchar(c) == IOStream::EOF)
    yield();

  // Enable the transmitter
  *UCSRnB() |= _BV(UDRIE0);
  return (c & 0xff);
}

int
UART::flush()
{
  // Check for idle transmitter; nothing to flush
  if (m_idle) return (0);

  // Wait for transmission to complete
  while ((*UCSRnB() & _BV(UDRIE0)) != 0)
    yield();

  // Wait for the last character to be transmitted
  while ((*UCSRnA() & _BV(TXC0)) == 0)
    ;
  *UCSRnA() |= _BV(TXC0);

  // Mark as idle again
  m_idle = true;
  return (0);
}

void
UART::powerup()
{
  switch (m_port) {
#if defined(power_usart0_enable)
  case 0:
    power_usart0_enable();
    break;
#endif
#if defined(power_usart1_enable)
  case 1:
    power_usart1_enable();
    break;
#endif
#if defined(power_usart2_enable)
  case 2:
    power_usart2_enable();
    break;
#endif
#if defined(power_usart3_enable)
  case 3:
    power_usart3_enable();
    break;
#endif
  default:
    break;
  }
}

void
UART::powerdown()
{
  switch (m_port) {
#if defined(power_usart0_disable)
  case 0:
    power_usart0_disable();
    break;
#endif
#if defined(power_usart1_disable)
  case 1:
    power_usart1_disable();
    break;
#endif
#if defined(power_usart2_disable)
  case 2:
    power_usart2_disable();
    break;
#endif
#if defined(power_usart3_disable)
  case 3:
    power_usart3_disable();
    break;
#endif
  default:
    break;
  }
}

void
UART::on_udre_interrupt()
{
  int c = m_obuf->getchar();
  if (c != IOStream::EOF) {
    *UDRn() = c;
    *UCSRnA() |= _BV(TXC0);
  }
  else {
    *UCSRnB() &= ~_BV(UDRIE0);
  }
}

void
UART::on_rx_interrupt()
{
  m_ibuf->putchar(*UDRn());
}

#define UART_ISR(vec,nr)			\
ISR(vec ## _UDRE_vect)				\
{						\
  if (UNLIKELY(UART::uart[nr] == NULL)) return;	\
  UART::uart[nr]->on_udre_interrupt();		\
}						\
						\
ISR(vec ## _RX_vect)				\
{						\
  if (UNLIKELY(UART::uart[nr] == NULL)) return;	\
  UART::uart[nr]->on_rx_interrupt();		\
}						\

#if defined(USART_UDRE_vect)
UART_ISR(USART, 0)
#endif
#if defined(USART1_UDRE_vect)
UART_ISR(USART1, 1)
#endif
#if defined(USART2_UDRE_vect)
UART_ISR(USART2, 2)
#endif
#if defined(USART3_UDRE_vect)
UART_ISR(USART3, 3)
#endif

#endif

