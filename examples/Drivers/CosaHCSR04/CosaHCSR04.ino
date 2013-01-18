/**
 * @file CosaHCSR04.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * @section Description
 * Cosa demonstration of the Ultrasonic range module 
 * HC-SR04/US-020 device driver. 
 *
 * @section Configuration
 *  USE_TFT	distance presentation on display (ST7735R)
 *
 * This file is part of the Arduino Che Cosa project.
 */

#define USE_TFT

#include "Cosa/Pins.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Memory.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/Driver/HCSR04.hh"

// Heartbeat on built-in led
OutputPin ledPin(Board::LED);

// Use the ST7735R TFT display or the trace iostream
#ifdef USE_TFT

#undef putchar
#include "Cosa/IOStream.hh"
#include "Cosa/SPI/ST7735R.hh"
#include "Cosa/Canvas/Element/Textbox.hh"
#include "Cosa/Canvas/Font/FixedNums8x16.hh"
#include "Cosa/Canvas/Icon/arduino_icon_96x32.h"

ST7735R tft;
Textbox textbox(&tft);
IOStream cout(&textbox);
uint16_t RED, GREEN, BLUE, YELLOW, CYAN, GRAY;

class Ping : private HCSR04 {
public:
  Ping() : HCSR04(Board::D2, Board::D3) 
  {
    Watchdog::attach(this, 256);
  }
  virtual void on_change(uint16_t distance) 
  {
    if      (distance < 100) tft.set_canvas_color(RED);
    else if (distance < 200) tft.set_canvas_color(YELLOW);
    else if (distance < 300) tft.set_canvas_color(GREEN);
    else if (distance < 400) tft.set_canvas_color(CYAN);
    else                     tft.set_canvas_color(GRAY);
    tft.fill_screen();
    if (distance < 50 || distance > 3000) {
      uint8_t saved = tft.get_text_scale();
      tft.set_text_scale(1);
      tft.draw_icon((tft.WIDTH-96)/2, (tft.HEIGHT-32)/2, arduino_icon_96x32);
      tft.set_text_scale(saved);
    }
    else {
      textbox.set_cursor(10, 35);
      cout.printf_P(PSTR("%d"), distance);
    }
  }
};

Ping ping;

void tft_setup()
{
  tft.begin();
  textbox.set_text_scale(4);
  textbox.set_text_font(&fixednums8x16);
  tft.set_orientation(Canvas::LANDSCAPE);
  RED = tft.shade(Canvas::RED, 75);
  YELLOW = tft.shade(Canvas::YELLOW, 75);
  GREEN = tft.shade(Canvas::GREEN, 75);
  CYAN = tft.shade(Canvas::CYAN, 75);
  GRAY = tft.shade(Canvas::WHITE, 50);
}

#else

HCSR04 ping(2, 3);

#endif

void setup()
{
  // Start trace output stream
  trace.begin(9600, PSTR("CosaHCSR04: started"));

  // Check amount of free memory and size of instances
  TRACE(free_memory());
  TRACE(sizeof(InputPin));
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(HCSR04));

  // Start the watchdog ticks and push time events
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);

#ifdef USE_TFT
  // Start display and initiate text scale and port
  tft_setup();
#else 
  // Attach the range module to read distance every 1/4 second
  Watchdog::attach(&ping, 256);
#endif
}

void loop()
{
  // Wait and dispatch event for reading of distance
  Event event;
  Event::queue.await(&event);
  event.dispatch();

  // Print the distance from the latest reading
#ifndef USE_TFT
  ledPin.toggle();
  static uint16_t distance = 0;
  uint16_t measurement = ping.get_distance();
  if (distance != measurement) {
    distance = measurement;
    TRACE(distance);
  }
  ledPin.toggle();
#endif
}
