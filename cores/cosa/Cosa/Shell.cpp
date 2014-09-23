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

const Shell::command_t*
Shell::lookup(char* name) 
{
  for (uint8_t i = 0; i < m_cmdc; i++) {
    if (strcmp_P(name, (const char*) pgm_read_word(&m_cmdtab[i].name)) == 0)
      return (&m_cmdtab[i]);
  }
  return (NULL);
}

int
Shell::get(char* &option, char* &value)
{
  // Check end of options
  if (m_optind == m_argc || m_optend) return (m_optind);

  // Check for single character option and possible value
  char* arg = m_argv[m_optind];
  if (arg[0] == '-') {
    if (arg[1] == 0) {
      m_optend = false;
      return (m_optind);
    }
    arg[0] = arg[1];
    arg[1] = 0;
    value = arg + 2;
  }

  // Check for option value assignment. End of options if not found
  else {
    char* sp = strchr(arg, '=');
    if (sp == NULL) {
      m_optend = true;
      return (m_optind);
    }
    *sp = 0;
    value = sp + 1;
  }
  option = arg;
  m_optind += 1;
  return (0);
}

int
Shell::execute(char* buf)
{
  // Scan the line for command, options and parameters
  if (buf == NULL) return (0);
  char* argv[ARGV_MAX];
  int argc = 0;
  char* bp = buf;
  char c;
  do {
    c = *bp;
    // Skip white space
    while (c <= ' ' && c != 0) c = *++bp;
    if (c == 0) break;
    // Check for string literal
    if (c == '"') {
      c = *++bp;
      if (c == 0) return (ILLEGAL_COMMAND);
      argv[argc++] = bp;
      while (c != 0 && c != '"') c = *++bp;
      if (c == 0) return (ILLEGAL_COMMAND);
    }

    // Scan token with possible embedded string literal
    else {
      argv[argc++] = bp;
      while (c > ' ' && c != '"') c = *++bp;
      if (c == '"') {
	do 
	  c = *++bp; 
	while (c != 0 && c != '"');
	if (c == 0) return (ILLEGAL_COMMAND);
	c = *++bp;
	if (c != 0 && c > ' ') return (ILLEGAL_COMMAND);
      }
    }
    *bp++ = 0;
  } while (c != 0);

  // End the argument list and check for empty commmand line
  argv[argc] = NULL;
  m_argc = argc;
  if (argc == 0) return (0);

  // Lookup shell command and call action function or script
  const command_t* cp = lookup(argv[0]);
  if (cp == NULL) return (UNKNOWN_COMMAND);
  if (m_level < (Level) pgm_read_byte(&cp->level)) return (PERMISSION_DENIED);
  m_optind = 1;
  m_optend = false;
  m_argv = argv;

  // Check if the action is a script
  const char* sp = (const char*) pgm_read_word(&cp->action);
  if (strncmp_P(SHELL_SCRIPT_MAGIC, sp, sizeof(SHELL_SCRIPT_MAGIC) - 1) == 0) 
    return (script(sp, argc, argv));

  // Otherwise call the action function
  action_fn action = (action_fn) sp;
  return (action(argc, argv));
}

int 
Shell::script(const char* sp, int argc, char* argv[])
{
  char buf[BUF_MAX];
  int line = 0;
  int res;
  char c;

  // Execute the script by copying line by line to local buffer
  sp += sizeof(SHELL_SCRIPT_MAGIC) - 1;
  do {
    // Copy command line from program memory to buffer
    char* bp = buf;
    do {
      // Fix: Should check for buffer overflow
      c = pgm_read_byte(sp++);
      if (c != '$') {
	*bp++ = c;
      }
      // Expand possible argument; $0..$9
      else {
	c = pgm_read_byte(sp++);
	if (c < '0' || c > '9') return (ILLEGAL_COMMAND);
	uint8_t ix = c - '0';
	if (ix >= argc) return (ILLEGAL_COMMAND);
	char* ap = argv[ix];
	while ((c = *ap++) != 0) *bp++ = c;
	c = pgm_read_byte(sp++);
	*bp++ = c;
      }
    } while (c != '\n' && c != 0);
    *--bp = 0;
    line += 1;

    // Execute the command and check for errors
    if ((res = execute(buf)) != 0) return (res);

    // Continue until end of script
  } while (c != 0);
  return (0);
}

int 
Shell::run(IOStream& ios)
{
  // Check first time run; will need to prompt
  if (m_firstrun) {
    prompt(ios);
    m_firstrun = false;
  }

  // Check if a command line is available
  if (ios.readline(m_buf, BUF_MAX, m_echo) == NULL) return (0);

  // Check if the command line was too long
  int res = 0;
  if (m_buf[strlen(m_buf)-1] != '\n') {
    ios << PSTR("error: too long command") << endl;
    res = -1;
  }

  // Execute and check for error return value
  else if ((res = execute(m_buf)) != 0) {
    ios << m_buf << PSTR(": ");
    if (res == PERMISSION_DENIED) ios << PSTR("permission denied");
    else if (res == UNKNOWN_COMMAND) ios << PSTR("unknown command");
    else if (res == UNKNOWN_OPTION) ios << PSTR("unknown option");
    else if (res == ILLEGAL_OPTION) ios << PSTR("illegal option");
    else ios << PSTR("illegal command");
    ios << endl;
  }

  // Prompt for the next command line
  prompt(ios);
  *m_buf = 0;
  return (res);
}

int
Shell::help(IOStream& outs)
{
  for (uint8_t i = 0; i < m_cmdc; i++) {
    str_P help = (str_P) pgm_read_word(&m_cmdtab[i].help);
    if (help == NULL) continue;
    str_P name = (str_P) pgm_read_word(&m_cmdtab[i].name);
    outs << name << ' ' << help << endl;
  }
  return (0);
}

void 
Shell::prompt(IOStream& outs)
{
  outs << m_prompt;
}
