/**
 * @file CosaRF24scanner.ino
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
 * @section Description
 * Cosa 2.4 GHz Scanner using the NRF24L01P device driver.
 *
 * @section Acknowledgement
 * This tool is inspired by the Nrf24L01-Poor Man's 2.4 GHz Scanner.
 * http://arduino-info.wikispaces.com/Nrf24L01-Poor+Man%27s+2.4+GHz+Scanner
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <NRF24L01P.h>

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"
#include "Cosa/Trace.hh"
#undef measure

/**
 * Extend the NRF24L01P device driver with measurement and plot of
 * received power detect. For more details see ch. 6.3 and 6.4.
 *
 * @section References
 * nRF24L01+ Product Specification (Rev. 1.0),
 * http://www.nordicsemi.com/kor/nordic/download_resource/8765/2/17776224
 */
class RF24scanner : public NRF24L01P {
public:
  /** Number of channels scanned; 2 MHz bandwidth */
  static const uint8_t CHANNEL_MAX = 64;

  /**
   * Construct the 2.4 GHz channel scanner.
   */
  RF24scanner() : NRF24L01P(0xC05A, 0x11) {}

  /**
   * Measure the received power for all channels for the given
   * number of samples per channel.
   * @param[in] samples per channel (default 200).
   */
  void measure(uint16_t samples = 200);

  /**
   * Plot the number of received power detect per channel to the
   * given output stream. The print out is channel frequency,
   * NRF24L01 channel number, WiFi channel number and samples
   * as a bar.
   * @param[in] outs output stream.
   */
  void plot(IOStream& outs);

protected:
  static const uint16_t WAIT_TIME = 130 + 40 + 30;
  uint8_t m_channel[CHANNEL_MAX];
};

void
RF24scanner::measure(uint16_t samples)
{
  if (samples == 0) return;

  // Clear the receive power detect counters and configure as receiver
  memset(m_channel, 0, sizeof(m_channel));
  write(CONFIG, (_BV(EN_CRC) | _BV(CRCO) | _BV(PWR_UP) | _BV(PRIM_RX)));

  do {
    // Set channel and check the receiver power detect register
    for (uint8_t i = 0; i < CHANNEL_MAX; i++) {
      write(RF_CH, i * 2);
      m_ce.set();
      DELAY(WAIT_TIME);
      m_channel[i] += (read(RPD) != 0);
      m_ce.clear();
    }
  } while (--samples);
}

void
RF24scanner::plot(IOStream& outs)
{
  // Plot the measurements with channel frequency and number (wifi)
  trace << PSTR("--------------------") << endl;
  for (uint8_t i = 0; i < CHANNEL_MAX; i++) {
    if (m_channel[i] != 0) {
      float freq = 2.4 + 0.002 * i;
      outs << freq << ':' << 2 * i << ':';
      if (freq < 2.412 || freq > 2.472)
	outs << PSTR("-:");
      else
	outs << (int) ((freq - 2.412) * 200.0) + 1 << ':';
      for (uint8_t j = 0; j < m_channel[i]; j++)
	outs << 'I';
      outs << endl;
    }
  }
}

// Scanner with the default pin configuration
RF24scanner scanner;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaRF24scanner: started"));
  trace << PSTR("channel frequency:number:wifi:detects") << endl;
  Watchdog::begin();
  RTT::begin();
  ASSERT(scanner.begin());
}

void loop()
{
  scanner.measure();
  scanner.plot(trace);
}
