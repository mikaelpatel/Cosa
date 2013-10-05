/**
 * @file Cosa/Wireless/Driver/CC1101.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Wireless/Driver/CC1101.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Power.hh"
#include "Cosa/RTC.hh"

const uint8_t CC1101::config[CC1101::CONFIG_MAX] __PROGMEM = {
  0x47,		// GDO2 Output Pin Configuration
  0x2E,		// GDO1 Output Pin Configuration
  0x2E,		// GDO0 Output Pin Configuration
  0x07,		// RX FIFO and TX FIFO Thresholds
  0xC0,		// Synchronization word, high byte
  0x5A,		// Synchronization word, low byte
  0x3D,		// Packet Length, 61 bytes
  0x06,		// Packet Automation Control
  0x45,		// Packet Automation Control
  0xFF,		// Device Address
  0x00,		// Channel Number
  0x08,		// Frequency Synthesizer Control
  0x00,		// Frequency Synthesizer Control
  0x10,		// Frequency Control Word, High Byte
  0xA7,		// Frequency Control Word, Middle Byte
  0x62,		// Frequency Control Word, Low Byte
  0xCA,		// Modem Configuration
  0x83,		// Modem Configuration
  0x93,		// Modem Configuration
  0x22,		// Modem Configuration
  0xF8,		// Modem Configuration
  0x35,		// Modem Deviation Setting
  0x07,		// Main Radio Control State Machine Configuration
  0x30,		// Main Radio Control State Machine Configuration
  0x18,		// Main Radio Control State Machine Configuration
  0x16,		// Frequency Offset Compensation Configuration
  0x6C,		// Bit Synchronization Configuration
  0x43,		// AGC Control
  0x40,		// AGC Control
  0x91,		// AGC Control
  0x87,		// High Byte Event0 Timeout
  0x6B,		// Low Byte Event0 Timeout
  0xFB,		// Wake On Radio Control
  0x56,		// Front End RX Configuration
  0x10,		// Front End TX Configuration
  0xE9,		// Frequency Synthesizer Calibration
  0x2A,		// Frequency Synthesizer Calibration
  0x00,		// Frequency Synthesizer Calibration
  0x1F,		// Frequency Synthesizer Calibration
  0x41,		// RC Oscillator Configuration
  0x00		// RC Oscillator Configuration
};

uint8_t 
CC1101::read(uint8_t reg)
{
  spi.begin(this);
  m_status = spi.transfer(header_t(reg, 0, 1));
  uint8_t res = spi.transfer(0);
  spi.end();
  return (res);
}

void 
CC1101::read(uint8_t reg, void* buf, size_t count)
{
  spi.begin(this);
  m_status = spi.transfer(header_t(reg, 1, 1));
  spi.read(buf, count);
  spi.end();
}

void 
CC1101::write(uint8_t reg, uint8_t value)
{
  spi.begin(this);
  m_status = spi.transfer(header_t(reg, 0, 0));
  spi.transfer(value);
  spi.end();
}

void 
CC1101::write(uint8_t reg, const void* buf, size_t count)
{
  spi.begin(this);
  m_status = spi.transfer(header_t(reg, 1, 0));
  spi.write(buf, count);
  spi.end();
}

void 
CC1101::write_P(uint8_t reg, const uint8_t* buf, size_t count)
{
  spi.begin(this);
  m_status = spi.transfer(header_t(reg, 1, 0));
  spi.write_P(buf, count);
  spi.end();
}

void 
CC1101::IRQPin::on_interrupt(uint16_t arg)
{
  if (m_rf == 0) return;
  m_rf->m_avail = 1;
}

void 
CC1101::strobe(Command cmd)
{
  spi.begin(this);
  m_status = spi.transfer(header_t(cmd, 0, 0));
  spi.end();
}

bool 
CC1101::set_mode(Mode mode, Command cmd, uint8_t retry)
{
  while (read_status().mode != IDLE_MODE) Power::sleep(SLEEP_MODE_IDLE);
  do {
    if (read_status().mode == mode) return (true);
    strobe(cmd);
    DELAY(100);
  } while (--retry);
  return (false);
}

void 
CC1101::begin(const uint8_t* setting)
{
  m_cs.pulse(30);
  DELAY(30);
  strobe(SRES);
  DELAY(300);
  write_P(IOCFG2, setting ? setting : config, CONFIG_MAX);
  write(ADDR, m_addr);
  write(PATABLE, 0x60);
  m_irq.enable();
}

int 
CC1101::send(const iovec_t* vec)
{
  size_t count = 0;
  for (const iovec_t* vp = vec; vp->buf != 0; vp++) 
    count += vp->size;
  if (count > PAYLOAD_MAX) return (-3);
  while (read_status().mode != IDLE_MODE) Power::sleep(SLEEP_MODE_IDLE);
  write(TXFIFO, count);
  for (const iovec_t* vp = vec; vp->buf != 0; vp++) 
    write(TXFIFO, vp->buf, vp->size);
  if (!set_transmit_mode()) return (-1);
  return (count);
}

bool 
CC1101::await(uint32_t ms)
{
  uint32_t start = RTC::millis();
  while (!m_avail && (ms == 0 || (RTC::since(start) < ms))) 
    Power::sleep(SLEEP_MODE_IDLE);
  return (m_avail);
}

int 
CC1101::recv(void* buf, size_t count, uint32_t ms)
{
  set_receive_mode();
  if (!await(ms)) return (-2);
  m_avail = 0;
  uint8_t size = read(RXFIFO);
  if (size > count) goto exception;
  read(RXFIFO, buf, size);
  read(RXFIFO, &m_recv_status, sizeof(m_recv_status));
  return (size);
 exception:
  if (read_status().mode != IDLE_MODE) strobe(SIDLE);
  strobe(SFRX);
  return (-1);
}

int 
CC1101::recv(uint8_t& src, void* buf, size_t count, uint32_t ms)
{
  set_receive_mode();
  if (!await(ms)) return (-2);
  m_avail = 0;
  uint8_t size = read(RXFIFO) - 1;
  if (size > count) goto exception;
  src = read(RXFIFO);
  read(RXFIFO, buf, size);
  read(RXFIFO, &m_recv_status, sizeof(m_recv_status));
  return (size);
 exception:
  if (read_status().mode != IDLE_MODE) strobe(SIDLE);
  strobe(SFRX);
  return (-1);
}
