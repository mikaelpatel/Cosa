/**
 * @file Commands.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Commands.h"
#include "Cosa/RTC.hh"
#include "Cosa/Time.hh"
#include "Cosa/Tone.hh"
#include "Cosa/Memory.h"
#include "Cosa/AnalogPin.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"

// Digital and Analog Pin map (pin index => Cosa pin identity).
// These maps are need as internally Cosa works with pin symbols and 
// not pin numbers.
static const Board::DigitalPin digital_pin_map[] __PROGMEM = {
  Board::D0, 
  Board::D1, 
  Board::D2, 
  Board::D3, 
  Board::D4, 
  Board::D5, 
  Board::D6, 
  Board::D7, 
  Board::D8, 
  Board::D9, 
  Board::D10, 
  Board::D11, 
  Board::D12, 
  Board::D13,
  Board::D14,
  Board::D15,
  Board::D16,
  Board::D17,
  Board::D18,
  Board::D19
};

static const Board::AnalogPin analog_pin_map[] __PROGMEM = {
  Board::A0, 
  Board::A1, 
  Board::A2, 
  Board::A3, 
  Board::A4, 
  Board::A5
};

static const char ANALOGREAD_NAME[] __PROGMEM = 
  "analogread";
static const char ANALOGREAD_HELP[] __PROGMEM = 
  "all|ALL|PIN.. -- read analog pins";
static int analogread_action(int argc, char* argv[])
{
  if (argc == 2 && (strcmp_P(argv[1], PSTR("all")) == 0 ||
		    strcmp_P(argv[1], PSTR("ALL")) == 0)) {
    bool chap = argv[1][0] == 'A';
    for (uint8_t ix = 0; ix < membersof(analog_pin_map); ix++) {
      Board::AnalogPin pin;
      pin = (Board::AnalogPin) pgm_read_byte(&analog_pin_map[ix]);
      if (chap) ios << 'A' ; else ios << 'a';
      ios << ix << '=' << AnalogPin::sample(pin) << endl;
    }
  }
  else {
    bool multi = argc > 2;
    for (int i = 1; i < argc; i++) {
      char* name = argv[i];
      if (name[0] != 'a' && name[0] != 'A') return (-1);
      char* sp;
      uint32_t ix = strtoul(name + 1, &sp, 10);
      if (*sp != 0 || ix >= membersof(analog_pin_map)) return (-1);
      Board::AnalogPin pin;
      pin = (Board::AnalogPin) pgm_read_byte(&analog_pin_map[ix]);
      if (multi) ios << name << '=';
      ios << AnalogPin::sample(pin) << endl;
    }
  }
  return (0);
}

static const char ARGS_NAME[] __PROGMEM = 
  "args";
static const char ARGS_HELP[] __PROGMEM = 
  "OPTS ARGS -- display options and arguments";
static int args_action(int argc, char* argv[])
{
  char* option;
  char* value;
  int ix;
  while ((ix = shell.get(option, value)) == 0)
    ios << PSTR("option: ") << option << PSTR(" value: ") << value << endl;
  while (ix < argc)
    ios << PSTR("argument: ") << argv[ix++] << endl;
  return (0);
}
   
#define LF "\n"

static const char BLINK_NAME[] __PROGMEM = 
  "blink";
static const char BLINK_HELP[] __PROGMEM = 
  "MS -- turn led on and off";
static const char BLINK_SCRIPT[] __PROGMEM = 
  SHELL_SCRIPT_MAGIC					       
  "echo -n $1 \"ms:led on..\"" LF 
  "led on" LF
  "delay $1" LF 
  "echo -n \"off..\"" LF 
  "led off" LF
  "delay $1" LF
  "echo -n \"on..\"" LF
  "led on" LF
  "delay $1" LF
  "echo -n \"off..\"" LF
  "led off" LF
  "delay $1" LF
  "echo -n \"on..\"" LF
  "led on" LF
  "delay $1" LF 
  "echo \"off\"" LF 
  "led off";
#define blink_action (Shell::action_fn) BLINK_SCRIPT

static const char DATE_NAME[] __PROGMEM = 
  "date";
static const char DATE_HELP[] __PROGMEM = 
  "[YEAR-MON-DAY HOUR:MIN:SEC] -- print or set the system date and time";
static int date_action(int argc, char* argv[])
{
  if (argc == 3) {
    uint32_t value;
    time_t now;
    char* sp;
    value = strtoul(argv[1], &sp, 10);
    if (*sp != '-' || value < 2000 || value > 2099) return (-1);
    value -= 2000;
    now.year = value;
    value = strtoul(sp + 1, &sp, 10);
    if (*sp != '-' || value < 1 || value > 12) return (-1);
    now.month = value;
    value = strtoul(sp + 1, &sp, 10);
    if (*sp != 0 || value < 1 || value > 31) return (-1);
    now.date = value;
    value = strtoul(argv[2], &sp, 10);
    if (*sp != ':' || value > 23) return (-1);
    now.hours = value;
    value = strtoul(sp + 1, &sp, 10);
    if (*sp != ':' || value > 60) return (-1);
    now.minutes = value;
    value = strtoul(sp + 1, &sp, 10);
    if (*sp != 0 || value > 60) return (-1);
    now.seconds = value;
    now.to_bcd();
    RTC::time(now);
  }
  else if (argc != 1) return (-1);
  time_t now(RTC::seconds());
  ios << now << endl;
  return (0);
}

static const char DELAY_NAME[] __PROGMEM = 
  "delay";
static const char DELAY_HELP[] __PROGMEM = 
  "MS -- delay for milliseconds";
static int delay_action(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  char* sp;
  uint32_t ms = strtoul(argv[1], &sp, 10);
  if (*sp != 0) return (-1);
  delay(ms);
  return (0);
}

static const char DIGITALREAD_NAME[] __PROGMEM = 
  "digitalread";
static const char DIGITALREAD_HELP[] __PROGMEM = 
  "all|ALL|[led] PIN.. -- read digital pins";
static int digitalread_action(int argc, char* argv[])
{
  if (argc == 2 && (strcmp_P(argv[1], PSTR("all")) == 0 ||
		    strcmp_P(argv[1], PSTR("ALL")) == 0)) {
    bool chap = argv[1][0] == 'A';
    for (uint8_t ix = 0; ix < membersof(digital_pin_map); ix++) {
      Board::DigitalPin pin;
      pin = (Board::DigitalPin) pgm_read_byte(&digital_pin_map[ix]);
      if (chap) ios << 'D' ; else ios << 'd';
      ios << ix << '=' << InputPin::read(pin) << endl;
    }
  }
  else {
    bool multi = argc > 2;
    for (int i = 1; i < argc; i++) {
      Board::DigitalPin pin;
      char* name = argv[i];
      if (strcmp_P(argv[1], PSTR("led")) == 0) {
	pin = Board::LED;
      } else if (name[0] == 'd' || name[0] == 'D') {
	char* sp;
	uint32_t ix = strtoul(name + 1, &sp, 10);
	if (*sp != 0 || ix >= membersof(digital_pin_map)) return (-1);
	pin = (Board::DigitalPin) pgm_read_byte(&digital_pin_map[ix]);
      }
      else return (-1);
      if (multi) ios << name << '=';
      ios << InputPin::read(pin) << endl;
    }
  }
  return (0);
}

static const char DIGITALTOGGLE_NAME[] __PROGMEM = 
  "digitaltoggle";
static const char DIGITALTOGGLE_HELP[] __PROGMEM = 
  "led|PIN -- toggle digital pin";
static int digitaltoggle_action(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  Board::DigitalPin pin;
  char* name = argv[1];
  if (strcmp_P(name, PSTR("led")) == 0) {
    pin = Board::LED;
  }
  else {
    if (name[0] != 'd' && name[0] != 'D') return (-1);
    char* sp;
    uint32_t ix = strtoul(name + 1, &sp, 10);
    if (*sp != 0 || ix >= membersof(digital_pin_map)) return (-1);
    pin = (Board::DigitalPin) pgm_read_byte(&digital_pin_map[ix]);
  }
  OutputPin::toggle(pin);
  ios << InputPin::read(pin) << endl;
  return (0);
}

static const char DUMP_NAME[] __PROGMEM = 
  "dump";
static const char DUMP_HELP[] __PROGMEM = 
  "[-b|-d] ADDR [SIZE] -- dump memory block";
static int dump_action(int argc, char* argv[])
{
  IOStream::Base base = IOStream::hex;
  uint32_t addr = 0L;
  size_t size = 256;
  char* option;
  char* value;
  char* sp;
  int ix;
  while ((ix = shell.get(option, value)) == 0) {
    if (strcmp_P(option, PSTR("b")) == 0)
      base = IOStream::bin;
    else if (strcmp_P(option, PSTR("d")) == 0)
      base = IOStream::dec;
    else 
      return (-1);
  }
  if (ix < argc) {
    addr = strtoul(argv[ix++], &sp, 16);
    if (*sp != 0) return (-1);
  }
  if (ix < argc) {
    size = strtoul(argv[ix++], &sp, 10);
    if (*sp != 0) return (-1);
  }
  if (ix != argc) return (-1);
  ios.print(addr, (void*) addr, size, base);
  return (0);
}

static const char ECHO_NAME[] __PROGMEM = 
  "echo";
static const char ECHO_HELP[] __PROGMEM = 
  "[-n] STRING.. -- display a line of text";
static int echo_action(int argc, char* argv[])
{
  bool newline = true;
  char* option;
  char* value;
  int ix;
  while ((ix = shell.get(option, value)) == 0)
    if (strcmp_P(option, PSTR("n")) == 0)
      newline = false;
    else return (-1);
  if (ix == argc) return (0);
  ios << argv[ix++];
  while (ix < argc) ios << ' ' << argv[ix++];
  if (newline) ios << endl;
  return (0);
}

static const char HELP_NAME[] __PROGMEM = 
  "help";
static const char HELP_HELP[] __PROGMEM = 
  "-- list command help";
static int help_action(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1) return (-1);
  return (shell.help(ios));
}

static const char IDLE_NAME[] __PROGMEM = 
  "idle";
static const char IDLE_HELP[] __PROGMEM = 
  "-- display idle time";
static int idle_action(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1) return (-1);
  ios << (idle * 100.0) / RTC::micros() << '%' << endl;
  return (0);
}

static const char LED_NAME[] __PROGMEM = 
  "led";
static const char LED_HELP[] __PROGMEM = 
  "on|off -- turn led on or off";
static int led_action(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  if (strcmp_P(argv[1], PSTR("on")) == 0) 
    OutputPin::write(Board::LED, 1);
  else if (strcmp_P(argv[1], PSTR("off")) == 0) 
    OutputPin::write(Board::LED, 0);
  else return (-1);
  return (0);
}

static const char MEMORY_NAME[] __PROGMEM = 
  "memory";
static const char MEMORY_HELP[] __PROGMEM = 
  "-- display amout of free memory";
static int memory_action(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1) return (-1);
  ios << free_memory() << PSTR(" byte") << endl;
  return (0);
}

static const char MICROS_NAME[] __PROGMEM = 
  "micros";
static const char MICROS_HELP[] __PROGMEM = 
  "-- clock in micro-seconds";
static int micros_action(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1) return (-1);
  ios << RTC::micros() << endl;
  return (0);
}

static const char MILLIS_NAME[] __PROGMEM = 
  "millis";
static const char MILLIS_HELP[] __PROGMEM = 
  "-- clock in milli-seconds";
static int millis_action(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1) return (-1);
  ios << RTC::millis() << endl;
  return (0);
}

static const char REPEAT_NAME[] __PROGMEM = 
  "repeat";
static const char REPEAT_HELP[] __PROGMEM = 
  "COUNT [DELAY] COMMAND -- repeat command line";
static int repeat_action(int argc, char* argv[])
{
  if (argc < 3) return (-1);
  uint8_t fx = 3;
  char* sp;
  uint16_t count = strtoul(argv[1], &sp, 10);
  if (*sp != 0 || count == 0) return (-1);
  uint32_t ms = strtoul(argv[2], &sp, 10);
  if (*sp != 0) {
    ms = 0L;
    fx = 2;
  }
  const size_t BUF_MAX = 64;
  char buf[BUF_MAX];
  while (--count) {
    buf[0] = 0;
    strcat(buf, argv[fx]);
    for (uint8_t ix = fx + 1; ix < argc; ix++) {
      strcat(buf, " ");
      strcat(buf, argv[ix]);
    }
    if (shell.execute(buf) != 0) return (-1);
    if (ms != 0) delay(ms);
  }
  return (0);
}

static const char SECONDS_NAME[] __PROGMEM = 
  "seconds";
static const char SECONDS_HELP[] __PROGMEM = 
  "-- clock in seconds";
static int seconds_action(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1) return (-1);
  ios << RTC::seconds() << endl;
  return (0);
}

static const char STTY_NAME[] __PROGMEM = 
  "stty";
static const char STTY_HELP[] __PROGMEM = 
  "[eol=CR|LF|CRLF] -- set tty mode";
static int stty_action(int argc, char* argv[])
{
  UNUSED(argv);
  char* option;
  char* value;
  int ix;
  while ((ix = shell.get(option, value)) == 0) {
    if (strcmp_P(option, PSTR("eol")) == 0) {
      if (strcmp_P(value, PSTR("CR")) == 0) 
	ios.get_device()->set_eol(IOStream::CR_MODE);
      else if (strcmp_P(value, PSTR("LF")) == 0) 
	ios.get_device()->set_eol(IOStream::LF_MODE);
      else if (strcmp_P(value, PSTR("CRLF")) == 0) 
	ios.get_device()->set_eol(IOStream::CRLF_MODE);
      else return (-1);
    }
  }
  if (ix != argc) return (-1);
  return (0);
}

static const char TONE_NAME[] __PROGMEM = 
  "tone";
static const char TONE_HELP[] __PROGMEM = 
  "FREQ [VOLUME [DURATION]] -- play tone";
static int tone_action(int argc, char* argv[])
{
  if (argc < 2 || argc > 4) return (-1);
  char* sp;
  uint16_t freq = strtoul(argv[1], &sp, 10);
  if (*sp != 0) return (-1);
  uint8_t volume = Tone::VOLUME_MAX / 2;
  if (argc > 2) {
    volume = strtoul(argv[2], &sp, 10);
    if (*sp != 0) return (-1);
  }
  uint16_t duration = 0;
  if (argc > 3) {
    duration = strtoul(argv[3], &sp, 10);
    if (*sp != 0) return (-1);
  }
  Tone::play(freq, volume, duration);
  return (0);
}

static const Shell::command_t command_tab[] __PROGMEM = {
  { ANALOGREAD_NAME, ANALOGREAD_HELP, analogread_action },
  { ARGS_NAME, ARGS_HELP, args_action },
  { BLINK_NAME, BLINK_HELP, blink_action },
  { DATE_NAME, DATE_HELP, date_action },
  { DELAY_NAME, DELAY_HELP, delay_action },
  { DUMP_NAME, DUMP_HELP, dump_action },
  { ECHO_NAME, ECHO_HELP, echo_action },
  { DIGITALREAD_NAME, DIGITALREAD_HELP, digitalread_action },
  { DIGITALTOGGLE_NAME, DIGITALTOGGLE_HELP, digitaltoggle_action },
  { HELP_NAME, HELP_HELP, help_action },
  { IDLE_NAME, IDLE_HELP, idle_action },
  { LED_NAME, LED_HELP, led_action },
  { MEMORY_NAME, MEMORY_HELP, memory_action },
  { MICROS_NAME, MICROS_HELP, micros_action },
  { MILLIS_NAME, MILLIS_HELP, millis_action },
  { REPEAT_NAME, REPEAT_HELP, repeat_action },
  { SECONDS_NAME, SECONDS_HELP, seconds_action },
  { STTY_NAME, STTY_HELP, stty_action },
  { TONE_NAME, TONE_HELP, tone_action }
};

Shell shell(membersof(command_tab), command_tab);
