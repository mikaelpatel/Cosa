/**
 * @file RFM69.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#include "RFM69.hh"

#if !defined(BOARD_ATTINYX5)

#include "Cosa/Power.hh"
#include "Cosa/RTT.hh"

// Device configuration support macros
#define REG_VALUE8(reg,value) (reg), (uint8_t) (value)
#define REG_VALUE16(reg,value)					\
  REG_VALUE8(reg,value >> 8),					\
  REG_VALUE8(reg+1,value)
#define REG_VALUE24(reg,value)					\
  REG_VALUE8(reg,value >> 16),					\
  REG_VALUE8(reg+1,value >> 8),					\
  REG_VALUE8(reg+2,value)

// Crystal Oscillator Frequency/Step; 32 MHz/61.0 Hz
#define FXOSC 32000000L
#define FSTEP (FXOSC >> 19)

// RF Carrier Frequency, 24-bit (RF / FSTEP)
#define FRF_315_MHZ 0x4EC000L
#define FRF_434_MHZ 0x6C8000L
#define FRF_868_MHZ 0xD90000L
#define FRF_915_MHZ 0xE4C000L
#define FRF_SETTING FRF_868_MHZ

// Bitrates, 16-bit (FSOSC / BITRATE)
#define BITRATE_1200_BPS 0x682B
#define BITRATE_2400_BPS 0x3415
#define BITRATE_4800_BPS 0x1A0B
#define BITRATE_9600_BPS 0x0D05
#define BITRATE_19200_BPS 0x0683
#define BITRATE_38400_BPS 0x0341
#define BITRATE_57600_BPS 0x022C
#define BITRATE_76800_BPS 0x01A1
#define BITRATE_115200_BPS 0x0116
#define BITRATE_153600_BPS 0x00D0
#define BITRATE_SETTING BITRATE_4800_BPS

// Frequency deviation, 16-bit (FDEV / FSTEP); 5 KHz
#define FDEV_SETTING 0x0052

/**
 * Default configuration:
 * Radio: 868 MHz, 4.8 kbps, GFSK(0). Whitening, 13 dBm.
 * Packet: Variable packet length with CRC, address check and broadcast(0x00)
 * Frame: sync(2), length(1), dest(1), src(1), port(1), payload(max 63), crc(2)
 * Digital Output Pins: DIO0, Asserts: RX:CRC_OK, TX:PACKET_SENT
 */
const uint8_t RFM69::config[] __PROGMEM = {
  // Common Configuration Registers
  REG_VALUE8(OP_MODE, SEQUENCER_ON | LISTEN_OFF | STANDBY_MODE),
  REG_VALUE8(DATA_MODUL, PACKET_MODE | FSK_MODULATION | FSK_NO_SHAPING),
  REG_VALUE16(BITRATE, BITRATE_SETTING),
  REG_VALUE16(FDEV, FDEV_SETTING),
  REG_VALUE24(FRF, FRF_SETTING),
  REG_VALUE8(AFC_CTRL, AFC_LOW_BETA_OFF),
  // Transmitter Registers
  REG_VALUE8(PA_LEVEL, PA0_ON | PA1_OFF | PA2_OFF | FULL_OUTPUT_POWER),
  REG_VALUE8(PA_RAMP, 9),
  REG_VALUE8(OCP, OCP_ON | 10),
  REG_VALUE8(LNA, ZIN_200_OHM | (1 << CURRENT_GAIN)),
  // Receiver Registers
  REG_VALUE8(RX_BW, (2 << DCC_FREQ) | BW_MANT_24 | (5 << BW_EXP)),
  // IRQ and Pin Mapping Registers
  REG_VALUE8(DIO_MAPPING1, 0x0),
  REG_VALUE8(DIO_MAPPING2, 0x7),
  REG_VALUE8(RSSI_THRESH, 220),
  // Packet Engine Registers
  REG_VALUE16(PREAMBLE, 3),
  REG_VALUE8(SYNC_CONFIG, SYNC_ON | FIFO_FILL_AUTO
	     | ((sizeof(int16_t) - 1) << SYNC_SIZE)),
  REG_VALUE8(PACKET_CONFIG1, VARIABLE_LENGTH | WHITENING
	     | CRC_ON | CRC_AUTO_CLEAR_ON
	     | ADDR_FILTER_ON),
  REG_VALUE8(PAYLOAD_LENGTH, 66),
  REG_VALUE8(BROADCAST_ADDR, BROADCAST),
  REG_VALUE8(FIFO_THRESHOLD, TX_START_NOT_EMPTY | 15),
  REG_VALUE8(PACKET_CONFIG2, (1 << INTER_PACKET_RX_DELAY)
	     | AUTO_RX_RESTART_ON
	     | AES_OFF),
  REG_VALUE8(TEST_PA1, TEST_PA1_NORMAL_MODE),
  REG_VALUE8(TEST_PA2, TEST_PA2_NORMAL_MODE),
  REG_VALUE8(TEST_DAGC, TEST_DAGC_IMPROVED_MARGIN_AFC_LOG_BETA_OFF),
  0
};

RFM69::RFM69(uint16_t net, uint8_t dev,
	     Board::DigitalPin csn,
	     Board::ExternalInterruptPin irq) :
  SPI::Driver(csn, SPI::ACTIVE_LOW, SPI::DIV4_CLOCK, 0, SPI::MSB_ORDER, &m_irq),
  Wireless::Driver(net, dev),
  m_irq(irq, ExternalInterrupt::ON_RISING_MODE, this)
{
}

void
RFM69::IRQPin::on_interrupt(uint16_t arg)
{
  UNUSED(arg);

  // The interrupt handler is called on rising signal (RFM69:DIO0).
  // This occures on TX: PACKET_SENT and RX: CRC_OK
  if (UNLIKELY(m_rf == 0)) return;
  if (m_rf->m_opmode == RECEIVER_MODE)
    m_rf->m_avail = true;
  else if (m_rf->m_opmode == TRANSMITTER_MODE)
    m_rf->m_done = true;
}

void
RFM69::set(Mode mode)
{
  write(OP_MODE, (read(OP_MODE) & ~MODE_MASK) | mode);
  while ((read(IRQ_FLAGS1) & MODE_READY) == 0x00) DELAY(10);
  m_opmode = mode;
}

bool
RFM69::begin(const void* config)
{
  // Wait for the transceiver to become ready
  do write(SYNC_VALUE1, 0xaa); while (read(SYNC_VALUE1) != 0xaa);
  do write(SYNC_VALUE1, 0x55); while (read(SYNC_VALUE1) != 0x55);

  // Upload the configuration. Check for default configuration
  const uint8_t* cp = RFM69::config;
  Reg reg;
  if (config != NULL) cp = (const uint8_t*) config;
  while ((reg = (Reg) pgm_read_byte(cp++)) != 0)
    write(reg, pgm_read_byte(cp++));

  // Adjust configuration with instance specific state
  uint16_t sync = hton(m_addr.network);
  write(SYNC_VALUE1, &sync, sizeof(sync));
  write(NODE_ADDR, m_addr.device);

  // Set standby mode and calibrate RC oscillator
  recalibrate();

  // Initiate device driver state and enable interrupt handler
  m_avail = false;
  m_done = true;
  spi.attach(this);
  m_irq.enable();
  return (true);
}

bool
RFM69::end()
{
  m_irq.disable();
  powerdown();
  return (true);
}

int
RFM69::send(uint8_t dest, uint8_t port, const iovec_t* vec)
{
  // Sanity check the payload size
  if (UNLIKELY(vec == NULL)) return (EINVAL);
  size_t len = iovec_size(vec);
  if (UNLIKELY(len > PAYLOAD_MAX)) return (EMSGSIZE);

  // Check if a packet available. Should receive before send
  if (UNLIKELY(m_avail)) return (ENXIO);

  // Write frame header(length, dest, src, port) and payload
  spi.acquire(this);
    spi.begin();
      spi.transfer(REG_WRITE | FIFO);
      spi.transfer(len + HEADER_MAX);
      spi.transfer(dest);
      spi.transfer(m_addr.device);
      spi.transfer(port);
      spi.write(vec);
    spi.end();
  spi.release();

  // Trigger the transmit and await completion. Set standby mode
  m_done = false;
  set(TRANSMITTER_MODE);
  while (!m_done) yield();
  set(STANDBY_MODE);

  // Return total length of payload
  return (len);
}

int
RFM69::send(uint8_t dest, uint8_t port, const void* buf, size_t len)
{
  iovec_t vec[2];
  iovec_t* vp = vec;
  iovec_arg(vp, buf, len);
  iovec_end(vp);
  return (send(dest, port, vec));
}

int
RFM69::recv(uint8_t& src, uint8_t& port, void* buf, size_t len, uint32_t ms)
{
  // Set receive mode and wait for a message
  set(RECEIVER_MODE);
  uint32_t start = RTT::millis();
  while (!m_avail && ((ms == 0) || (RTT::since(start) < ms))) yield();

  // Set standby and check if a message was received
  set(STANDBY_MODE);
  if (!m_avail) return (ETIME);
  m_avail = false;

  // Read the payload size and check size
  spi.acquire(this);
    spi.begin();
      spi.transfer(REG_READ | FIFO);
      uint8_t size = spi.transfer(0);
      size = size - HEADER_MAX;
      if (size > len) {
	spi.end();
	spi.release();
	return (EMSGSIZE);
      }
      // Read the frame (dest, src, payload)
      m_dest = spi.transfer(0);
      src = spi.transfer(0);
      port = spi.transfer(0);
      spi.read(buf, size);
    spi.end();
  spi.release();

  return (size);
}

void
RFM69::powerdown()
{
  set(SLEEP_MODE);
}

void
RFM69::wakeup_on_radio()
{
  // Fix: Use LISTEN_ON instead
  set(STANDBY_MODE);
}

void
RFM69::output_power_level(int8_t dBm)
{
  // Fix: High power level setting for RFM69HW
  if (dBm < -18) dBm = -18; else if (dBm > 13) dBm = 13;
  uint8_t level = (dBm + 18) & OUTPUT_POWER_MASK;
  uint8_t pa_level = read(PA_LEVEL) & ~OUTPUT_POWER_MASK;
  write(PA_LEVEL, pa_level | level);
}

int
RFM69::input_power_level()
{
  return ((-read(RSSI_VALUE)) >> 1);
}

int
RFM69::temperature()
{
  set(STANDBY_MODE);
  write(TEMP1, TEMP_MEAS_START);
  while (read(TEMP1) & TEMP_MEAS_RUNNING) DELAY(100);
  return (-read(TEMP2));
}

void RFM69::recalibrate()
{
  set(STANDBY_MODE);
  write(OSC1, RC_CAL_START);
  while ((read(OSC1) & RC_CAL_DONE) == 0x00) DELAY(100);
}

#endif
