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
  for (uint8_t i = 0; i < 10; i++) {
    Tone::play(Note::A4, 5, 200);
    delay(200);
  }
  sleep(5);
}

void play(const uint16_t* freq, uint8_t volume)
{
  uint16_t f;
  while ((f = *freq++) != 0) Tone::play(f, volume, 1000);
}

void play_P(const uint16_t* freq, uint8_t volume)
{
  uint16_t f;
  while ((f = pgm_read_word(freq++)) != 0) Tone::play(f, volume, 1000);
}

void loop()
{
  // To get the spaceships' attention prior to their arrival at
  // Devil's Tower, the five notes the scientists play are G, A, F,
  // (octave lower) F, C.
  uint16_t ping[] = {
    Note::G4, Note::A4, Note::F4, Note::F3, Note::C4, 0
  };
  play(ping, 5);
  sleep(5);

  // When they arrive at the tower and are attempting communication,
  // the notes they play are B flat, C, A flat, (octave lower) A flat,
  // E flat.
  static const uint16_t pong[] PROGMEM = {
    Note::Bes4, Note::C4, Note::As4, Note::As3, Note::Es4, 0
  };
  play_P(pong, 5);
  sleep(5);
}
