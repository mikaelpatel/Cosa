/**
 * @file Rotary.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2011, Ben Buxton (Rotary encoder state machine)
 * Copyright (C) 2013-2015, Mikael Patel (Cosa port and extension)
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

#include "Rotary.hh"

/*
 * Copyright 2011 Ben Buxton. Licenced under the GNU GPL Version 3.
 * Contact: bb@cactii.net
 *
 * A typical mechanical rotary encoder emits a two bit gray code
 * on 3 output pins. Every step in the output (often accompanied
 * by a physical 'click') generates a specific sequence of output
 * codes on the pins.
 *
 * There are 3 pins used for the rotary encoding - one common and
 * two 'bit' pins.
 *
 * The following is the typical sequence of code on the output when
 * moving from one step to the next:
 *
 *   Position   Bit1   Bit2
 *   ----------------------
 *     Step1     0      0
 *      1/4      1      0
 *      1/2      1      1
 *      3/4      0      1
 *     Step2     0      0
 *
 * From this table, we can see that when moving from one 'click' to
 * the next, there are 4 changes in the output code.
 *
 * - From an initial 0 - 0, Bit1 goes high, Bit0 stays low.
 * - Then both bits are high, halfway through the step.
 * - Then Bit1 goes low, but Bit2 stays high.
 * - Finally at the end of the step, both bits return to 0.
 *
 * Detecting the direction is easy - the table simply goes in the other
 * direction (read up instead of down).
 *
 * To decode this, we use a simple state machine. Every time the output
 * code changes, it follows state, until finally a full steps worth of
 * code is received (in the correct order). At the final 0-0, it returns
 * a value indicating a step in one direction or the other.
 *
 * It's also possible to use 'half-step' mode. This just emits an event
 * at both the 0-0 and 1-1 positions. This might be useful for some
 * encoders where you want to detect all positions.
 *
 * If an invalid state happens (for example we go from '0-1' straight
 * to '1-0'), the state machine resets to the start until 0-0 and the
 * next valid codes occur.
 *
 * The biggest advantage of using a state machine over other algorithms
 * is that this has inherent debounce built in. Other algorithms emit
 * spurious output with switch bounce, but this one will simply flip
 * between sub-states until the bounce settles, then continue along the
 * state machine.
 *
 * A side effect of debounce is that fast rotations can cause steps to
 * be skipped. By not requiring debounce, fast rotations can be accurately
 * measured.
 *
 * Another advantage is the ability to properly handle bad state, such
 * as due to EMI, etc. It is also a lot simpler than others - a static
 * state table and less than 10 lines of logic.
 *
 * @see also
 * http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
 */

// No complete step yet
#define DIR_NONE 0x0
// Clockwise step
#define DIR_CW 0x10
// Anti-clockwise step
#define DIR_CCW 0x20

// Half-cycle states
#define R_START 0x0
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5

/**
 * Half-cycle state table (emits a code at 00 and 11)
 *
 * @dot
 * digraph half_cycle {
 *   R_START -> R_START_M [label="00"];
 *   R_START -> R_CW_BEGIN [label="01"];
 *   R_START -> R_CCW_BEGIN [label="10"];
 *   R_START -> R_START [label="11"];
 *   R_CCW_BEGIN -> R_START_M [label="00, DIR_CCW"];
 *   R_CCW_BEGIN -> R_START [label="01"];
 *   R_CCW_BEGIN -> R_CCW_BEGIN [label="10"];
 *   R_CCW_BEGIN -> R_START [label="11"];
 *   R_CW_BEGIN -> R_START_M [label="00, DIR_CW"];
 *   R_CW_BEGIN -> R_CW_BEGIN [label="01"];
 *   R_CW_BEGIN -> R_START [label="10"];
 *   R_CW_BEGIN -> R_START [label="11"];
 *   R_START_M -> R_START_M [label="00"];
 *   R_START_M -> R_CCW_BEGIN_M [label="01"];
 *   R_START_M -> R_CW_BEGIN_M [label="10"];
 *   R_START_M -> R_START [label="11"];
 *   R_CW_BEGIN_M -> R_START_M [label="00"];
 *   R_CW_BEGIN_M -> R_START_M [label="01"];
 *   R_CW_BEGIN_M -> R_CW_BEGIN_M [label="10"];
 *   R_CW_BEGIN_M -> R_START [label="11, DIR_CW"];
 *   R_CCW_BEGIN_M -> R_START_M [label="00"];
 *   R_CCW_BEGIN_M -> R_CCW_BEGIN_M [label="01"];
 *   R_CCW_BEGIN_M -> R_START_M [label="10"];
 *   R_CCW_BEGIN_M -> R_START [label="10, DIR_CCW"];
 * }
 * @enddot
 */
const uint8_t Rotary::Encoder::half_cycle_table[6][4] __PROGMEM = {
// R_START (00)
  {R_START_M,            R_CW_BEGIN,     R_CCW_BEGIN,  R_START},
// R_CCW_BEGIN
  {R_START_M | DIR_CCW,  R_START,        R_CCW_BEGIN,  R_START},
// R_CW_BEGIN
  {R_START_M | DIR_CW,   R_CW_BEGIN,     R_START,      R_START},
// R_START_M (11)
  {R_START_M,            R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START},
// R_CW_BEGIN_M
  {R_START_M,            R_START_M,      R_CW_BEGIN_M, R_START | DIR_CW},
// R_CCW_BEGIN_M
  {R_START_M,            R_CCW_BEGIN_M,  R_START_M,    R_START | DIR_CCW},
};
#undef R_CCW_BEGIN

// Full-cycle states
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

/**
 * Full-cycle state table (emits a code at 00 only)
 *
 * @dot
 * digraph full_cycle {
 *   R_START -> R_START [label="00"];
 *   R_START -> R_CW_BEGIN [label="01"];
 *   R_START -> R_CCW_BEGIN [label="10"];
 *   R_START -> R_START [label="11"];
 *   R_CW_FINAL -> R_CW_NEXT [label="00"];
 *   R_CW_FINAL -> R_START [label="01"];
 *   R_CW_FINAL -> R_CW_FINAL [label="10"];
 *   R_CW_FINAL -> R_START [label="11,DIR_CW"];
 *   R_CW_BEGIN -> R_CW_NEXT [label="00"];
 *   R_CW_BEGIN -> R_CW_BEGIN [label="01"];
 *   R_CW_BEGIN -> R_START [label="10"];
 *   R_CW_BEGIN -> R_START [label="11"];
 *   R_CW_NEXT -> R_CW_NEXT [label="00"];
 *   R_CW_NEXT -> R_CW_BEGIN [label="01"];
 *   R_CW_NEXT -> R_CW_FINAL [label="10"];
 *   R_CW_NEXT -> R_START [label="11"];
 *   R_CCW_BEGIN -> R_CCW_NEXT [label="00"];
 *   R_CCW_BEGIN -> R_START [label="01"];
 *   R_CCW_BEGIN -> R_CCW_BEGIN [label="10"];
 *   R_CCW_BEGIN -> R_START [label="11"];
 *   R_CCW_FINAL -> R_CCW_NEXT [label="00"];
 *   R_CCW_FINAL -> R_CCW_FINAL [label="01"];
 *   R_CCW_FINAL -> R_START [label="10"];
 *   R_CCW_FINAL -> R_START [label="01,DIR_CCW"];
 *   R_CCW_NEXT -> R_CCW_NEXT [label="00"];
 *   R_CCW_NEXT -> R_CCW_FINAL [label="01"];
 *   R_CCW_NEXT -> R_CCW_BEGIN [label="10"];
 *   R_CCW_NEXT -> R_START [label="11"];
 * }
 * @enddot
 */
const uint8_t Rotary::Encoder::full_cycle_table[7][4] __PROGMEM = {
// R_START
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
// R_CW_FINAL
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
// R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
// R_CW_NEXT
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
// R_CCW_BEGIN
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
// R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
// R_CCW_NEXT
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};

void
Rotary::Encoder::SignalPin::on_interrupt(uint16_t arg)
{
  UNUSED(arg);
  Rotary::Encoder::Direction change = m_encoder->detect();
  if (change) Event::push(Event::CHANGE_TYPE, m_encoder, change);
}

Rotary::Encoder::Direction
Rotary::Encoder::detect()
{
  uint8_t pins = ((m_dt.is_set() << 1) | m_clk.is_set());
  if (m_mode == FULL_CYCLE)
    m_state = pgm_read_byte(&full_cycle_table[m_state & 0xf][pins]);
  else
    m_state = pgm_read_byte(&half_cycle_table[m_state & 0xf][pins]);
  return ((Direction) (m_state & 0xf0));
}

