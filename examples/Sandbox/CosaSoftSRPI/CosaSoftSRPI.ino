/**
 * @file CosaSoftSRPI.ino
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
 * Demonstrate Cosa Soft N-Shift Register (Soft::SRPI) parallel
 * input port device driver.
 *
 *                        74HC165/1   (VCC)
 *                       +----U----+    |
 * (D5/PLD)---+--------1-|/PL   VCC|-16-+
 * (D4/SCL)-+-)--------2-|CP    /CE|-15-----------(GND)
 * (Q4)-----)-)--------3-|D4     D3|-14------------(Q3)
 * (Q5)-----)-)--------4-|D5     D2|-13------------(Q2)
 * (Q6)-----)-)--------5-|D6     D1|-12------------(Q1)
 * (Q7)-----)-)--------6-|D7     D0|-11------------(Q0)
 *          | |        7-|/Q7    DS|-10-----------------+
 *          | |      +-8-|GND    Q7|--9--------(SDA/D3) |
 *          | |      |   +---------+                    |
 *          | |      |      0.1uF                       |
 *          | |    (GND)-----||-------(VCC)             |
 *          | |                         |               |
 *          | |           74HC165/2     |               |
 *          | |          +----U----+    |               |
 *          | +--------1-|/PL   VCC|-16-+               |
 *          +----------2-|CP    /CE|-15-----------(GND) |
 * (Q12)---------------3-|D4     D3|-14-----------(Q11) |
 * (Q13)---------------4-|D5     D2|-13-----------(Q10) |
 * (Q14)---------------5-|D6     D1|-12------------(Q9) |
 * (Q15)---------------6-|D7     D0|-11------------(Q8) |
 *                     7-|/Q7    DS|-10                 |
 *                   +-8-|GND    Q7|--9-----------------+
 *                   |   +---------+
 *                   |      0.1uF
 *                 (GND)-----||-------(VCC)
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Soft/SRPI.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"

// Shift Register Parallel Input Port
Soft::SRPI<2> port;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaSoftSRPI: started"));
  Watchdog::begin();
}

void loop()
{
  port.update();
  uint8_t pin = port.PINS;
  while (pin--) trace << port.is_set(pin);
  trace << endl;
  sleep(2);
}
