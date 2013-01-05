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
 * This file is part of the Arduino Che Cosa project.
 */

// Enable/disable distance presentation on display (ST7735R)
#define USE_TFT

#include "Cosa/Driver/HCSR04.hh"
#include "Cosa/Watchdog.hh"

// Connect to HCSR04 to pin 2 and 3
HCSR04 ping(2, 3);

// Use the ST7735R TFT display or the trace iostream
#ifdef USE_TFT
#undef putchar
#include "Cosa/SPI/ST7735R.hh"
#include "Cosa/IOStream.hh"
ST7735R tft;
IOStream cout(&tft);
uint16_t RED, GREEN, BLUE, YELLOW, CYAN, GRAY;
#else
#include "Cosa/Pins.hh"
#include "Cosa/Trace.hh"
OutputPin ledPin(13);
#endif

void setup()
{
#ifdef USE_TFT
  // Start display and initiate text scale and port
  tft.begin();
  tft.set_text_scale(4);
  tft.set_text_port(10, 40, tft.WIDTH - 20, tft.HEIGHT - 40);
  RED = tft.shade(Canvas::RED, 75);
  YELLOW = tft.shade(Canvas::YELLOW, 75);
  GREEN = tft.shade(Canvas::GREEN, 75);
  CYAN = tft.shade(Canvas::CYAN, 75);
  GRAY = tft.shade(Canvas::WHITE, 50);
#else 
  // Start trace output stream
  trace.begin(9600, PSTR("CosaHCSR04: started"));

  // Check amount of free memory and size of instances
  TRACE(free_memory());
  TRACE(sizeof(InputPin));
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(HCSR04));
#endif

  // Start the watchdog ticks and push time events
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);

  // Attach the range module to read distance every second
  Watchdog::attach(&ping, 256);
}

void loop()
{
  // Wait and dispatch event for reading of distance
  Event event;
  Event::queue.await(&event);
  event.dispatch();
#ifndef USE_TFT
  ledPin.toggle();
#endif

  // Print the distance from the latest reading
  static uint16_t distance = 0;
  uint16_t measurement = ping.get_distance();
  if (distance != measurement) {
    distance = measurement;
#ifdef USE_TFT
    if (distance < 100)      tft.set_canvas_color(RED);
    else if (distance < 200) tft.set_canvas_color(YELLOW);
    else if (distance < 300) tft.set_canvas_color(GREEN);
    else if (distance < 400) tft.set_canvas_color(CYAN);
    else                     tft.set_canvas_color(GRAY);
    tft.fill_screen();
    tft.set_cursor(10, 10);
    cout.printf_P(PSTR("%d\nmm"), distance);
#else
    TRACE(distance);
#endif
  }
#ifndef USE_TFT
  ledPin.toggle();
#endif
}
