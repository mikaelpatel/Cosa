/**
 * @file CosaCanvasFonts.ino
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
 * Cosa demonstration of device driver for ST7735 or ILI9341.
 * Shows use of different fonts.
 *
 * @section Circuit
 * @code
 *                           ST7735
 *                       +------------+
 * (GND)---------------1-|GND         |
 * (VCC)---------------2-|VCC         |
 *                      -|            |
 * (RST)---------------6-|RESET       |
 * (D9)----------------7-|A0          |
 * (MOSI/D11)----------8-|SDA         |
 * (SCK/D13)-----------9-|SCK         |
 * (SS/D10)-----------10-|CS          |
 *                      -|            |
 * (VCC)----[330]-----15-|LED+        |
 * (GND)--------------16-|LED-        |
 *                       +------------+
 *
 *                           ILI9341
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (GND)---------------2-|GND         |
 * (SS/D10)------------3-|CS          |
 * (RST)---------------4-|RST         |
 * (D9)----------------5-|DC          |
 * (MOSI/D11)----------6-|SDI         |
 * (SCK/D13)-----------7-|SCK         |
 * (VCC)------[330]----8-|LED         |
 * (MISO/D12)----------9-|SDO         |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Canvas.hh"
#include "Cosa/Canvas/Element/Textbox.hh"

//#define ONE_CHAR '&'
#define CYCLE_CHARS 0 // ms; 0 to benchmark

#define USE_TFT_ST7735
//#define USE_TFT_ILI9341

//#define FIXEDNUMS_8x16
//#define FONT_5x8
//#define FONT_6x9
//#define FONT_6x10
#define FONT_6x12
//#define FONT_6x13
//#define FONT_6x13B
//#define FONT_7x13
//#define FONT_7x13B
//#define FONT_7x14
//#define FONT_7x14B
//#define FONT_8x13
//#define FONT_8x13B
//#define FONT_8x16
//#define FONT_9x15
//#define FONT_9x15B
//#define FONT_10x20
//#define FONT_12x24
//#define SEGMENT_32x50
//#define SYSTEM_5x7

#ifdef USE_TFT_ST7735
#include "Cosa/Canvas/Driver/ST7735.hh"
ST7735 tft;
#define DEVICE "ST7735"
#endif

#ifdef USE_TFT_ILI9341
#include "Cosa/Canvas/Driver/ILI9341.hh"
ILI9341 tft;
#define DEVICE "ILI9341"
#endif

#ifdef SYSTEM_5x7
#include "Cosa/Canvas/Font/System5x7.hh"
#define FONT system5x7
#endif

#ifdef FONT_5x8
#include "Cosa/Canvas/Font/Font5x8.hh"
#define FONT font5x8
#endif

#ifdef FONT_6x9
#include "Cosa/Canvas/Font/Font6x9.hh"
#define FONT font6x9
#endif

#ifdef FONT_6x10
#include "Cosa/Canvas/Font/Font6x10.hh"
#define FONT font6x10
#endif

#ifdef FONT_6x12
#include "Cosa/Canvas/Font/Font6x12.hh"
#define FONT font6x12
#endif

#ifdef FONT_6x13
#include "Cosa/Canvas/Font/Font6x13.hh"
#define FONT font6x13
#endif

#ifdef FONT_6x13B
#include "Cosa/Canvas/Font/Font6x13B.hh"
#define FONT font6x13B
#endif

#ifdef FONT_7x13
#include "Cosa/Canvas/Font/Font7x13.hh"
#define FONT font7x13
#endif

#ifdef FONT_7x13B
#include "Cosa/Canvas/Font/Font7x13B.hh"
#define FONT font7x13B
#endif

#ifdef FONT_7x14
#include "Cosa/Canvas/Font/Font7x14.hh"
#define FONT font7x14
#endif

#ifdef FONT_7x14B
#include "Cosa/Canvas/Font/Font7x14B.hh"
#define FONT font7x14B
#endif

#ifdef FONT_8x13
#include "Cosa/Canvas/Font/Font8x13.hh"
#define FONT font8x13
#endif

#ifdef FONT_8x13B
#include "Cosa/Canvas/Font/Font8x13B.hh"
#define FONT font8x13B
#endif

#ifdef FONT_8x16
#include "Cosa/Canvas/Font/Font8x16.hh"
#define FONT font8x16
#endif

#ifdef FONT_9x15
#include "Cosa/Canvas/Font/Font9x15.hh"
#define FONT font9x15
#endif

#ifdef FONT_9x15B
#include "Cosa/Canvas/Font/Font9x15B.hh"
#define FONT font9x15B
#endif

#ifdef FONT_10x20
#include "Cosa/Canvas/Font/Font10x20.hh"
#define FONT font10x20
#endif

#ifdef FONT_12x24
#include "Cosa/Canvas/Font/Font12x24.hh"
#define FONT font12x24
#endif

#ifdef FIXEDNUMS_8x16
#include "Cosa/Canvas/Font/FixedNums8x16.hh"
#define FONT fixednums8x16
#endif

#ifdef SEGMENT_32x50
#include "Cosa/Canvas/Font/Segment32x50.hh"
#define FONT segment32x50
#endif

Textbox textbox(&tft, (Font*)&FONT);
static IOStream tftout(&textbox);

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

void setup()
{
  RTC::begin();
  uart.begin(9600);
  trace.begin(&uart);
  trace << PSTR("CosaCanvasFont: started ")
        << PSTR(DEVICE)
        << PSTR(" font ") << STRINGIFY(FONT)
        << endl;

  tft.begin();
  tft.set_canvas_color(Canvas::WHITE);
  tft.set_orientation(Canvas::LANDSCAPE);
  tft.fill_screen();

  textbox.set_canvas_color(Canvas::WHITE);
  textbox.set_text_color(Canvas::BLACK);
  textbox.set_text_port(1, 1, tft.WIDTH - 2, tft.HEIGHT - 2);
  
#ifdef ONE_CHAR
  tftout << ONE_CHAR;
#endif

#if !defined(ONE_CHAR) && !defined(CYCLE_CHARS)
  tftout << PSTR("Hello World!") << endl;
#endif

#ifdef CYCLE_CHARS
  trace << PSTR("Font has ") << (FONT.LAST-FONT.FIRST+1) 
	<< PSTR(" characters") 
	<< endl;
#endif
}

void loop()
{
#ifdef CYCLE_CHARS
#if CYCLE_CHARS == 0
  MEASURE("full character set ", 20)
#endif
  {
    for (uint16_t c = FONT.FIRST; c <= FONT.LAST; c++)
      {
        switch ((char)c)
          {
          case '\n':
          case '\r':
          case '\f':
            break;

          default:
            tftout << (char)c;
          }
        delay(CYCLE_CHARS);
      }
  }
#else
  delay(100);
#endif
}
