/**
 * @file Cosa/Note.hh
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_NOTE_HH
#define COSA_NOTE_HH

#include <math.h>

/**
 * C0-C8 scale, suffix -s/is of sharp, and -s/es for flat,
 * Scandinavian/Dutch naming convention.
 *
 * @section Usage
 *    Tone::play(Note::C4);
 *
 * @section References
 * 1. Frequencies for equal-tempered scale,
 *    http://www.phy.mtu.edu/~suits/notefreqs.html
 * 2. Wiki, Note, http://en.wikipedia.org/wiki/Note
 */
class Note {
public:
  enum {
    END = 0,
    PAUSE = 1,

    C0 = 16,
    Cis0 = 17,
    Des0 = 17,
    D0 = 18,
    Dis0 = 19,
    Es0 = 19,
    E0 = 21,
    F0 = 22,
    Fis0 = 23,
    Ges0 = 23,
    G0 = 25,
    Gis0 = 26,
    As0 = 26,
    A0 = 28,
    Ais0 = 29,
    Bes0 = 29,
    B0 = 31,

    C1 = 33,
    Cis1 = 35,
    Des1 = 35,
    D1 = 37,
    Dis1 = 39,
    Es1 = 39,
    E1 = 41,
    F1 = 44,
    Fis1 = 46,
    Ges1 = 46,
    G1 = 49,
    Gis1 = 52,
    As1 = 52,
    A1 = 55,
    Ais1 = 58,
    Bes1 = 58,
    B1 = 62,

    C2 = 65,
    Cis2 = 69,
    Des2 = 69,
    D2 = 73,
    Dis2 = 78,
    Es2 = 78,
    E2 = 82,
    F2 = 87,
    Fis2 = 93,
    Ges2 = 93,
    G2 = 98,
    Gis2 = 104,
    As2 = 104,
    A2 = 110,
    Ais2 = 117,
    Bes2 = 117,
    B2 = 123,

    C3 = 131,
    Cis3 = 139,
    Des3 = 139,
    D3 = 147,
    Dis3 = 156,
    Es3 = 156,
    E3 = 165,
    F3 = 175,
    Fis3 = 185,
    Ges3 = 185,
    G3 = 196,
    Gis3 = 208,
    As3 = 208,
    A3 = 220,
    Ais3 = 233,
    Bes3 = 233,
    B3 = 247,

    C4 = 262,
    Cis4 = 277,
    Des4 = 277,
    D4 = 294,
    Dis4 = 311,
    Es4 = 311,
    E4 = 330,
    F4 = 349,
    Fis4 = 370,
    Ges4 = 370,
    G4 = 392,
    Gis4 = 415,
    As4 = 415,
    A4 = 440,
    Ais4 = 466,
    Bes4 = 466,
    B4 = 494,

    C5 = 523,
    Cis5 = 554,
    Des5 = 554,
    D5 = 587,
    Dis5 = 622,
    Es5 = 622,
    E5 = 659,
    F5 = 698,
    Fis5 = 740,
    Ges5 = 740,
    G5 = 784,
    Gis5 = 831,
    As5 = 831,
    A5 = 880,
    Ais5 = 932,
    Bes5 = 932,
    B5 = 988,

    C6 = 1047,
    Cis6 = 1109,
    Des6 = 1109,
    D6 = 1175,
    Dis6 = 1245,
    Es6 = 1245,
    E6 = 1319,
    F6 = 1397,
    Fis6 = 1480,
    Ges6 = 1480,
    G6 = 1568,
    Gis6 = 1661,
    As6 = 1661,
    A6 = 1760,
    Ais6 = 1865,
    Bes6 = 1865,
    B6 = 1976,

    C7 = 2093,
    Cis7 = 2217,
    Des7 = 2217,
    D7 = 2349,
    Dis7 = 2489,
    Es7 = 2489,
    E7 = 2637,
    F7 = 2794,
    Fis7 = 2960,
    Ges7 = 2960,
    G7 = 3136,
    Gis7 = 3322,
    As7 = 3322,
    A7 = 3520,
    Ais7 = 3729,
    Bes7 = 3729,
    B7 = 3951,

    C8 = 4186,
    Cis8 = 4435,
    Des8 = 4435,
    D8 = 4699,
    Dis8 = 4978,
    Es8 = 4978,
    E8 = 5274,
    F8 = 5588,
    Fis8 = 5920,
    Ges8 = 5920,
    G8 = 6272,
    Gis8 = 6645,
    As8 = 6645,
    A8 = 7040,
    Ais8 = 7459,
    Bes8 = 7459,
    B8 = 7902
  };

  /**
   * Return frequency for given MIDI note number.
   * @param[in] n note number.
   * @return frequency.
   */
  static uint16_t MIDI(uint8_t n)
  {
    return (round(440.0 * (pow(2.0, (n - 69.0) / 12))));
  }
};
#endif

