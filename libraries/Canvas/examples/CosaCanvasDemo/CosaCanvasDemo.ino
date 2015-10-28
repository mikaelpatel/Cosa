/**
 * @file CosaCanvasDemo.ino
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
 * @section Description
 * Cosa demonstration of device driver for ST7735, ILI9341 or ILI9163.
 * Shows binding to IOStream::Device and basic drawing functions.
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
 *
 *                           ILI9163
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (GND)---------------2-|GND         |
 * (SS/D10)------------3-|CS          |
 * (RST)---------------4-|RST         |
 * (D9)----------------5-|DC          |
 * (MOSI/D11)----------6-|SDI         |
 * (SCK/D13)-----------7-|SCK         |
 * (VCC)------[330]----8-|LED         |
 *                       +------------+
 * @endcode
 *
 * Note: ILI9341 and ILI9163 signals are 3V3.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/IOStream.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

#include <Canvas.h>
#include "Canvas/Element/Textbox.hh"
#include "Canvas/Icon/arduino_icon_34x32.h"
#include "Canvas/Icon/arduino_icon_64x64.h"
#include "Canvas/Icon/arduino_icon_96x32.h"

//#define USE_TFT_ST7735
//#include <GDDRAM.h>
//#include <ST7735.h>
//ST7735 tft;

//#define USE_TFT_ILI9341
//#include <GDDRAM.h>
//#include <ILI9341.h>
//ILI9341 tft;

#define USE_TFT_ILI9163
#include <GDDRAM.h>
#include <ILI9163.h>
ILI9163 tft;

// Virtual grid image
class Grid : public Canvas::Image {
public:
  Grid(uint16_t width, uint16_t height, Canvas::color16_t color) :
    Canvas::Image(width, height),
    m_color(color),
    m_x(0),
    m_y(0)
  {}

  virtual bool read(Canvas::color16_t* buf, size_t count)
  {
    for (size_t i = 0; i < count; i++) {
      buf[i] = (m_x & m_y & 1) ? m_color : Canvas::WHITE;
      m_x += 1;
      if (m_x == WIDTH) {
	m_x = 0;
	m_y += 1;
	if (m_y == HEIGHT) m_y = 0;
      }
    }
    return (true);
  }

private:
  Canvas::color16_t m_color;
  uint16_t m_x;
  uint16_t m_y;
};

Textbox textbox(&tft);
IOStream console(&textbox);

void setup()
{
  // Initiate trace stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaCanvasDemo: started"));

  // Check amount of free memory and size of objects
  TRACE(free_memory());
  TRACE(sizeof(Trace));
  TRACE(sizeof(IOStream::Device));
  TRACE(sizeof(UART));
  TRACE(sizeof(Canvas));
  TRACE(sizeof(Font));
  TRACE(sizeof(tft));
  TRACE(sizeof(Canvas::Context));
  TRACE(sizeof(Canvas::Element));
  TRACE(sizeof(Canvas::Image));
  TRACE(sizeof(IOStream));
  TRACE(sizeof(Textbox));
  TRACE(sizeof(Grid));

  // Start the watchdog with default timeout (16 ms)
  Watchdog::begin();
  RTT::begin();

  // Initiate the display
  TRACE(tft.begin());
}

void loop()
{
  static uint8_t direction = Canvas::PORTRAIT;

  // Test#1: Fill screen
  MEASURE("test#1:fill screen:", 1) {
    tft.set_canvas_color(Canvas::WHITE);
    tft.fill_screen();
  }

  // Test#2: Use the display as an output stream
  MEASURE("test#2:output stream:", 1) {
    textbox.set_text_color(Canvas::BLUE);
    textbox.set_text_scale(1);
    textbox.set_text_port(2, 2, tft.WIDTH, tft.HEIGHT);
    console.print(PSTR("CosaCanvasDemo: started"));
    console.println();
    console.printf(PSTR("text_color(%od)\n"), textbox.get_text_color().rgb);
    console.printf(PSTR("text_scale(%d)\n"), textbox.get_text_scale());
    uint16_t x, y;
    textbox.get_cursor(x, y);
    console.printf(PSTR("cursor(x = %d, y = %d)\n"), x, y);
    textbox.set_text_color(Canvas::RED);
    textbox.set_text_scale(2);
    console.print(PSTR("  Hello\n  World"));
    console.println();
    textbox.set_text_color(Canvas::BLACK);
    textbox.set_text_scale(1);
  }
  sleep(2);

  // Test#3: Scroll text port
  tft.set_canvas_color(tft.shade(Canvas::WHITE, 50));
  tft.fill_screen();
  tft.set_canvas_color(Canvas::WHITE);
  MEASURE("test#3:scroll text mode:", 1) {
#if defined(USE_TFT_ST7735)
    tft.draw_rect(4, 4, tft.WIDTH-8, tft.HEIGHT-8);
    textbox.set_text_port(5, 5, tft.WIDTH-10, tft.HEIGHT-10);
#endif
#if defined(USE_TFT_ILI9341)
    tft.draw_rect(tft.WIDTH/4+4, tft.HEIGHT/4+4,
		  tft.WIDTH/2-8, tft.HEIGHT/2-8);
    textbox.set_text_port(tft.WIDTH/4+5, tft.HEIGHT/4+5,
			  tft.WIDTH/2-10, tft.HEIGHT/2-10);
#endif
    console.print('\f');
    console.print(&tft, 200, IOStream::hex,
		  tft.get_orientation() == Canvas::PORTRAIT ? 3 : 5);
  }
  sleep(2);

  // Test#4: Grid with draw pixel
  tft.fill_screen();
  MEASURE("test#4:draw pixel grid:", 1) {
    tft.set_pen_color(Canvas::BLACK);
    for (uint16_t x = 0; x < tft.WIDTH; x += 2) {
      for (uint16_t y = 0; y < tft.HEIGHT; y += 2) {
	tft.draw_pixel(x, y);
      }
    }
    tft.set_pen_color(Canvas::RED);
    tft.fill_rect(20, 20, 20, 20);
  }
  sleep(2);

  // Test#5: Grid with draw rectangle
  MEASURE("test#5:draw rect grid:", 1) {
    tft.set_pen_color(Canvas::BLACK);
    for (uint16_t x = 0; x < tft.WIDTH; x += 20) {
      for (uint16_t y = 0; y < tft.HEIGHT; y += 20) {
	tft.draw_rect(x, y, 20, 20);
      }
    }
  }
  sleep(2);

  // Test#6: Fill some of the rectangles
  MEASURE("test#6:fill rect grid:", 1) {
    tft.set_pen_color(Canvas::WHITE);
    for (uint16_t x = 0; x < tft.WIDTH; x += 20) {
      for (uint16_t y = x; y < tft.HEIGHT; y += 40) {
	tft.fill_rect(x + 1, y + 1, 19, 19);
      }
    }
  }
  sleep(2);

  // Test#7: Fill circles
  tft.fill_screen();
  MEASURE("test#7:draw circle grid:", 1) {
    tft.set_text_color(Canvas::WHITE);
    tft.set_text_scale(1);
    uint16_t color = Canvas::BLUE;
    for (uint16_t x = 0; x < tft.WIDTH; x += 30) {
      for (uint16_t y = 0; y < tft.HEIGHT; y += 30) {
	tft.set_pen_color(tft.shade(color, y * 100/tft.HEIGHT));
	tft.fill_circle(x, y, 12);
	tft.set_cursor(x - 2, y - 3);
	tft.draw_char('A' + (x*(tft.HEIGHT+30))/900 + y/30);
	tft.set_pen_color(Canvas::BLACK);
	tft.draw_circle(x, y, 12);
      }
      color <<= 3;
    }
  }
  sleep(2);

  // Test#8: Draw lines
  tft.set_canvas_color(tft.shade(Canvas::WHITE, 20));
  tft.fill_screen();
  MEASURE("test#8:draw lines:", 1) {
    tft.set_pen_color(Canvas::RED);
    for (uint16_t x = 0; x < tft.WIDTH; x += 6) {
      tft.draw_line(0, 0, x, tft.HEIGHT - 1);
    }
    tft.set_pen_color(Canvas::GREEN);
    for (uint16_t y = 0; y < tft.HEIGHT; y += 6) {
      tft.draw_line(0, 0, tft.WIDTH - 1, y);
    }
    tft.set_pen_color(Canvas::BLUE);
    for (uint16_t x = 0; x < tft.WIDTH; x += 6) {
      tft.draw_line(tft.WIDTH - 1, 0, x, tft.HEIGHT - 1);
    }
  }
  sleep(2);

  // Test#9: Draw more lines
  tft.set_pen_color(Canvas::BLACK);
  tft.fill_screen();
  MEASURE("test#9:draw more lines:", 1) {
    tft.set_pen_color(Canvas::YELLOW);
    for (uint16_t y = 0; y < tft.HEIGHT; y += 6) {
      tft.draw_line(0, tft.HEIGHT - 1, tft.WIDTH - 1, y);
    }
    for (uint16_t x = 0; x < tft.WIDTH; x += 6) {
      tft.draw_line(tft.WIDTH - 1, tft.HEIGHT - 1, x, 0);
    }
    for (uint16_t y = 0; y < tft.HEIGHT; y += 6) {
      tft.draw_line(tft.WIDTH - 1, tft.HEIGHT - 1, 0, y);
    }
  }
  sleep(2);

  // Test#10: Display polygons
  tft.set_canvas_color(Canvas::WHITE);
  tft.fill_screen();
  MEASURE("test#10:polygon:", 1) {
    static const int8_t polygon[] __PROGMEM = {
      100, 100,
      -100, 0,
      50, -50,
      0, 50,
      50, 50,
      -10, -10,
      0, 0
    };
    for (uint16_t x = 10; x < tft.WIDTH - 40; x += 10) {
      tft.set_pen_color(tft.shade(Canvas::GREEN, 20 + x));
      tft.set_cursor(10 + x, 50);
      tft.draw_poly_P(polygon);
    }
  }
  sleep(2);

  // Test#11: Display stroke
  tft.set_canvas_color(Canvas::WHITE);
  tft.fill_screen();
  MEASURE("test#11:stroke:", 1) {
    static const int8_t stroke[] __PROGMEM = {
      20, -100,
      20, 100,
      -30, -50,
      20, 0,
      0, 0
    };
    for (uint16_t x = 10; x < tft.WIDTH - 40; x += 10) {
      tft.set_pen_color(tft.shade(Canvas::RED, 20 + x));
      tft.set_cursor(x, 120);
      tft.draw_stroke_P(stroke);
    }
  }
  sleep(2);

  // Test#12: Display the Arduino Icons
  tft.set_canvas_color(tft.shade(Canvas::WHITE, 10));
  tft.fill_screen();
  tft.set_pen_color(tft.shade(Canvas::CYAN, 80));
  MEASURE("test#12a:draw arduino icon 34x32:", 1) {
    tft.draw_icon((tft.WIDTH-34)/2, (tft.HEIGHT-32)/2, arduino_icon_34x32);
  }
  sleep(2);
  tft.fill_screen();
  MEASURE("test#12b:draw arduino icon 64x64:", 1) {
    tft.draw_icon((tft.WIDTH-64)/2, (tft.HEIGHT-64)/2, arduino_icon_64x64);
  }
  sleep(2);
  tft.fill_screen();
  MEASURE("test#12c:draw arduino icon 96x32:", 1) {
    tft.draw_icon((tft.WIDTH-96)/2, (tft.HEIGHT-32)/2, arduino_icon_96x32);
  }
  sleep(2);

  // Test#13: Grid with draw image
  tft.fill_screen();
  MEASURE("test#13:draw grid image:", 1) {
    Grid grid(tft.WIDTH, tft.HEIGHT, Canvas::RED);
    tft.draw_image(0, 0, &grid);
  }
  sleep(2);

  // Rotate display
  direction = !direction;
  tft.set_orientation(direction);
}
