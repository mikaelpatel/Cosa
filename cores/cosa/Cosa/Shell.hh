/**
 * @file Cosa/Shell.hh
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

#ifndef COSA_SHELL_HH
#define COSA_SHELL_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

class Shell {
public:
  /**
   * Shell command action function. Called with number arguments
   * and NULL terminated argument vector. Should return zero(0) if
   * successful otherwise a negative error code.
   * @param[in] argc argument count.
   * @param[in] argv argument vector.
   * @return zero or negative error code.
   */
  typedef int (*action_fn)(int argc, char* argv[]);

  /**
   * Shell command descriptor.
   */
  struct command_t {
    int argc;			//!< Required number of parameters.
    const char* name;		//!< Shell command name string (PROGMEM).
    action_fn action;		//!< Shell command action function.
    const char* help;		//!< Short description.
  };
  
  Shell(uint8_t cmdc, const command_t* cmdv, const char* prompt = NULL) :
    m_cmdc(cmdc),
    m_cmdv(cmdv),
    m_prompt(prompt == NULL ? DEFAULT_PROMPT : prompt)
  {}
  
  /**
   * Parse buffer and create command, option and parameter
   * list. Lookup command in given command vector. If found call
   * action function with arguments and count. Return value from
   * action function or negative error code if not found.
   * @param[in] buf command line (zero terminated string).
   * @return value from action function or negative error code.
   */
  int execute(char* buf);

  /**
   * Prompt to given output stream (if not NULL), read line from given
   * input stream and execute command. Return zero or negative error
   * code. 
   * @param[in] ins input stream.
   * @param[in] outs output stream (default NULL).
   * @return zero or negative error code.
   */
  int run(IOStream* ins, IOStream* outs = NULL);

protected:
  static const char DEFAULT_PROMPT[] PROGMEM;
  
  uint8_t m_cmdc;		//!< Number of shell commands.
  const command_t* m_cmdv;	//!< Vector with shell command decriptors.
  const char* m_prompt;		//!< Shell prompt.

  /**
   * Lookup given command name in command set. Return command index or
   * negative error code.
   * @param[in] name of shell command.
   * @return index or negative error code.
   */
  int lookup(char* name);
};
#endif

