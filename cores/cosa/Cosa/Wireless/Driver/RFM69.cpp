/**
 * @file Cosa/Wireless/Driver/RFM69.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#include "Cosa/Wireless/Driver/RFM69.hh"

#if !defined(__ARDUINO_TINYX5__)
#include "Cosa/Watchdog.hh"
#include "Cosa/Power.hh"
#include "Cosa/RTC.hh"

// Device configuration support macro
#define REG_VALUE8(reg,value) (reg), (value)
#define REG_VALUE16(reg,value)					\
  REG_VALUE8(reg,value >> 8),					\
  REG_VALUE8(reg+1,value)
#define REG_VALUE24(reg,value)					\
  REG_VALUE8(reg,value >> 16),					\
  REG_VALUE8(reg+1,value >> 8),					\
  REG_VALUE8(reg+2,value)
#define REG_VALUE_END() 0

// RF Carrier Frequency; 24-bit
#define FRF_315_MHZ 0x4EC000L
#define FRF_434_MHZ 0x6C8000L
#define FRF_868_MHZ 0xD90000L
#define FRF_915_MHZ 0xE4C000L
#define FRF_MHZ FRF_868_MHZ

// Bitrate, 4.8 Kbps; 16-bit
#define BITRATE_315_MHZ 0x682B
#define BITRATE_434_MHZ 0x3415
#define BITRATE_868_MHZ 0x1A0B
#define BITRATE_915_MHZ 0x0D05
#define BITRATE_BPS BITRATE_868_MHZ

// Frequency deviation, 5 KHz; 16-bit
#define FDEV_KHZ 0x0052

/**
 * Default configuration:
 * Radio: 868 MHz, 4.8 kbps, GFSK(0). Whitening, 0 dBm. 
 * Packet: Variable packet length with CRC, address check and broadcast(0x00)
 * Frame: sync(2), length(1), dest(1), src(1), port(1), payload(max 63), crc(2)
 * Digital Output Pins: DIO0, Asserts: RX:CRC_OK, TX:PACKET_SENT
 */
const uint8_t RFM69::config[] __PROGMEM = {
  // Common Configuration Registers
  REG_VALUE8(OP_MODE, SEQUENCER_ON | LISTEN_OFF | STANDBY_MODE),
  REG_VALUE8(DATA_MODUL, PACKET_MODE | FSK_MODULATION | FSK_NO_SHAPING),
  REG_VALUE16(BITRATE, BITRATE_BPS),
  REG_VALUE16(FDEV, FDEV_KHZ),
  REG_VALUE24(FRF, FRF_MHZ),
  REG_VALUE8(AFC_CTRL, AFC_LOW_BETA_OFF),
  // Transmitter Registers
  REG_VALUE8(PA_LEVEL, PA0_ON | PA1_OFF | PA2_OFF | FULL_OUTPUT_POWER),
  REG_VALUE8(PA_RAMP, 9),
  REG_VALUE8(OCP, OCP_ON | 10),
  // Receiver Registers
  REG_VALUE8(RX_BW, (2 << DCC_FREQ) | BW_MANT_24 | (5 << BW_EXP)),
  // IRQ and Pin Mapping Registers
  REG_VALUE8(DIO_MAPPING1, 0),
  REG_VALUE8(DIO_MAPPING2, 0x7),
  REG_VALUE8(RSSI_THRESH, 220),
  // Packet Engine Registers
  REG_VALUE16(PREAMBLE, 3),
  REG_VALUE8(SYNC_CONFIG, SYNC_ON | FIFO_FILL_AUTO  | (1 << SYNC_SIZE)),
  REG_VALUE8(PACKET_CONFIG1, VARIABLE_LENGTH | WHITENING 
	     | CRC_ON | CRC_AUTO_CLEAR_ON 
	     | ADDR_FILTER_ON),
  REG_VALUE8(PAYLOAD_LENGTH, 66),
  REG_VALUE8(BROADCAST_ADDR, BROADCAST),
  REG_VALUE8(FIFO_THRESHOLD, TX_START_NOT_EMPTY | 15),
  REG_VALUE8(PACKET_CONFIG2, (1 << INTER_PACKET_RX_DELAY) 
	     | AUTO_RX_RESTART_ON 
	     | AES_OFF),
  REG_VALUE8(TEST_DAGC, TEST_DAGC_IMPROVED_MARGIN_AFC_LOG_BETA_OFF),
  REG_VALUE_END()
};

void 
RFM69::IRQPin::on_interrupt(uint16_t arg)
{
  // The interrupt handler is called on rising signal (RFM69:DIO0). 
  // This occures on TX: PACKET_SENT and RX: CRC_OK
  if (m_rf == 0) return;
  if (m_rf->m_opmode == RECEIVER_MODE)
    m_rf->m_avail = true;
  else if (m_rf->m_opmode == TRANSMITTER_MODE)
    m_rf->m_done = true;
}

void
RFM69::set(Mode mode)
{
  write(OP_MODE, (read(OP_MODE) & ~MODE_MASK) | mode);
  while ((read(IRQ_FLAGS1) & MODE_READY) == 0x00);
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
  if (config != NULL) cp = (const uint8_t*) config;
  for (Reg reg; (reg = (Reg) pgm_read_byte(cp++)) != 0; cp++)
    write(reg, pgm_read_byte(cp));

  // Adjust configuration with instance specific state
  uint16_t sync = hton(m_addr.network);
  write(SYNC_VALUE1, &sync, sizeof(sync));
  write(NODE_ADDR, m_addr.device);

  // Set standby mode and calibrate RC oscillator
  set(STANDBY_MODE);
  write(OSC1, RC_CAL_START);
  while ((read(OSC1) & RC_CAL_DONE) == 0x00);

  // Initiate device driver state and enable interrupt handler
  m_avail = false;
  m_done = true;
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
  if (vec == NULL) return (-1);
  size_t len = 0;
  for (const iovec_t* vp = vec; vp->buf != NULL; vp++)
    len += vp->size;
  if (len > PAYLOAD_MAX) return (-1);

  // Check if a packet available. Should receive before send
  if (m_avail) return (-2);

  // Wait for previous packet send if any
  while (!m_done) Power::sleep(m_mode);

  // Put the device in standby before writing packet
  set(STANDBY_MODE);

  // Write frame header(length, dest, src, port) and payload
  spi.begin(this);
  spi.transfer(REG_WRITE | FIFO);
  spi.transfer(len + HEADER_MAX);
  spi.transfer(dest);
  spi.transfer(m_addr.device);
  spi.transfer(port);
  for (const iovec_t* vp = vec; vp->buf != NULL; vp++)
    spi.transfer(vp->buf, vp->size);
  spi.end();

  // Trigger the transmit
  m_done = false;
  set(TRANSMITTER_MODE);

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
  // Check if we need to wait for a message; outgoing or incoming packet
  if (!m_avail) {
    uint32_t start = RTC::millis();
    while (!m_done && ((ms == 0) || (RTC::since(start) < ms)))
      Power::sleep(m_mode);
    if (!m_done) return (-2);
    set(RECEIVER_MODE);
    while (!m_avail && ((ms == 0) || (RTC::since(start) < ms)))
      Power::sleep(m_mode);
    if (!m_avail) return (-2);
  }
  
  // Set standby while retrieving the paket
  set(STANDBY_MODE);

  // Read the payload size and check size
  spi.begin(this);
  uint8_t size = spi.transfer(REG_READ | FIFO) - HEADER_MAX;
  if (size > len) {
    spi.end();
    set(RECEIVER_MODE);
    return (-1);
  } 

  // Read the frame (dest, src, payload)
  m_dest = spi.transfer(0);
  src = spi.transfer(0);
  port = spi.transfer(0);
  spi.read(buf, size);
  spi.end();

  // Turn on receive mode again and return size of payload
  set(RECEIVER_MODE);
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
RFM69::set_output_power_level(int8_t dBm)
{
  // Fix: High power level setting for RFM69HW
  uint8_t pa_level = read(PA_LEVEL) & ~OUTPUT_POWER_MASK;
  write(PA_LEVEL, pa_level | (dBm & OUTPUT_POWER_MASK));
}

int 
RFM69::get_input_power_level()
{
  // Fix: Should be performed with detecting preamble?
  write(RSSI_CONFIG, RSSI_START);
  while ((read(RSSI_CONFIG) & RSSI_DONE) == 0x00);
  return ((-read(RSSI_VALUE)) >> 1);
}

#endif
