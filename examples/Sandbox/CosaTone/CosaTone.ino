/**
 * @file CosaTone.ino
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
 * @section Description
 * Demonstrate Cosa Tone class. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Tone.hh"
#include "Cosa/Watchdog.hh"

#if defined(BOARD_ATTINY)
#error "CosaTone: board not supported"
#endif

void setup()
{
  Watchdog::begin();
  Tone::begin();
}

void loop()
{
  uint8_t volume = 7;

  // To get the spaceships' attention prior to their arrival at
  // Devil's Tower, the five notes the scientists play are G, A, F,
  // (octave lower) F, C.
  Tone::play(Tone::G4, volume, 1000);
  Tone::play(Tone::A4, volume, 1000);
  Tone::play(Tone::F4, volume, 1000);
  Tone::play(Tone::F3, volume, 1000);
  Tone::play(Tone::C4, volume, 1000);
  sleep(5);

  // When they arrive at the tower and are attempting communication,
  // the notes they play are B flat, C, A flat, (octave lower) A flat,
  // E flat.
  volume /= 2;
  Tone::play(Tone::Bes4, volume, 1000);
  Tone::play(Tone::C4, volume, 1000);
  Tone::play(Tone::As4, volume, 1000);
  Tone::play(Tone::As3, volume, 1000);
  Tone::play(Tone::Es4, volume, 1000);
  sleep(10);
}
