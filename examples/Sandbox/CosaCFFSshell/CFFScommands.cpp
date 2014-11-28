/**
 * @file CFFScommands.cpp
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

#include "CFFScommands.h"
#include "Cosa/Time.hh"
#include "Cosa/RTC.hh"
#include "Cosa/FS/CFFS.hh"

static const char CAT_NAME[] __PROGMEM = "cat";
static const char CAT_ARGS[] __PROGMEM = "FILE..";
static const char CAT_HELP[] __PROGMEM = "print content of file";
static int cat_action(int argc, char* argv[])
{
  for (int ix = 1; ix < argc; ix++) {
    const size_t BUF_MAX = 256;
    char buf[BUF_MAX];
    CFFS::File file;
    int res = file.open(argv[ix], O_READ);
    if (res < 0) return (res);
    while ((res = file.read(buf, sizeof(buf))) > 0)
      ios.get_device()->write(buf, res);
    file.close();
  }
  return (0);
}

static const char CD_NAME[] __PROGMEM = "cd";
static const char CD_ARGS[] __PROGMEM = "DIR";
static const char CD_HELP[] __PROGMEM = "change directory";
static int cd_action(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  return (CFFS::cd(argv[1]));
}

static const char DATE_NAME[] __PROGMEM = "date";
static const char DATE_HELP[] __PROGMEM = "current time and date";
static int date_action(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1) return (-1);
  time_t now(RTC::seconds());
  ios << now << endl;
  return (0);
}

static const char DU_NAME[] __PROGMEM = "du";
static const char DU_ARGS[] __PROGMEM = "FILE";
static const char DU_HELP[] __PROGMEM = "file size";
static int du_action(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  CFFS::File file;
  int res = file.open(argv[1], O_READ);
  if (res < 0) return (res);
  ios << file.size() << endl;
  return (0);
}
   
static const char HELP_NAME[] __PROGMEM = "help";
static const char HELP_HELP[] __PROGMEM = "list command help";
static int help_action(int argc, char* argv[])
{
  if (argc != 1)
    return (shell.help(ios, argv[1]));
  else
    return (shell.help(ios));
}

static const char LS_NAME[] __PROGMEM = "ls";
static const char LS_ARGS[] __PROGMEM = "[-v]";
static const char LS_HELP[] __PROGMEM = "list files (verbose)";
static int ls_action(int argc, char* argv[])
{
  if (argc > 2) return (-1);
  bool verbose = false;
  if (argc == 2) {
    if (strcmp_P(argv[1], PSTR("-v")) == 0)
      verbose = true;
    else return (-1);
  }
  return (CFFS::ls(ios, verbose));
}

static const char MKDIR_NAME[] __PROGMEM = "mkdir";
static const char MKDIR_ARGS[] __PROGMEM = "DIR";
static const char MKDIR_HELP[] __PROGMEM = "make directory";
static int mkdir_action(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  return (CFFS::mkdir(argv[1]));
}

static const char OD_NAME[] __PROGMEM = "od";
static const char OD_ARGS[] __PROGMEM = "[-b|-d] FILE";
static const char OD_HELP[] __PROGMEM = "dump file (bin,dec,hex)";
static int od_action(int argc, char* argv[])
{
  IOStream::Base base = IOStream::hex;
  char* option;
  char* value;
  int ix;
  while ((ix = shell.get(option, value)) == 0) {
    if (strcmp_P(option, PSTR("b")) == 0)
      base = IOStream::bin;
    else if (strcmp_P(option, PSTR("d")) == 0)
      base = IOStream::dec;
    else 
      return (-1);
  }
  if (ix == argc) return (-1);
  const size_t BUF_MAX = 256;
  char buf[BUF_MAX];
  CFFS::File file;
  int res = file.open(argv[ix], O_READ);
  if (res < 0) return (res);
  uint32_t src = 0L;
  while ((res = file.read(buf, sizeof(buf))) > 0) {
    ios.print(src, buf, res, base);
    src += res;
  }
  return (0);
}

static const char READ_NAME[] __PROGMEM = "read";
static const char READ_ARGS[] __PROGMEM = "[-pPOS|-sSIZE] FILE";
static const char READ_HELP[] __PROGMEM = "print content of file (position/size)";
static int read_action(int argc, char* argv[])
{
  uint32_t pos = 0L;
  uint32_t size = 0L;
  char* option;
  char* value;
  char* sp;
  int ix;
  while ((ix = shell.get(option, value)) == 0)
    if (strcmp_P(option, PSTR("p")) == 0) {
      pos = strtoul(value, &sp, 10);
      if (*sp != 0) return (-1);
    }
    else if (strcmp_P(option, PSTR("s")) == 0) {
      size = strtoul(value, &sp, 10);
      if (*sp != 0) return (-1);
    }
    else return (-1);
  if (ix + 1 != argc) return (-1);
  const size_t BUF_MAX = 256;
  char buf[BUF_MAX];
  CFFS::File file;
  int res = file.open(argv[ix], O_READ);
  if (res < 0) return (res);
  uint32_t total = file.size();
  if (size == 0L) size = total;
  if (pos >= total) return (-1);
  if (pos + size > total) size = total - pos;
  if (file.seek(pos) != 0) return (-1);
  do {
    size_t count = (size > sizeof(buf) ? sizeof(buf) : size);
    res = file.read(buf, count);
    if (res < 0) return (-1);
    ios.get_device()->write(buf, res);
    size -= res;
  } while (size != 0);
  return (0);
}

static const char RM_NAME[] __PROGMEM = "rm";
static const char RM_ARGS[] __PROGMEM = "FILE";
static const char RM_HELP[] __PROGMEM = "remove file";
static int rm_action(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  return (CFFS::rm(argv[1]));
}
   
static const char STTY_NAME[] __PROGMEM = 
  "stty";
static const char STTY_ARGS[] __PROGMEM = 
  "[eol=CR|LF|CRLF]";
static const char STTY_HELP[] __PROGMEM = 
  "set tty mode";
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

static const char WRITE_NAME[] __PROGMEM = 
  "write";
static const char WRITE_ARGS[] __PROGMEM = 
  "[-n|t] FILE STRING..";
static const char WRITE_HELP[] __PROGMEM = 
  "print text to file (newline/timestamp)";
static int write_action(int argc, char* argv[])
{
  bool newline = true;
  bool timestamp = false;
  char* option;
  char* value;
  int ix;
  while ((ix = shell.get(option, value)) == 0) {
    if (strcmp_P(option, PSTR("n")) == 0)
      newline = false;
    else if (strcmp_P(option, PSTR("t")) == 0)
      timestamp = true;
    else return (-1);
  }
  if (argc <= ix + 1) return (-1);
  CFFS::File file;
  if (file.open(argv[ix], O_CREAT | O_EXCL) != 0) 
    if (file.open(argv[ix], O_WRITE) != 0) return (-1);
  ix += 1;
  IOStream ios(&file);
  if (timestamp) ios << RTC::micros() << ':';
  ios << argv[ix++];
  while (ix < argc) ios << ' ' << argv[ix++];
  if (newline) ios << endl;
  return (0);
}

static const Shell::command_t command_tab[] __PROGMEM = {
  { CAT_NAME, CAT_ARGS, CAT_HELP, cat_action, Shell::GUEST },
  { CD_NAME, CD_ARGS, CD_HELP, cd_action, Shell::GUEST },
  { DATE_NAME, NULL, DATE_HELP, date_action, Shell::GUEST },
  { DU_NAME, DU_ARGS, DU_HELP, du_action, Shell::GUEST },
  { HELP_NAME, NULL, HELP_HELP, help_action, Shell::GUEST },
  { LS_NAME, LS_ARGS, LS_HELP, ls_action, Shell::GUEST },
  { MKDIR_NAME, MKDIR_ARGS, MKDIR_HELP, mkdir_action, Shell::GUEST },
  { OD_NAME, OD_ARGS, OD_HELP, od_action, Shell::GUEST },
  { READ_NAME, READ_ARGS, READ_HELP, read_action, Shell::GUEST },
  { RM_NAME, RM_ARGS, RM_HELP, rm_action, Shell::GUEST },
  { STTY_NAME, STTY_ARGS, STTY_HELP, stty_action, Shell::GUEST },
  { WRITE_NAME, WRITE_ARGS, WRITE_HELP, write_action, Shell::GUEST }
};

Shell shell(membersof(command_tab), command_tab);
