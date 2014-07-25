/**
 * @file CFFSshell.cpp
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

#include "CFFSshell.h"
#include "Cosa/Time.hh"
#include "Cosa/RTC.hh"
#include "Cosa/FS/CFFS.hh"

static const char CAT_NAME[] __PROGMEM = "cat";
static const char CAT_HELP[] __PROGMEM = "FILE -- print content of file";
static int cat_action(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  const size_t BUF_MAX = 256;
  char buf[BUF_MAX];
  CFFS::File file;
  int res = file.open(argv[1], O_READ);
  if (res < 0) return (res);
  while ((res = file.read(buf, sizeof(buf))) > 0)
    cout.get_device()->write(buf, res);
  return (0);
}

static const char CD_NAME[] __PROGMEM = "cd";
static const char CD_HELP[] __PROGMEM = "DIR -- change directory";
static int cd_action(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  return (CFFS::cd(argv[1]));
}

static const char DATE_NAME[] __PROGMEM = "date";
static const char DATE_HELP[] __PROGMEM = "-- current time and date";
static int date_action(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1) return (-1);
  time_t now(RTC::seconds());
  cout << now << endl;
  return (0);
}

static const char HELP_NAME[] __PROGMEM = "help";
static const char HELP_HELP[] __PROGMEM = "-- list command help";
static int help_action(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1) return (-1);
  return (shell.help(cout));
}

static const char LS_NAME[] __PROGMEM = "ls";
static const char LS_HELP[] __PROGMEM = "[--verbose] -- list files";
static int ls_action(int argc, char* argv[])
{
  if (argc > 2) return (-1);
  bool verbose = false;
  if (argc == 2) {
    if (strcmp_P(argv[1], PSTR("--verbose")) == 0)
      verbose = true;
    else return (-1);
  }
  return (CFFS::ls(cout, verbose));
}

static const char MKDIR_NAME[] __PROGMEM = "mkdir";
static const char MKDIR_HELP[] __PROGMEM = "DIR -- make directory";
static int mkdir_action(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  return (CFFS::mkdir(argv[1]));
}

static const char OD_NAME[] __PROGMEM = "od";
static const char OD_HELP[] __PROGMEM = "FILE -- dump file in hex";
static int od_action(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  const size_t BUF_MAX = 256;
  char buf[BUF_MAX];
  CFFS::File file;
  int res = file.open(argv[1], O_READ);
  if (res < 0) return (res);
  while ((res = file.read(buf, sizeof(buf))) > 0) 
    cout.print(buf, res, IOStream::hex, 128 + 16);
  return (0);
}

static const char RM_NAME[] __PROGMEM = "rm";
static const char RM_HELP[] __PROGMEM = "FILE -- remove file";
static int rm_action(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  return (CFFS::rm(argv[1]));
}
   
static const char SIZE_NAME[] __PROGMEM = "size";
static const char SIZE_HELP[] __PROGMEM = "FILE -- file size";
static int size_action(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  CFFS::File file;
  int res = file.open(argv[1], O_READ);
  if (res < 0) return (res);
  cout << file.size() << endl;
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
  { CAT_NAME, CAT_HELP, cat_action },
  { CD_NAME, CD_HELP, cd_action },
  { DATE_NAME, DATE_HELP, date_action },
  { HELP_NAME, HELP_HELP, help_action },
  { LS_NAME, LS_HELP, ls_action },
  { MKDIR_NAME, MKDIR_HELP, mkdir_action },
  { OD_NAME, OD_HELP, od_action },
  { RM_NAME, RM_HELP, rm_action },
  { SIZE_NAME, SIZE_HELP, size_action },
  { STTY_NAME, STTY_HELP, stty_action }
};

Shell shell(membersof(command_tab), command_tab);
