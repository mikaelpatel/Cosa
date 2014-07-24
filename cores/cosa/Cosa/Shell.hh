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
    const char* name;		//!< Shell command name string (PROGMEM).
    action_fn action;		//!< Shell command action function.
    const char* help;		//!< Short description.
  };
  
  /**
   * Construct command shell with given command list and prompt.
   * @param[in] cmdc number of commands in vector.
   * @param[in] cmdv command vector.
   * @param[in] prompt to be written to cout.
   */
  Shell(uint8_t cmdc, const command_t* cmdv, const char* prompt = NULL) :
    m_cmdc(cmdc),
    m_cmdv(cmdv),
    m_prompt(prompt == NULL ? DEFAULT_PROMPT : prompt),
    m_echo(true)
  {}
  
  /**
   * Set command line echo mode. Useful for Arduino Serial Monitor to 
   * echo commands received.
   * @param[in] flag on or off.
   */
  void set_echo(bool flag)
  {
    m_echo = flag;
  }

  /**
   * Parse command parameter list for options. The command has the
   * format: NAME -X -XVALUE OPTION=VALUE ARGUMENT.., where X is an
   * option character with or without VALUE string, OPTION is an
   * option name (string), and ARGUMENT is the first
   * non-option. Returns zero and option string and value if
   * successful otherwise the index of the first argument in the
   * argument vector. 
   * @param[out] option string.
   * @param[out] value string.
   * @return zero or index of first argument.
   */
  int get(char* &option, char* &value);

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
   * Read command lines from script in program memory. Return zero or 
   * the script command line number of the failed.
   * @param[in] script pointer to script in program memory.
   * @return zero or script line number.
   */
  int execute_P(const char* script);

  /**
   * Prompt to given output stream (if not NULL), read line from given
   * input stream and execute command. Return zero or negative error
   * code. 
   * @param[in] ins input stream.
   * @param[in] outs output stream (default NULL).
   * @return zero or negative error code.
   */
  int run(IOStream* ins, IOStream* outs = NULL);

  /**
   * Print short description of commands to the given output
   * stream. Return zero or negative error code. 
   * @param[in] outs output stream.
   * @return zero or negative error code.
   */
  int help(IOStream& outs);

protected:
  /** Default prompt */
  static const char DEFAULT_PROMPT[] PROGMEM;

  /** Command line size */
  static const size_t BUF_MAX = 64;

  uint8_t m_cmdc;		//!< Number of shell commands.
  const command_t* m_cmdv;	//!< Vector with shell command decriptors.
  const char* m_prompt;		//!< Shell prompt.
  bool m_echo;			//!< Echo command line.
  uint8_t m_argc;		//!< Number of arguments.
  char** m_argv;		//!< Argument vector.
  uint8_t m_optind;		//!< Next option index.
  bool m_optend;		//!< End of options.
  
  /**
   * Lookup given command name in command set. Return command index or
   * negative error code.
   * @param[in] name of shell command.
   * @return index or negative error code.
   */
  int lookup(char* name);
};

/** 
 * Shell script magic marker.
 */
#define SHELL_SCRIPT_MAGIC "#!Cosa/Shell\n" 

#endif

