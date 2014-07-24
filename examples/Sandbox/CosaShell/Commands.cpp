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
#include "Cosa/Time.hh"
#include "Cosa/RTC.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"

// Digital and Analog Pin map (index => pin identity)
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
  "[all|PIN..] -- read analog pins";
static int analogread_action(int argc, char* argv[])
{
  if (argc == 2 && strcmp_P(argv[1], PSTR("all")) == 0) {
    for (uint8_t ix = 0; ix < membersof(analog_pin_map); ix++) {
      Board::AnalogPin pin;
      pin = (Board::AnalogPin) pgm_read_byte(&analog_pin_map[ix]);
      cout << 'A' << ix << '=' << AnalogPin::sample(pin) << endl;
    }
  }
  else {
    for (int i = 1; i < argc; i++) {
      char* name = argv[i];
      if (name[0] != 'a' && name[0] != 'A') return (-1);
      char* sp;
      uint32_t ix = strtoul(name + 1, &sp, 10);
      if (*sp != 0 || ix >= membersof(analog_pin_map)) return (-1);
      Board::AnalogPin pin;
      pin = (Board::AnalogPin) pgm_read_byte(&analog_pin_map[ix]);
      cout << name << '=' << AnalogPin::sample(pin) << endl;
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
  int i;
  while ((i = shell.get(option, value)) == 0)
    cout << PSTR("option: ") << option << PSTR(" value: ") << value << endl;
  while (i < argc)
    cout << PSTR("argument: ") << argv[i++] << endl;
  return (0);
}
   
#define LF "\n"

static const char BLINK_NAME[] __PROGMEM = 
  "blink";
static const char BLINK_HELP[] __PROGMEM = 
  "MS -- turn led on and off";
static const char BLINK_SCRIPT[] __PROGMEM = 
  SHELL_SCRIPT_MAGIC					       
  "echo -n $1 \"ms:led on\"" LF
  "led on" LF
  "delay $1" LF
  "echo -n \"..off\"" LF
  "led off" LF
  "delay $1" LF
  "echo -n \"..on\"" LF
  "led on" LF
  "delay $1" LF
  "echo \"..off\"" LF
  "led off" LF;
#define blink_action (Shell::action_fn) BLINK_SCRIPT

static const char DATE_NAME[] __PROGMEM = 
  "date";
static const char DATE_HELP[] __PROGMEM = 
  "-- print the system date and time";
static int date_action(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1) return (-1);
  time_t now(RTC::seconds());
  cout << now << endl;
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
  "[all|PIN..] -- read digital pins";
static int digitalread_action(int argc, char* argv[])
{
  if (argc == 2 && strcmp_P(argv[1], PSTR("all")) == 0) {
    for (uint8_t ix = 0; ix < membersof(digital_pin_map); ix++) {
      Board::DigitalPin pin;
      pin = (Board::DigitalPin) pgm_read_byte(&digital_pin_map[ix]);
      cout << 'D' << ix << '=' << InputPin::read(pin) << endl;
    }
  }
  else {
    for (int i = 1; i < argc; i++) {
      char* name = argv[i];
      if (name[0] != 'd' && name[0] != 'D') return (-1);
      char* sp;
      uint32_t ix = strtoul(name + 1, &sp, 10);
      if (*sp != 0 || ix >= membersof(digital_pin_map)) return (-1);
      Board::DigitalPin pin;
      pin = (Board::DigitalPin) pgm_read_byte(&digital_pin_map[ix]);
      cout << name << '=' << InputPin::read(pin) << endl;
    }
  }
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
  cout << argv[ix++];
  while (ix < argc) cout << ' ' << argv[ix++];
  if (newline) cout << endl;
  return (0);
}

static const char HELP_NAME[] __PROGMEM = 
  "help";
static const char HELP_HELP[] __PROGMEM = 
  "-- list command help";
static int help_action(int argc, char* argv[])
{
  UNUSED(argc);
  UNUSED(argv);
  return (shell.help(cout));
}

static const char LED_NAME[] __PROGMEM = 
  "led";
static const char LED_HELP[] __PROGMEM = 
  "[on|off] -- turn led on or off";
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

static const char MILLIS_NAME[] __PROGMEM = 
  "millis";
static const char MILLIS_HELP[] __PROGMEM = 
  "-- clock in milli-seconds";
static int millis_action(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1) return (-1);
  cout << RTC::millis() << endl;
  return (0);
}

static const char STTY_NAME[] __PROGMEM = 
  "stty";
static const char STTY_HELP[] __PROGMEM = 
  "echo [on|off] -- turn tty echo on or off";
static int stty_action(int argc, char* argv[])
{
  if (argc != 3 && strcmp_P(argv[1], PSTR("echo")) != 0) return (-1);
  if (strcmp_P(argv[2], PSTR("on")) == 0) 
    shell.set_echo(1);
  else if (strcmp_P(argv[2], PSTR("off")) == 0) 
    shell.set_echo(0);
  else return (-1);
  return (0);
}

static const Shell::command_t command_tab[] __PROGMEM = {
  { ANALOGREAD_NAME, ANALOGREAD_HELP, analogread_action },
  { ARGS_NAME, ARGS_HELP, args_action },
  { BLINK_NAME, BLINK_HELP, blink_action },
  { DATE_NAME, DATE_HELP, date_action },
  { DELAY_NAME, DELAY_HELP, delay_action },
  { ECHO_NAME, ECHO_HELP, echo_action },
  { DIGITALREAD_NAME, DIGITALREAD_HELP, digitalread_action },
  { HELP_NAME, HELP_HELP, help_action },
  { LED_NAME, LED_HELP, led_action },
  { MILLIS_NAME, MILLIS_HELP, millis_action },
  { STTY_NAME, STTY_HELP, stty_action }
};

Shell shell(membersof(command_tab), command_tab);
