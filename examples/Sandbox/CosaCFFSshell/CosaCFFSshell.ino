/**
 * @file CosaCFFSshell.ino
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
 * Tiny command shell to demonstration the S25FL127S SPI Flash Memory
 * device driver and file system.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/FS/CFFS.hh"
#include "Cosa/Shell.hh"
#include "Cosa/Time.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

#if defined(ANARDUINO_MINIWIRELESS)
OutputPin rf_cs(Board::D10, 1);
#endif

S25FL127S flash;
IOStream cout;
IOStream cin;

static const char CAT_NAME[] PROGMEM = "cat";
static const char CAT_HELP[] PROGMEM = "cat FILE -- print content of file";
int cat_action(int argc, char* argv[])
{
  UNUSED(argc);
  const size_t BUF_MAX = 256;
  char buf[BUF_MAX];
  CFFS::File file;
  int res = file.open(argv[1], O_READ);
  if (res < 0) return (res);
  while ((res = file.read(buf, sizeof(buf))) > 0)
    cout.get_device()->write(buf, res);
  return (0);
}

static const char CD_NAME[] PROGMEM = "cd";
static const char CD_HELP[] PROGMEM = "cd DIR -- change directory";
int cd_action(int argc, char* argv[])
{
  UNUSED(argc);
  return (CFFS::cd(argv[1]));
}

static const char DATE_NAME[] PROGMEM = "date";
static const char DATE_HELP[] PROGMEM = "date -- current time and date";
int date_action(int argc, char* argv[])
{
  UNUSED(argc);
  UNUSED(argv);
  time_t now(RTC::seconds());
  cout << now << endl;
  return (0);
}

static const char LS_NAME[] PROGMEM = "ls";
static const char LS_HELP[] PROGMEM = "ls [--verbose] -- list files";
int ls_action(int argc, char* argv[])
{
  bool verbose = false;
  if (argc == 2) {
    if (strcmp_P(argv[1], PSTR("--verbose")) == 0)
      verbose = true;
    else return (-1);
  }
  return (CFFS::ls(cout, verbose));
}

static const char MKDIR_NAME[] PROGMEM = "mkdir";
static const char MKDIR_HELP[] PROGMEM = "mkdir DIR -- make directory";
int mkdir_action(int argc, char* argv[])
{
  UNUSED(argc);
  return (CFFS::mkdir(argv[1]));
}

static const char OD_NAME[] PROGMEM = "od";
static const char OD_HELP[] PROGMEM = "od FILE -- dump file in hex";
int od_action(int argc, char* argv[])
{
  UNUSED(argc);
  const size_t BUF_MAX = 256;
  char buf[BUF_MAX];
  CFFS::File file;
  int res = file.open(argv[1], O_READ);
  if (res < 0) return (res);
  while (file.read(buf, sizeof(buf)) > 0) 
    cout.print(buf, sizeof(buf), IOStream::hex, 128 + 16);
  return (0);
}

static const char RM_NAME[] PROGMEM = "rm";
static const char RM_HELP[] PROGMEM = "rm FILE -- remove file";
int rm_action(int argc, char* argv[])
{
  UNUSED(argc);
  return (CFFS::rm(argv[1]));
}
   
// List of shell command descriptors
static const Shell::command_t command_vec[] PROGMEM = {
  { 2, CAT_NAME, cat_action, CAT_HELP },
  { 2, CD_NAME, cd_action, CD_HELP },
  { 2, DATE_NAME, date_action, DATE_HELP },
  { 2, LS_NAME, ls_action, LS_HELP },
  { 2, MKDIR_NAME, mkdir_action, MKDIR_HELP },
  { 2, OD_NAME, od_action, OD_HELP },
  { 2, RM_NAME, rm_action, RM_HELP }
};

Shell shell(membersof(command_vec), command_vec);

void setup()
{
  // Initiate timers
  Watchdog::begin();
  RTC::begin();

  // Initiate UART for blocked read line
  uart.begin(9600);
  uart.set_blocking(SLEEP_MODE_IDLE);
  cin.set_device(&uart);
  cout.set_device(&uart);

  // Initiate flash memory and file system
  flash.begin();
  CFFS::begin(&flash);
}

void loop()
{
  // The shell command handler will do the top loop
  if (!shell.run(&cin, &cout)) return;
  cout << PSTR("illegal command") << endl;
}
