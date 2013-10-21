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

#if !defined(__ARDUINO_TINYX5__)
#include "Cosa/Watchdog.hh"
#include "Cosa/Power.hh"
#include "Cosa/RTC.hh"

/**
 * Default configuration (generated with TI SmartRF Studio tool):
 * Radio: 433 MHz, 38 kbps, GFSK. Whitening. 
 * Packet: Variable packet length with CRC, address check and broadcast(0x00)
 * FIFO: Append link status. 
 * Frame: sync(2), length(1), dest(1), payload(max 59), crc(2)
 * - Send(62): length(1), dest(1), src(1), payload(max 59)
 * - Received(64): length(1), dest(1), src(1), payload(max 59), status(2)
 * Digital Output Pins:
 * - GDO2: valid frame received, active low
 * - GDO1: high impedance, not used
 * - GDO0: high impedance, not used
 */
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

void 
CC1101::IRQPin::on_interrupt(uint16_t arg)
{
  if (m_rf == 0) return;
  m_rf->m_avail = true;
}

void 
CC1101::await(Mode mode)
{
  while (read_status().mode != mode) 
    Power::sleep(m_mode);
}

bool
CC1101::begin(const void* config)
{
  // Reset the device
  m_cs.pulse(30);
  DELAY(30);
  strobe(SRES);
  DELAY(300);

  // Upload the configuration. Check for default configuration
  spi.begin(this);
  write_P(IOCFG2, 
	  config ? (const uint8_t*) config : CC1101::config, 
	  CONFIG_MAX);
  spi.end();

  // Adjust configuration with instance specific state
  uint16_t sync = swap(m_addr.network);
  spi.begin(this);
  write(PATABLE, 0x60);
  write(CHANNR, m_channel);
  write(ADDR, m_addr.device);
  write(SYNC1, &sync, sizeof(sync));
  spi.end();

  // Initiate device driver state and enable interrupt handler
  m_avail = false;
  m_irq.enable();
  return (true);
}

bool 
CC1101::end()
{
  m_irq.disable();
  powerdown();
  return (true);
}

int 
CC1101::send(uint8_t dest, uint8_t port, const iovec_t* vec)
{
  // Sanity check the payload size
  if (vec == NULL) return (-1);
  size_t len = 0;
  for (const iovec_t* vp = vec; vp->buf != 0; vp++)
    len += vp->size;
  if (len > PAYLOAD_MAX) return (-1);

  // Wait for the device to become idle before writing the frame
  await(IDLE_MODE);

  // Write frame header(length, dest, src, port)
  spi.begin(this);
  write(TXFIFO, len + 3);
  write(TXFIFO, dest);
  write(TXFIFO, m_addr.device);
  write(TXFIFO, port);
  spi.end();

  // Write frame payload
  for (const iovec_t* vp = vec; vp->buf != 0; vp++) {
    spi.begin(this);
    write(TXFIFO, vp->buf, vp->size);
    spi.end();
  }

  // Trigger the transmit
  strobe(STX);
  return (len);
}

int 
CC1101::send(uint8_t dest, uint8_t port, const void* buf, size_t len)
{
  iovec_t vec[2];
  iovec_t* vp = vec;
  iovec_arg(vp, buf, len);
  iovec_end(vp);
  return (send(dest, port, vec));
}

int 
CC1101::recv(uint8_t& src, uint8_t& port, void* buf, size_t len, uint32_t ms)
{
  // Check if we need to wait for a message
  if (!m_avail) {
    // Fix: Use wakeup on radio to reduce power during wait
    uint32_t start = RTC::millis();
    strobe(SRX);
    while (!m_avail && ((ms == 0) || (RTC::since(start) < ms)))
      Power::sleep(m_mode);
    if (!m_avail) return (-2);
  }
  m_avail = false;

  // Read the payload size and check against buffer length
  spi.begin(this);
  uint8_t size = read(RXFIFO) - 3;
  if (size > len) {
    spi.end();
    strobe(SIDLE);
    strobe(SFRX);
    return (-1);
  }

  // Read the frame (dest, src, payload)
  m_dest = read(RXFIFO);
  src = read(RXFIFO);
  port = read(RXFIFO);
  read(RXFIFO, buf, size);
  spi.end();

  // Read the link quality status
  spi.begin(this);
  read(RXFIFO, &m_recv_status, sizeof(m_recv_status));
  spi.end();

  // Fix: Add possible address checking for robustness
  return (size);
}

void 
CC1101::powerdown()
{
  await(IDLE_MODE);
  strobe(SPWD);
}

void 
CC1101::wakeup_on_radio()
{
  await(IDLE_MODE);
  strobe(SWOR);
}
#endif
