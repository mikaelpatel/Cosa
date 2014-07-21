/**
 * @file Cosa/Shell.cpp
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

#include "Cosa/Shell.hh"

const char Shell::DEFAULT_PROMPT[] __PROGMEM = "arduino:$ ";

int
Shell::lookup(char* name) 
{
  for (uint8_t i = 0; i < m_cmdc; i++) {
    if (strcmp_P(name, (const char*) pgm_read_word(&m_cmdv[i].name)) == 0)
      return (i);
  }
  return (-1);
}

int
Shell::execute(char* buf)
{
  const size_t ARGV_MAX = 32;
  char* argv[ARGV_MAX];
  int argc;
  
  // Scan the line for command, options and parameters
  char* bp = buf;
  char c;
  argc = 0;
  while (1) {
    c = *bp;
    while (c <= ' ' && c != 0) c = *++bp;
    if (c == 0) break;
    argv[argc++] = bp;
    while (c > ' ') c = *++bp;
    if (c == 0) break;
    *bp++ = 0;
  }
  argv[argc] = NULL;
  if (argc == 0) return (0);
  
  // Lookup shell command, validate number of argument and call action
  int i = lookup(argv[0]);
  if (i < 0) return (-1);
  int count = (int) pgm_read_word(&m_cmdv[i].argc);
  if (argc <= count) {
    action_fn action = (action_fn) pgm_read_word(&m_cmdv[i].action);
    return (action(argc, argv));
  }
  return (-1);
}

int 
Shell::run(IOStream* ins, IOStream* outs)
{
  const size_t BUF_MAX = 64;
  char buf[BUF_MAX];
  if (ins == NULL) return (-1);
  if (outs != NULL) *outs << m_prompt;
  if (ins->get_device()->gets(buf, sizeof(buf)) == NULL) return (-1);
  return (execute(buf));
}

