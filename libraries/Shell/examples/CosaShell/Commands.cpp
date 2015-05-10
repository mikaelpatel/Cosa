/**
 * @file Commands.cpp
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

#include <Shell.h>
#include "Commands.h"
#include "Cosa/RTC.hh"
#include "Cosa/Time.hh"
#include "Cosa/Tone.hh"
#include "Cosa/Memory.h"
#include "Cosa/AnalogPin.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/IOPin.hh"
#include "Cosa/OWI.hh"
#include "Cosa/TWI.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/IOStream/Driver/UART.hh"

IOStream ios(&uart);

static uint32_t idle = 0L;

void iowait()
{
  uint32_t start = RTC::micros();
  Power::sleep();
  uint32_t stop = RTC::micros();
  idle = (start > stop) ? 0L : idle + (stop - start);
}

static uint32_t epoch = 0L;

SHELL_ACTION(analogread, "all|ALL|vcc|PIN..", "read analog pin(s)")
(int argc, char* argv[])
{
  if (argc == 2 && (strcmp_P(argv[1], PSTR("all")) == 0 ||
		    strcmp_P(argv[1], PSTR("ALL")) == 0)) {
    char prefix = argv[1][0];
    for (uint8_t ix = 0; ix < membersof(analog_pin_map);) {
      Board::AnalogPin pin;
      pin = (Board::AnalogPin) pgm_read_byte(&analog_pin_map[ix]);
      ios << prefix << ix << '=' << AnalogPin::sample(pin);
      ix += 1;
      if (ix < membersof(analog_pin_map)) ios << ',';
    }
    ios << endl;
  }
  else if (argc == 2 && (strcmp_P(argv[1], PSTR("vcc")) == 0)) {
    ios << AnalogPin::bandgap() << endl;
  }
  else {
    Board::AnalogPin pin[argc];
    for (int i = 1; i < argc; i++) {
      char* name = argv[i];
      if (name[0] != 'a' && name[0] != 'A')
	return (Shell::ILLEGAL_COMMAND);
      char* sp;
      uint32_t ix = strtoul(name + 1, &sp, 10);
      if (*sp != 0 || ix >= membersof(analog_pin_map))
	return (Shell::ILLEGAL_COMMAND);
      pin[i] = (Board::AnalogPin) pgm_read_byte(&analog_pin_map[ix]);
    }
    for (int i = 1; i < argc;) {
      char* name = argv[i];
      if (argc > 2) ios << name << '=';
      ios << AnalogPin::sample(pin[i]);
      i += 1;
      if (i < argc) ios << ',';
    }
    ios << endl;
  }
  return (0);
}

SHELL_ACTION(args, "OPTS ARGS", "display options and arguments")
(int argc, char* argv[])
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

SHELL_SCRIPT_BEGIN(blink, "MS", "turn led on and off")
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
  "led off" LF
SHELL_SCRIPT_END(blink)

SHELL_ACTION(date, "[YEAR-MON-DAY HOUR:MIN:SEC]", "display or set the system date and time")
(int argc, char* argv[])
{
  if (argc == 3) {
    if (!shell.is_privileged(Shell::ADMIN))
      return (Shell::PERMISSION_DENIED);
    uint32_t value;
    time_t now;
    char* sp;
    value = strtoul(argv[1], &sp, 10);
    if (*sp != '-' || value < 2000 || value > 2099)
      return (Shell::ILLEGAL_COMMAND);
    value -= 2000;
    now.year = value;
    value = strtoul(sp + 1, &sp, 10);
    if (*sp != '-' || value < 1 || value > 12)
      return (Shell::ILLEGAL_COMMAND);
    now.month = value;
    value = strtoul(sp + 1, &sp, 10);
    if (*sp != 0 || value < 1 || value > 31)
      return (Shell::ILLEGAL_COMMAND);
    now.date = value;
    value = strtoul(argv[2], &sp, 10);
    if (*sp != ':' || value > 23)
      return (Shell::ILLEGAL_COMMAND);
    now.hours = value;
    value = strtoul(sp + 1, &sp, 10);
    if (*sp != ':' || value > 60)
      return (Shell::ILLEGAL_COMMAND);
    now.minutes = value;
    value = strtoul(sp + 1, &sp, 10);
    if (*sp != 0 || value > 60)
      return (Shell::ILLEGAL_COMMAND);
    now.seconds = value;
    epoch = now;
    RTC::time(epoch);
  }
  else if (argc != 1)
    return (Shell::ILLEGAL_COMMAND);
  time_t now(RTC::seconds());
  ios << now << endl;
  return (0);
}

SHELL_ACTION(delay, "MS", "delay for milliseconds")
(int argc, char* argv[])
{
  if (argc != 2)
    return (Shell::ILLEGAL_COMMAND);
  char* sp;
  uint32_t ms = strtoul(argv[1], &sp, 10);
  if (*sp != 0)
    return (Shell::ILLEGAL_COMMAND);
  delay(ms);
  return (0);
}

SHELL_ACTION(digitalread, "all|ALL|led|PIN..", "read digital pin(s)")
(int argc, char* argv[])
{
  if (argc == 2 && (strcmp_P(argv[1], PSTR("all")) == 0 ||
		    strcmp_P(argv[1], PSTR("ALL")) == 0)) {
    char prefix = (argv[1][0] == 'A') ? 'D' : 'd';
    for (uint8_t ix = 0; ix < membersof(digital_pin_map);) {
      Board::DigitalPin pin;
      pin = (Board::DigitalPin) pgm_read_byte(&digital_pin_map[ix]);
      ios << prefix << ix << '=' << InputPin::read(pin);
      ix += 1;
      if (ix < membersof(digital_pin_map)) ios << ',';
    }
    ios << endl;
  }
  else if (argc == 2 && (strcmp_P(argv[1], PSTR("led")) == 0))
    ios << InputPin::read(Board::LED) << endl;
  else {
    Board::DigitalPin pin[argc];
    for (int i = 1; i < argc; i++) {
      char* name = argv[i];
      if (name[0] != 'd' && name[0] != 'D')
	return (Shell::ILLEGAL_COMMAND);
      char* sp;
      uint32_t ix = strtoul(name + 1, &sp, 10);
      if (*sp != 0 || ix >= membersof(digital_pin_map))
	return (Shell::ILLEGAL_COMMAND);
      pin[i] = (Board::DigitalPin) pgm_read_byte(&digital_pin_map[ix]);
    }
    for (int i = 1; i < argc;) {
      char* name = argv[i];
      if (argc > 2) ios << name << '=';
      ios << InputPin::read(pin[i]);
      i += 1;
      if (i < argc) ios << ',';
    }
    ios << endl;
  }
  return (0);
}

SHELL_ACTION(digitaltoggle, "led|PIN", "toggle digital pin")
(int argc, char* argv[])
{
  if (argc != 2)
    return (Shell::ILLEGAL_COMMAND);
  Board::DigitalPin pin;
  if (strcmp_P(argv[1], PSTR("led")) == 0) {
    pin = Board::LED;
  }
  else {
    char* name = argv[1];
    if (name[0] != 'd' && name[0] != 'D')
      return (Shell::ILLEGAL_COMMAND);
    char* sp;
    uint32_t ix = strtoul(name + 1, &sp, 10);
    if (*sp != 0 || ix >= membersof(digital_pin_map))
      return (Shell::ILLEGAL_COMMAND);
    pin = (Board::DigitalPin) pgm_read_byte(&digital_pin_map[ix]);
  }
  OutputPin::toggle(pin);
  ios << InputPin::read(pin) << endl;
  return (0);
}

SHELL_ACTION(digitalwrite, "led|PIN VALUE", "write digital pin")
(int argc, char* argv[])
{
  if (argc != 3)
    return (Shell::ILLEGAL_COMMAND);
  Board::DigitalPin pin;
  char* name = argv[1];
  char* sp;
  if (strcmp_P(name, PSTR("led")) == 0) {
    pin = Board::LED;
  }
  else {
    if (name[0] != 'd' && name[0] != 'D')
      return (Shell::ILLEGAL_COMMAND);
    uint32_t ix = strtoul(name + 1, &sp, 10);
    if (*sp != 0 || ix >= membersof(digital_pin_map))
      return (Shell::ILLEGAL_COMMAND);
    pin = (Board::DigitalPin) pgm_read_byte(&digital_pin_map[ix]);
  }
  bool value = (strtoul(argv[2], &sp, 10) != 0);
  if (*sp != 0)
    return (Shell::ILLEGAL_COMMAND);
  OutputPin::write(pin, value);
  ios << InputPin::read(pin) << endl;
  return (0);
}

SHELL_ACTION(dump, "[-b|-d] ADDR [SIZE]", "dump memory block")
(int argc, char* argv[])
{
  IOStream::Base base = IOStream::hex;
  uint32_t addr = 0L;
  size_t size = 256;
  char* option;
  char* value;
  char* sp;
  int ix;
  while ((ix = shell.get(option, value)) == 0) {
    if (*value != 0)
      return (Shell::ILLEGAL_OPTION);
    if (strcmp_P(option, PSTR("b")) == 0)
      base = IOStream::bin;
    else if (strcmp_P(option, PSTR("d")) == 0)
      base = IOStream::dec;
    else
      return (Shell::UNKNOWN_OPTION);
  }
  if (ix < argc) {
    addr = strtoul(argv[ix++], &sp, 16);
    if (*sp != 0)
      return (Shell::ILLEGAL_COMMAND);
  }
  if (ix < argc) {
    size = strtoul(argv[ix++], &sp, 10);
    if (*sp != 0)
      return (Shell::ILLEGAL_COMMAND);
  }
  if (ix != argc)
    return (Shell::ILLEGAL_COMMAND);
  ios.print(addr, (void*) addr, size, base);
  return (0);
}

SHELL_ACTION(echo, "[-n] STRING..", "print a line of text")
(int argc, char* argv[])
{
  bool newline = true;
  char* option;
  char* value;
  int ix;
  while ((ix = shell.get(option, value)) == 0)
    if (strcmp_P(option, PSTR("n")) == 0)
      newline = false;
    else
      return (Shell::ILLEGAL_COMMAND);
  if (ix == argc) return (0);
  ios << argv[ix++];
  while (ix < argc) ios << ' ' << argv[ix++];
  if (newline) ios << endl;
  return (0);
}

SHELL_ACTION(epoch, "", "display start time")
(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1)
    return (Shell::ILLEGAL_COMMAND);
  time_t now(epoch);
  ios << now << endl;
  return (0);
}

SHELL_ACTION(logout, "", "logout from shell")
(int argc, char* argv[]);

SHELL_ACTION(help, "", "list command help")
(int argc, char* argv[])
{
  if (argc != 1)
    return (shell.help(ios, argv[1]));
  else
    return (shell.help(ios));
}

SHELL_ACTION(idle, "", "display idle time")
(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1)
    return (Shell::ILLEGAL_COMMAND);
  ios << (idle * 100.0) / RTC::micros() << '%' << endl;
  return (0);
}

SHELL_ACTION(led, "on|off", "turn led on or off")
(int argc, char* argv[])
{
  if (argc != 2)
    return (Shell::ILLEGAL_COMMAND);
  if (strcmp_P(argv[1], PSTR("on")) == 0)
    OutputPin::write(Board::LED, 1);
  else if (strcmp_P(argv[1], PSTR("off")) == 0)
    OutputPin::write(Board::LED, 0);
  else
    return (Shell::ILLEGAL_COMMAND);
  return (0);
}

SHELL_ACTION(memory, "", "display amount of free memory")
(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1)
    return (Shell::ILLEGAL_COMMAND);
  ios << free_memory() << PSTR(" bytes") << endl;
  return (0);
}

SHELL_ACTION(micros, "", "clock in micro-seconds")
(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1)
    return (Shell::ILLEGAL_COMMAND);
  ios << RTC::micros() << endl;
  return (0);
}

SHELL_ACTION(millis, "", "clock in milli-seconds")
(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1)
    return (Shell::ILLEGAL_COMMAND);
  ios << RTC::millis() << endl;
  return (0);
}

SHELL_ACTION(owi, "scan PIN", "scan 1-wire bus")
(int argc, char* argv[])
{
  if (argc != 3)
    return (Shell::ILLEGAL_COMMAND);
  if (strcmp_P(argv[1], PSTR("scan")) != 0)
    return (Shell::ILLEGAL_COMMAND);
  char* name = argv[2];
  char* sp;
  if (name[0] != 'd' && name[0] != 'D')
    return (Shell::ILLEGAL_COMMAND);
  uint32_t ix = strtoul(name + 1, &sp, 10);
  if (*sp != 0 || ix >= membersof(digital_pin_map))
    return (Shell::ILLEGAL_COMMAND);
  OWI owi((Board::DigitalPin) pgm_read_byte(&digital_pin_map[ix]));
  ios << owi;
  return (0);
}

static void write_pinmode(Board::DigitalPin pin)
{
  if (IOPin::get_mode(pin) == IOPin::OUTPUT_MODE)
    ios << PSTR("output") << endl;
  else {
    ios << PSTR("input");
    if (InputPin::get_mode(pin) == InputPin::PULLUP_MODE)
      ios << PSTR(", pullup");
    ios << endl;
  }
}

SHELL_ACTION(pinmode, "all|ALL|led|PIN [input|output|pullup]",
             "display or set pin mode")
(int argc, char* argv[])
{
  if (argc < 2 || argc > 3)
    return (Shell::ILLEGAL_COMMAND);
  Board::DigitalPin pin;
  char* name = argv[1];
  if (argc == 2 && (strcmp_P(argv[1], PSTR("all")) == 0 ||
		    strcmp_P(argv[1], PSTR("ALL")) == 0)) {
    char prefix = (argv[1][0] == 'A') ? 'D' : 'd';
    for (uint8_t ix = 0; ix < membersof(digital_pin_map); ix++) {
      pin = (Board::DigitalPin) pgm_read_byte(&digital_pin_map[ix]);
      ios << prefix << ix << '=';
      write_pinmode(pin);
    }
    return (0);
  }
  else if (strcmp_P(name, PSTR("led")) == 0)
    pin = Board::LED;
  else if (name[0] == 'd' || name[0] == 'D') {
    char* sp;
    uint32_t ix = strtoul(name + 1, &sp, 10);
    if (*sp != 0 || ix >= membersof(digital_pin_map))
      return (Shell::ILLEGAL_COMMAND);
    pin = (Board::DigitalPin) pgm_read_byte(&digital_pin_map[ix]);
  }
  else
    return (Shell::ILLEGAL_COMMAND);
  if (argc == 3) {
    if (!shell.is_privileged(Shell::USER))
      return (Shell::PERMISSION_DENIED);
    if (strcmp_P(argv[2], PSTR("input")) == 0)
      IOPin::set_mode(pin, IOPin::INPUT_MODE);
    else if (strcmp_P(argv[2], PSTR("output")) == 0)
      IOPin::set_mode(pin, IOPin::OUTPUT_MODE);
    else if (strcmp_P(argv[2], PSTR("pullup")) == 0)
      InputPin::set_mode(pin, InputPin::PULLUP_MODE);
    else
      return (Shell::ILLEGAL_COMMAND);
  }
  write_pinmode(pin);
  return (0);
}

SHELL_ACTION(repeat, "[-t] COUNT [DELAY] COMMAND", "repeat command line")
(int argc, char* argv[])
{
  uint8_t ix = 1;
  bool timing = (strcmp_P(argv[ix], PSTR("-t")) == 0);
  if (timing) ix += 1;
  char* sp;
  uint16_t count = strtoul(argv[ix++], &sp, 10);
  if (*sp != 0 || count == 0)
    return (Shell::ILLEGAL_COMMAND);
  uint32_t ms = strtoul(argv[ix], &sp, 10);
  if (*sp != 0) {
    ms = 0L;
  }
  else
    ix += 1;
  if (argc < ix + 1)
    return (Shell::ILLEGAL_COMMAND);
  const size_t BUF_MAX = 64;
  char buf[BUF_MAX];
  uint8_t fx = ix;
  uint32_t start = RTC::millis();
  do {
    buf[0] = 0;
    strcat(buf, argv[fx]);
    ix = fx + 1;
    while (ix < argc) {
      strcat(buf, " ");
      strcat(buf, argv[ix++]);
    }
    if (shell.execute(buf) != 0)
      return (Shell::ILLEGAL_COMMAND);
    if (ms != 0) delay(ms);
  } while (--count);
  uint32_t stop = RTC::millis();
  if (timing) ios << stop - start << PSTR(" ms") << endl;
  return (ios.get_device()->flush());
}

SHELL_ACTION(stty, "[eol=CR|LF|CRLF]", "display or set tty mode")
(int argc, char* argv[])
{
  UNUSED(argv);
  char* option;
  char* value;
  int ix;
  IOStream::Mode mode = ios.get_device()->get_eol();
  while ((ix = shell.get(option, value)) == 0) {
    if (strcmp_P(option, PSTR("eol")) == 0) {
      if (strcmp_P(value, PSTR("CR")) == 0)
	mode = IOStream::CR_MODE;
      else if (strcmp_P(value, PSTR("LF")) == 0)
	mode = IOStream::LF_MODE;
      else if (strcmp_P(value, PSTR("CRLF")) == 0)
	mode = IOStream::CRLF_MODE;
      else
	return (Shell::ILLEGAL_OPTION);
    }
    else
      return (Shell::UNKNOWN_OPTION);
  }
  if (ix != argc)
    return (Shell::ILLEGAL_COMMAND);
  ios.get_device()->set_eol(mode);
  ios << PSTR("eol=");
  switch (mode) {
  case IOStream::CR_MODE: ios << PSTR("CR"); break;
  case IOStream::LF_MODE: ios << PSTR("LF"); break;
  case IOStream::CRLF_MODE: ios << PSTR("CRLF"); break;
  }
  ios << endl;
  return (0);
}

SHELL_ACTION(tone, "off | FREQ [VOLUME [DURATION]]", "play tone")
(int argc, char* argv[])
{
  if (argc < 2 || argc > 4)
    return (Shell::ILLEGAL_COMMAND);
  if (argc == 2 && strcmp_P(argv[1], PSTR("off")) == 0) {
    Tone::silent();
  }
  else {
    char* sp;
    uint16_t freq = strtoul(argv[1], &sp, 10);
    if (*sp != 0)
      return (Shell::ILLEGAL_COMMAND);
    uint8_t volume = Tone::VOLUME_MAX / 2;
    if (argc > 2) {
      volume = strtoul(argv[2], &sp, 10);
      if (*sp != 0)
	return (Shell::ILLEGAL_COMMAND);
    }
    uint16_t duration = 0;
    if (argc > 3) {
      duration = strtoul(argv[3], &sp, 10);
      if (*sp != 0)
	return (Shell::ILLEGAL_COMMAND);
    }
    Tone::play(freq, volume, duration);
  }
  return (0);
}

SHELL_ACTION(twi, "scan", "scan I2C bus")
(int argc, char* argv[])
{
  if (argc != 2)
    return (Shell::ILLEGAL_COMMAND);
  if (strcmp_P(argv[1], PSTR("scan")) != 0)
    return (Shell::ILLEGAL_COMMAND);
  for (uint8_t addr = 3; addr < 128; addr++) {
    TWI::Driver dev(addr);
    twi.begin(&dev);
    uint8_t data;
    int count = twi.read(&data, sizeof(data));
    twi.end();
    if (count != sizeof(data)) continue;
    ios << PSTR("TWI::device(addr = ") << hex << addr
	<< PSTR(", group = ") << (addr >> 3) << '.' << (addr & 0x07)
	<< PSTR(")") << endl;
  }
  return (0);
}

SHELL_ACTION(uptime, "", "seconds since latest date set or system start")
(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1)
    return (Shell::ILLEGAL_COMMAND);
  ios << RTC::seconds() - epoch << endl;
  return (0);
}

SHELL_BEGIN(command_tab)
  SHELL_COMMAND(analogread, Shell::USER)
  SHELL_COMMAND(args, Shell::GUEST)
  SHELL_COMMAND(blink, Shell::USER)
  SHELL_COMMAND(date, Shell::GUEST)
  SHELL_COMMAND(delay, Shell::USER)
  SHELL_COMMAND(dump, Shell::USER)
  SHELL_COMMAND(echo, Shell::USER)
  SHELL_COMMAND(epoch, Shell::GUEST)
  SHELL_COMMAND(digitalread, Shell::GUEST)
  SHELL_COMMAND(digitaltoggle, Shell::USER)
  SHELL_COMMAND(digitalwrite, Shell::USER)
  SHELL_COMMAND(help, Shell::GUEST)
  SHELL_COMMAND(idle, Shell::GUEST)
  SHELL_COMMAND(led, Shell::GUEST)
  SHELL_COMMAND(logout, Shell::GUEST)
  SHELL_COMMAND(memory, Shell::USER)
  SHELL_COMMAND(micros, Shell::USER)
  SHELL_COMMAND(millis, Shell::USER)
  SHELL_COMMAND(owi, Shell::USER)
  SHELL_COMMAND(pinmode, Shell::GUEST)
  SHELL_COMMAND(repeat, Shell::USER)
  SHELL_COMMAND(stty, Shell::USER)
  SHELL_COMMAND(tone, Shell::USER)
  SHELL_COMMAND(twi, Shell::USER)
  SHELL_COMMAND(uptime, Shell::GUEST)
SHELL_END

SHELL_ACTION(login, "USER", "authenticate user")
(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 2)
    return (Shell::ILLEGAL_COMMAND);
  if (strcmp_P(argv[1], PSTR("guest")) == 0)
    shell.set_privilege(Shell::GUEST);
  else {
    ios << PSTR("password: ") << flush;
    const size_t PASSWD_MAX = 32;
    char passwd[PASSWD_MAX];
    passwd[0] = 0;
    while (ios.readline(passwd, PASSWD_MAX, false) == NULL) yield();
    if (shell.get_echo()) ios << endl;
    if (strcmp_P(passwd, PSTR("ciao\n")) != 0)
      return (Shell::ILLEGAL_COMMAND);
    if (strcmp_P(argv[1], PSTR("admin")) == 0)
      shell.set_privilege(Shell::ADMIN);
    else
      shell.set_privilege(Shell::USER);
  }
  shell.set_commands(membersof(command_tab), command_tab);
  return (0);
}

static const char INIT_PROMPT[] __PROGMEM = "$ ";
SHELL_BEGIN(init_tab)
  SHELL_COMMAND(help, Shell::GUEST)
  SHELL_COMMAND(login, Shell::GUEST)
SHELL_END

static int logout_action(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1)
    return (Shell::ILLEGAL_COMMAND);
  shell.set_commands(membersof(init_tab), init_tab, INIT_PROMPT);
  return (0);
}

Shell shell(membersof(init_tab), init_tab, INIT_PROMPT);

