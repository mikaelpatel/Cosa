/**
 * @file CosaTone.ino
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
 * Demonstrate Cosa Tone class.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Tone.hh"
#include "Cosa/Note.hh"
#include "Cosa/Watchdog.hh"

#if defined(BOARD_ATTINY)
#error "CosaTone: board not supported"
#endif

void setup()
{
  Watchdog::begin();
  Tone::begin();

  // Classical telephone signal
  for (uint8_t i = 0; i < 10; i++) {
    Tone::play(Note::A4, i, 200);
    delay(200);
  }
  sleep(3);

  // MIDI nodes (49..69..89)
  for (uint8_t i = 49; i < 90; i++) {
    Tone::play(Note::MIDI(i), 5, 200);
    delay(200);
  }
  sleep(3);
}

void play(const uint16_t* freq, uint8_t volume, uint16_t duration)
{
  uint16_t f;
  while ((f = *freq++) != Note::END) {
    if (f == Note::PAUSE) delay(100);
    else Tone::play(f, volume, duration);
  }
}

void play_P(const uint16_t* freq, uint8_t volume, uint16_t duration)
{
  uint16_t f;
  while ((f = pgm_read_word(freq++)) != Note::END) {
    if (f == Note::PAUSE) delay(100);
    else Tone::play(f, volume, duration);
  }
}

void loop()
{
  // To get the spaceships' attention prior to their arrival at
  // Devil's Tower, the five notes the scientists play are G, A, F,
  // (octave lower) F, C.
  uint16_t attention[] = {
    Note::G4, Note::A4, Note::F4, Note::F3, Note::C4, Note::END
  };
  play(attention, 5, 1000);
  sleep(5);

  // When they arrive at the tower and are attempting communication,
  // the notes they play are B flat, C, A flat, (octave lower) A flat,
  // E flat.
  static const uint16_t greating[] __PROGMEM = {
    Note::Bes3, Note::C3, Note::As3, Note::As2, Note::Es3, Note::END
  };
  play_P(greating, 5, 1000);
  sleep(5);
}
