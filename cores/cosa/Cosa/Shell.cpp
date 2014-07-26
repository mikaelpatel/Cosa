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
      if (c == 0) return (-1);
      argv[argc++] = bp;
      while (c != 0 && c != '"') c = *++bp;
      if (c == 0) return (-1);
    }
    // Scan token with possible embedded string literal
    else {
      argv[argc++] = bp;
      while (c > ' ' && c != '"') c = *++bp;
      if (c == '"') {
	do 
	  c = *++bp; 
	while (c != 0 && c != '"');
	if (c == 0) return (-1);
	c = *++bp;
	if (c != 0 && c > ' ') return (-1);
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
  if (cp == NULL) return (-1);
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
	if (c < '0' || c > '9') return (-1);
	uint8_t ix = c - '0';
	if (ix >= argc) return (-1);
	char* ap = argv[ix];
	while ((c = *ap++) != 0) *bp++ = c;
	c = pgm_read_byte(sp++);
	*bp++ = c;
      }
    } while (c != '\n' && c != 0);
    *--bp = 0;
    line += 1;
    // Execute the command and check for errors
    if (execute(buf) != 0) return (line);
    // Continue until end of script
  } while (c != 0);
  return (0);
}

int 
Shell::run(IOStream* ins, IOStream* outs)
{
  if (ins == NULL) return (-1);
  if (outs != NULL) *outs << m_prompt;
  char buf[BUF_MAX];
  int count = 0;
  // Read command line. Check that it is not too long for the buffer
  do {
    if (ins->get_device()->gets(buf, sizeof(buf)) == NULL) return (-1);
    count += 1;
  } while (buf[strlen(buf) - 1] != '\n');
  if (count != 1) {
    if (outs == NULL) return (-1);
    *outs << PSTR("error:too long command") << endl;
    return (-1);
  }
  // Check for command line echo (for the serial monitor)
  if (m_echo && outs != NULL) *outs << buf;
  // Execute the command and result code
  int res = execute(buf);
  if (res == 0 || outs == NULL) return (res);
  *outs << PSTR("error:illegal command") << endl;
  return (res);
}

int
Shell::help(IOStream& outs)
{
  for (uint8_t i = 0; i < m_cmdc; i++) {
    outs << (const char*) pgm_read_word(&m_cmdtab[i].name) << ' ';
    outs << (const char*) pgm_read_word(&m_cmdtab[i].help) << endl;
  }
  return (0);
}
