/**
 * @file CFFScommands.cpp
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

#include <CFFS.h>
#include <Shell.h>

#include "CFFScommands.h"

#include "Cosa/Time.hh"
#include "Cosa/RTT.hh"

SHELL_ACTION(cat, "FILE..", "print content of file")
(int argc, char* argv[])
{
  for (int ix = 1; ix < argc; ix++) {
    CFFS::File file;
    int res = file.open(argv[ix], O_READ);
    if (res < 0) return (res);
    int c;
    while ((c = file.getchar()) != IOStream::EOF)
      ios << (char) c;
    file.close();
  }
  return (0);
}

SHELL_ACTION(cd, "DIR", "change directory")
(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  return (CFFS::cd(argv[1]));
}

SHELL_ACTION(date, "", "current time and date")
(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc != 1) return (-1);
  time_t now(clock.time());
  ios << now << endl;
  return (0);
}

SHELL_ACTION(du, "FILE", "file size")
(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  CFFS::File file;
  int res = file.open(argv[1], O_READ);
  if (res < 0) return (res);
  ios << file.size() << endl;
  return (0);
}

SHELL_ACTION(help, "", "list command help")
(int argc, char* argv[])
{
  if (argc != 1)
    return (shell.help(ios, argv[1]));
  else
    return (shell.help(ios));
}

SHELL_ACTION(ls, "", "list files")
(int argc, char* argv[])
{
  UNUSED(argv);
  if (argc > 1) return (-1);
  return (CFFS::ls(ios));
}

SHELL_ACTION(mkdir, "DIR", "make directory")
(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  return (CFFS::mkdir(argv[1]));
}

SHELL_ACTION(od, "[-b|-d] FILE", "dump file (bin,dec,hex)")
(int argc, char* argv[])
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

SHELL_ACTION(read, "[-pPOS|-sSIZE] FILE", "print content of file (position/size)")
(int argc, char* argv[])
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
    ios.device()->write(buf, res);
    size -= res;
  } while (size != 0);
  return (0);
}

SHELL_ACTION(rm, "FILE", "remove file")
(int argc, char* argv[])
{
  if (argc != 2) return (-1);
  return (CFFS::rm(argv[1]));
}

SHELL_ACTION(stty, "[eol=CR|LF|CRLF]", "set tty mode")
(int argc, char* argv[])
{
  UNUSED(argv);
  char* option;
  char* value;
  int ix;
  while ((ix = shell.get(option, value)) == 0) {
    if (strcmp_P(option, PSTR("eol")) == 0) {
      if (strcmp_P(value, PSTR("CR")) == 0)
	ios.device()->eol(IOStream::CR_MODE);
      else if (strcmp_P(value, PSTR("LF")) == 0)
	ios.device()->eol(IOStream::LF_MODE);
      else if (strcmp_P(value, PSTR("CRLF")) == 0)
	ios.device()->eol(IOStream::CRLF_MODE);
      else return (-1);
    }
  }
  if (ix != argc) return (-1);
  return (0);
}

SHELL_ACTION(write, "[-n|t] FILE STRING..", "print text to file (newline/timestamp)")
(int argc, char* argv[])
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
  if (timestamp) ios << RTT::micros() << ':';
  ios << argv[ix++];
  while (ix < argc) ios << ' ' << argv[ix++];
  if (newline) ios << endl;
  return (0);
}

SHELL_BEGIN(command_tab)
  SHELL_COMMAND(cat, Shell::GUEST)
  SHELL_COMMAND(cd, Shell::GUEST)
  SHELL_COMMAND(date, Shell::GUEST)
  SHELL_COMMAND(du, Shell::GUEST)
  SHELL_COMMAND(help, Shell::GUEST)
  SHELL_COMMAND(ls, Shell::GUEST)
  SHELL_COMMAND(mkdir, Shell::GUEST)
  SHELL_COMMAND(od, Shell::GUEST)
  SHELL_COMMAND(read, Shell::GUEST)
  SHELL_COMMAND(rm, Shell::GUEST)
  SHELL_COMMAND(stty, Shell::GUEST)
  SHELL_COMMAND(write, Shell::GUEST)
SHELL_END

Shell shell(membersof(command_tab), command_tab);
