/**
 * @file CosaSD.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Demonstration and test of the SD card device driver. Allows trace
 * to the ST7735 TFT Canvas and Textbox (IOStream::Device). This also
 * allows test and validation of Cosa multiple SPI device handling.
 *
 * @section Circuit
 * This sketch was designed for a HY-1.8 SPI LCD/SD module but may be
 * used with other SD modules and ST7735 TFT modules. The sketch uses
 * SPI pins (MOSI, MISO, SCK) and chip/mode select pins. The SPI pins
 * are common to both SD and TFT. TFT does not use MISO. The default
 * pins for chip/mode select are used; SD::CS/D8, TFT::CS/D10 and
 * TFT::A0/D9 (this is the mode pin).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <SD.h>

#include "Cosa/RTT.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// Uncomment/comment to enable/disable trace output to TFT/Canvas/Textbox
// #define USE_SD_ADAPTER
#define USE_SD_DATA_LOGGING_SHIELD
//#define USE_ETHERNET_SHIELD
//#define USE_TFT_ST7735

#if defined (USE_TFT_ST7735)
#include <Canvas.h>
#include "Canvas/Element/Textbox.hh"
#include <GDDRAM.h>
#include <ST7735.h>

// Use ST7735 default pins; SPI (MOSI, SCK), CS (D10), AO (D9)
SD sd;
ST7735 tft;
Textbox textbox(&tft);
static const uint8_t WIDTH = 6;
#else
static const uint8_t WIDTH = 32;

#if defined(USE_ETHERNET_SHIELD)
SD sd(Board::D4);
OutputPin eth(Board::D10, 1);

#elif defined(WICKEDDEVICE_WILDFIRE) || defined(USE_SD_ADAPTER)
SD sd;

#elif defined(USE_SD_DATA_LOGGING_SHIELD)
SD sd(Board::D10);
#endif
#endif

void setup()
{
#if defined(USE_TFT_ST7735)
  IOStream::Device* dev = &textbox;
  tft.begin();
  tft.set_canvas_color(Canvas::WHITE);
  tft.set_orientation(Canvas::LANDSCAPE);
  textbox.set_text_color(Canvas::BLACK);
  textbox.set_text_port(2, 2, tft.WIDTH, tft.HEIGHT);
  tft.fill_screen();
#else
  IOStream::Device* dev = &uart;
  uart.begin(115200);
#endif

  Watchdog::begin();
  RTT::begin();
  trace.begin(dev, PSTR("CosaSD: started"));
  TRACE(free_memory());
  TRACE(sizeof(SD));
}

void loop()
{
  uint8_t buf[SD::BLOCK_MAX];
  uint8_t save[16];
  SD::cid_t* cid = (SD::cid_t*) save;
  SD::csd_t* csd = (SD::csd_t*) save;

  INFO("Connect to card and switch to a higher clock frequency", 0);
  ASSERT(sd.begin(SPI::DIV2_CLOCK));
  ASSERT(sd.type());

  INFO("Read CID and print fields", 0);
  ASSERT(sd.read(cid));
  trace.print(cid, sizeof(SD::cid_t), IOStream::hex, WIDTH);
  sleep(1);
  trace << PSTR("Manufacturer ID:") << cid->mid << endl;
  trace << PSTR("OEM/Application ID:") << cid->oid[0] << cid->oid[1] << endl;
  trace << PSTR("Product name:");
  for (uint8_t i = 0; i < 5; i++)
    trace << cid->pnm[i];
  trace << endl;
  trace << PSTR("Product revision:") << bcd << cid->prv << endl;
  trace << PSTR("Product serial number:") << swap((int32_t) cid->psn) << endl;
  trace << PSTR("Manufacturing date:");
  cid->mdt = swap((int16_t) cid->mdt);
  trace << (cid->mdt & 0xf) << '-' << (cid->mdt >> 4) << endl;
  sleep(1);

  INFO("Read CSD and dump", 0);
  ASSERT(sd.read(csd));
  trace.print(csd, sizeof(SD::csd_t), IOStream::hex, WIDTH);
  trace << PSTR("CSD version:") << csd->v1.csd_ver+1 << endl;
  sleep(1);

  INFO("Read block zero and dump", 0);
  ASSERT(sd.read(0, buf));
  trace.print(buf, sizeof(buf), IOStream::hex, WIDTH);

  INFO("Modify first 16 bytes and write to card", 0);
  for (uint8_t i = 0; i < sizeof(save); i++) {
    save[i] = buf[i];
    buf[i] = 0xa5;
  }
  ASSERT(sd.write(0, buf));
  sleep(1);

  INFO("Read back and restore content", 0);
  ASSERT(sd.read(0, buf));
  trace.print(buf, sizeof(buf), IOStream::hex, WIDTH);
  for (uint8_t i = 0; i < sizeof(save); i++) {
    // buf[i] = save[i];
    buf[i] = 0;
  }
  ASSERT(sd.write(0, buf));
  sleep(1);

  INFO("Read four first blocks and dump", 0);
  for (uint8_t block = 0; block < 4; block++) {
    TRACE(block);
    ASSERT(sd.read(block, buf));
    trace.print(buf, sizeof(buf), IOStream::hex, WIDTH);
  }
  sleep(1);

  ASSERT(sd.end());
  ASSERT(true == false);
}
