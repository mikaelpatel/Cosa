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
   * Shell command privilege levels.
   */
  enum Level {
    GUEST = 0,			//!< Read-only and limited set.
    USER = 1,			//!< Local setting and restricted set.
    ADMIN = 2			//!< Full access.
  } __attribute__((packed));

  /**
   * Shell action funtion and run error codes.
   */
  enum {
    ILLEGAL_COMMAND = -1,	//!< Illegal command.
    PERMISSION_DENIED = -2,	//!< Unsufficent privilege.
    UNKNOWN_COMMAND = -3,	//!< Command not found.
    UNKNOWN_OPTION = -4,	//!< Option not found.
    ILLEGAL_OPTION = -5		//!< Illegal option value.
  };

  /**
   * Shell command action function. Called with number arguments
   * and NULL terminated argument vector. Should return zero(0) if
   * successful otherwise a negative error code; ILLEGAL_COMMAND,
   * PERMISSION_DENIED, UNKNOWN_COMMAND, UNKNOWN_OPTION and 
   * ILLEGAL_OPTION.
   * @param[in] argc argument count.
   * @param[in] argv argument vector.
   * @return zero or negative error code.
   */
  typedef int (*action_fn)(int argc, char* argv[]);

  /**
   * Shell command descriptor with name, help string and action function.
   */
  struct command_t {
    const char* name;		//!< Shell command name string (PROGMEM).
    const char* args;		//!< Arguments to command.
    const char* help;		//!< Short description of command.
    action_fn action;		//!< Shell command action function.
    Level level;		//!< Shell command privilege level.
  };
  
  /**
   * Construct command shell with given command list and prompt.
   * @param[in] cmdc number of commands in vector (max 255).
   * @param[in] cmdtab command table (in program memory).
   * @param[in] prompt to be written to cout.
   * @param[in] help_separator help separator.
   * @param[in] gap_fill character to fill between args and help.
   */
  Shell(uint8_t cmdc, const command_t* cmdtab, const char* prompt = NULL,
	const char* help_separator = NULL, char gap_fill = DEFAULT_GAP) :
    m_cmdc(cmdc),
    m_cmdtab(cmdtab),
    m_prompt((str_P) (prompt == NULL ? DEFAULT_PROMPT : prompt)),
    m_gap_fill(gap_fill),
    m_help_separator((str_P) (help_separator == NULL ? DEFAULT_HELP_SEPARATOR : help_separator)),
    m_firstrun(true),
    m_echo(true),
    m_level(ADMIN)
  {}
  
  /**
   * Set local echo mode.
   * @param[in] mode.
   */
  void set_echo(bool mode)
  {
    m_echo = mode;
  }

  /**
   * Get local echo mode.
   * @return mode.
   */
  bool get_echo() const
  {
    return (m_echo);
  }

  /**
   * Set new prompt. Pass NULL for default prompt.
   * @param[in] prompt string in program memory.
   */
  void set_prompt(const char* prompt)
  {
    m_prompt = (str_P) (prompt == NULL ? DEFAULT_PROMPT : prompt);
  }

  /**
   * Get current prompt. Returns pointer to string in program
   * memory.
   * @return prompt.
   */
  str_P get_prompt() const
  {
    return (m_prompt);
  }
  
  /**
   * Set new privilege level.
   * @param[in] level.
   */
  void set_privilege(Level level)
  {
    m_level = level;
  }

  /**
   * Get privilege level.
   * @return level.
   */
  Level get_privilege() const
  {
    return (m_level);
  }

  /**
   * Validate privilege level.
   * @return bool.
   */
  bool is_privileged(Level level) const
  {
    return (m_level >= level);
  }

  /**
   * Reset for new session.
   */
  void reset()
  {
    m_firstrun = true;
  }

  /**
   * Set new command table and associated prompt.
   * @param[in] cmdc number of commands in vector (max 255).
   * @param[in] cmdtab command table (in program memory).
   * @param[in] prompt to be written to cout.
   */
  void set_commands(uint8_t cmdc, const command_t* cmdtab, 
		    const char* prompt = NULL)
  {
    m_cmdc = cmdc;
    m_cmdtab = cmdtab;
    m_prompt = (str_P) (prompt == NULL ? DEFAULT_PROMPT : prompt);
  }
  
  /**
   * Parse command parameter list for options. The command has the
   * format: NAME -X -XVALUE OPTION=VALUE ARGUMENT.., where X is an
   * option character with or without VALUE string, OPTION is an
   * option name (string), and ARGUMENT is the first non-option. 
   * Returns zero and option string and value if successful otherwise
   * the index of the first argument in the argument vector. 
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
   * Prompt and read line from given stream and execute command when a
   * line has been completed. Return zero if commmand was not
   * completed, one if executed or negative error code. 
   * @param[in] ios in- and output stream.
   * @return zero or negative error code.
   */
  int run(IOStream& ios);

  /**
   * Print short description of commands to the given output
   * stream. Return zero or negative error code.
   * @param[in] outs output stream.
   * @param[in] command optional command.
   * @return zero or negative error code.
   */
  int help(IOStream& outs, char* command = NULL);

  /**
   * @override Shell
   * Print prompt to given output stream. Default implementation will
   * print instance prompt string (in program memory).
   * @param[in] outs output stream.
   */
  virtual void prompt(IOStream& outs);
  
protected:
  /** Default prompt */
  static const char DEFAULT_PROMPT[] PROGMEM;

  /** Default gap fill character between command/args and help */
  static const char DEFAULT_GAP = ' ';

  /** Default help separator */
  static const char DEFAULT_HELP_SEPARATOR[] PROGMEM;

  /** Max command line buffer size */
  static const size_t BUF_MAX = 64;

  /** Max number of arguments (options and parameters) */
  static const size_t ARGV_MAX = 16;

  uint8_t m_cmdc;		//!< Number of shell commands.
  const command_t* m_cmdtab;	//!< Vector with shell command decriptors.
  str_P m_prompt;		//!< Shell prompt.
  char m_gap_fill;		//!< Gap fill character.
  str_P m_help_separator;	//!< Help separator.
  bool m_firstrun;		//!< First time run.
  bool m_echo;			//!< Echo mode.
  Level m_level;		//!< Privilege level.
  char m_buf[BUF_MAX];		//!< Command buffer.
  uint8_t m_argc;		//!< Number of arguments.
  char** m_argv;		//!< Argument vector.
  uint8_t m_optind;		//!< Next option index.
  bool m_optend;		//!< End of options.
  
  /**
   * Lookup given command name in command set. Return command entry 
   * in program memory or NULL.
   * @param[in] name of shell command.
   * @return command entry or NULL.
   */
  const command_t* lookup(char* name);

  /**
   * Execute script in program memory. Return zero or the script
   * command line number of the failed. 
   * @param[in] sp pointer to script in program memory.
   * @param[in] argc argument count.
   * @param[in] argv argument vector.
   * @return zero or script line number.
   */
  int script(const char* sp, int argc, char* argv[]);

  /**
   * Print short description of command to the given output
   * stream. Return zero or negative error code.
   * @param[in] outs output stream.
   * @param[in] column column where help begins.
   * @param[in] command command to output.
   * @return zero or negative error code.
   */
  int help_command(IOStream& outs, uint8_t column, const command_t* command);
};

/** 
 * Shell script magic marker. 
 */
#define SHELL_SCRIPT_MAGIC "#!Cosa/Shell\n" 

/**
 * Support macro to define a command.
 * Used in the form:
 *   SHELL_ACTION(command,args,help)
 *   (int argc, char* argv[])
 *   {
 *     ...
 *   }
 *
 * Example:
 *   SHELL_ACTION(echo, "[args]", "echo arguments")
 *   (int argc, char* argv[])
 *   {
 *     ...
 *   }
 *
 * @param[in] command name of command.
 * @param[in] args string of optional argument format.
 * @param[in] help string for help.
 */
#define SHELL_ACTION(command, args, help) \
  static const char command ## _NAME[] __PROGMEM = #command; \
  static const char command ## _ARGS[] __PROGMEM = args; \
  static const char command ## _HELP[] __PROGMEM = help;  \
  static int command ## _action

/**
 * Support macro to start to define a script.
 * Used in the form:
 *   SHELL_SCRIPT_BEGIN(command,args,help)
 *     SHELL_SCRIPT_MAGIC
 *     ...
 *   SHELL_SCRIPT_END(command)
 *
 * @param[in] command name of command.
 * @param[in] args string of optional argument format.
 * @param[in] help string for help.
 */
#define SHELL_SCRIPT_BEGIN(command, args, help) \
  static const char command ## _NAME[] __PROGMEM = #command; \
  static const char command ## _ARGS[] __PROGMEM = args; \
  static const char command ## _HELP[] __PROGMEM = help;  \
  static const char command ## _SCRIPT[] __PROGMEM =

/**
 * Support macro to complete definition of a script.
 * Used in the form:
 *   SHELL_SCRIPT_BEGIN(command,args,help)
 *     SHELL_SCRIPT_MAGIC
 *     ...
 *   SHELL_SCRIPT_END(command)
 *
 * @param[in] command name of command.
 */
#define SHELL_SCRIPT_END(command) \
  ; \
  static Shell::action_fn command ## _action = (Shell::action_fn) command ## _SCRIPT;

/**
 * Support macro to start the definition of commands in program memory.
 * Used in the form:
 *   SHELL_BEGIN(var)
 *     SHELL_COMMAND(command-1)
 *     ...
 *     SHELL_COMMAND(command-n)
 *   SHELL_END
 */
#define SHELL_BEGIN(var) \
  static const Shell::command_t var[] __PROGMEM = {

/**
 * Support macro to add a command item in program memory.
 * @param[in] var command reference to add.
 * @param[in] level of command.
 */
#define SHELL_COMMAND(name, level) \
  { name ## _NAME, name ## _ARGS, name ## _HELP, name ## _action, level },

/**
 * Support macro to end the definition of commands in program memory.
 */
#define SHELL_END \
  { 0, 0, 0, 0, Shell::GUEST} \
  };

#endif

