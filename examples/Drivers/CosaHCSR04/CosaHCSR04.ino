/**
 * @file CosaHCSR04.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * @section Circuit
 * Connect Arduino to HC-SR04 sensor/module; D2/EXT0 => Trig, 
 * D3 => Echo. Connect power (VCC) and ground (GND).  
 *
 * Connect Arduino to ST7735R Module (Arduino ==> HY-1.8 SPI):
 * GND ==> GND(1), VCC(5V) ==> VCC(2), RST ==> RESET(6),
 * D9 ==> A0(7), MOSI/D11 ==> SDA(8), SCK/D13 ==> SCK(9),
 * SS/D10 ==> CS(10), VCC(5V) ==> LED+(15), GND ==> LED-(16)    
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#define USE_TFT

#include "Cosa/Pins.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/Driver/HCSR04.hh"

// Heartbeat on built-in led
OutputPin ledPin(Board::LED);

// Use the ST7735R TFT display or the trace iostream
#ifdef USE_TFT

#undef putchar
#include "Cosa/IOStream.hh"
#include "Cosa/SPI/Driver/ST7735R.hh"
#include "Cosa/Canvas/Element/Textbox.hh"
#include "Cosa/Canvas/Font/Segment32x50.hh"
#include "Cosa/Canvas/Icon/arduino_icon_96x32.h"

ST7735R tft;
Textbox textbox(&tft);
IOStream cout(&textbox);
Canvas::color16_t RED, GREEN, BLUE, YELLOW, CYAN, GRAY;

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
      tft.draw_icon((tft.WIDTH-96)/2, (tft.HEIGHT-32)/2, arduino_icon_96x32);
    }
    else {
      textbox.set_cursor(4, 35);
      cout.printf_P(PSTR("%d"), distance);
    }
  }
};

Ping ping;

void tft_setup()
{
  tft.begin();
  tft.set_orientation(Canvas::LANDSCAPE);
  textbox.set_text_port(0, 0, tft.WIDTH, tft.HEIGHT);
  textbox.set_text_font(&segment32x50);
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
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaHCSR04: started"));

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
