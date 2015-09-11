/**
 * @file CosaCanvasHCSR04.ino
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
 * Cosa demonstration of the Ultrasonic range module HC-SR04 driver.
 * Display splash screen with Arduino icon and range in milli-meters.
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
 *                           HC-SR04
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (D2)----------------2-|TRIG        |
 * (D3) ---------------3-|ECHO        |
 * (GND)---------------4-|GND         |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <GDDRAM.h>
#include <ST7735.h>
#include <HCSR04.h>

#include <Canvas.h>
#include "Canvas/Element/Textbox.hh"
#include "Canvas/Icon/arduino_icon_96x32.h"

#include <Font.h>
#include "System5x7.hh"
#include "Segment32x50.hh"

#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"

ST7735 tft;
Textbox textbox(&tft);
IOStream cout(&textbox);

class Ping : public HCSR04 {
private:
public:
  Ping(Job::Scheduler* scheduler) : HCSR04(scheduler, Board::D2, Board::D3) {}
  virtual void on_change(uint16_t distance);
};

void
Ping::on_change(uint16_t distance)
{
  tft.fill_screen();
  tft.draw_roundrect(5, 5, tft.WIDTH - 10, tft.HEIGHT - 10, 8);
  if (distance > 50 && distance < 3000) {
    Canvas::color16_t color;
    if      (distance < 100) color = Canvas::YELLOW;
    else if (distance < 200) color = Canvas::GREEN;
    else if (distance < 300) color = Canvas::CYAN;
    else if (distance < 400) color = Canvas::BLUE;
    else if (distance < 500) color = Canvas::MAGENTA;
    else if (distance < 600) color = Canvas::RED;
    else                     color = Canvas::BLACK;
    uint8_t digits = (distance < 100 ? 2 : (distance < 1000 ? 3 : 4));
    uint16_t width = (segment32x50.WIDTH + system5x7.SPACING) * digits;
    textbox.set_text_color(color);
    textbox.set_cursor((tft.WIDTH - width)/2, (tft.HEIGHT - 50)/2);
    cout << distance;
  }
  else {
    uint16_t width = (system5x7.WIDTH + system5x7.SPACING) * 18;
    tft.draw_icon((tft.WIDTH - 96)/2, (tft.HEIGHT - 50)/2, arduino_icon_96x32);
    tft.set_cursor((tft.WIDTH - width)/2, 80);
    tft.draw_string(PSTR("Cosa Canvas HCSR04"));
  }
}

OutputPin ledPin(Board::LED);
Watchdog::Scheduler scheduler;
Ping ping(&scheduler);

void setup()
{
  // Start the watchdog ticks
  Watchdog::begin();

  // Attach the range module to read distance every 1/4 second
  ping.schedule(256);

  // Start display and initiate text scale and port. Draw splash screen
  tft.begin();
  tft.set_orientation(Canvas::LANDSCAPE);
  textbox.set_text_port(0, 0, tft.WIDTH, tft.HEIGHT);
  textbox.set_text_font(&segment32x50);
  ping.on_change(0);
}

void loop()
{
  // Wait for events (low power mode) and dispatch
  Event::service();
}
