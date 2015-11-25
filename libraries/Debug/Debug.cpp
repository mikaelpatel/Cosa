/**
 * @file Debug.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#include "Debug.hh"

Debug debug __attribute__((weak));

extern int __heap_start, *__brkval;

bool
Debug::begin(IOStream::Device* dev,
	     const char* file,
	     int line,
	     const char* func)
{
  if (m_dev != NULL) return (false);

  DATAEND =(int) &__heap_start;
  DATASIZE = DATAEND - RAMSTART;

  device(dev);
  print(PSTR("Cosa/Debug 1.0 Copyright (C) 2015\nDebug::begin:"));
  run(file, line, func);
  return (true);
}

void
Debug::assert(const char* file,
	      int line,
	      const char* func,
	      str_P cond)
{
  print(PSTR("Debug::assert"));
  run(file, line, func, cond);
  print(EXITCHARACTER);
  flush();
  exit(0);
}

void
Debug::break_at(const char* file,
		int line,
		const char* func,
		str_P cond)
{
  print(PSTR("Debug::break_at"));
  run(file, line, func, cond);
}

bool
Debug::check_stack(int room)
{
  uint16_t marker = 0xA5A5;
  int HEAPEND = (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  int STACKSTART = (int) &marker;
  return (STACKSTART > HEAPEND + room);
}

bool
Debug::end()
{
  if (m_dev == NULL) return (false);
  device(NULL);
  return (true);
}

void
Debug::run(const char* file, int line, const char* func, str_P expr)
{
  if (func != NULL) printf(PSTR(":%s:%d"), func, line);
  if (expr != NULL) printf(PSTR(":%S"), expr);
  if (func != NULL) println();

  uint16_t marker = 0xA5A5;
  str_P prompt = PSTR("Debug> ");

  while (1) {
    const size_t BUF_MAX = 32;
    char buf[BUF_MAX];
    memset(buf, 0, sizeof(buf));

    print(prompt);
    while (readline(buf, sizeof(buf)) == NULL) yield();
    size_t len = strlen(buf) - 1;
    if (buf[len] != '\n') continue;
    if (len == 0) continue;
    buf[len] = 0;

    if (!strncmp_P(buf, PSTR("go"), len)) return;

#if !defined(COSA_DEBUG_NO_BACKTRACE)
    if (!strncmp_P(buf, PSTR("backtrace"), len)) {
      printf(PSTR("%p:%s\n"), &marker, func);
      do_backtrace(func);
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_PRINT_DATA)
    if (!strncmp_P(buf, PSTR("data"), len)) {
      do_print_data();
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_PRINT_HEAP)
    if (!strncmp_P(buf, PSTR("heap"), len)) {
      do_print_heap();
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_HELP)
    if (!strncmp_P(buf, PSTR("help"), len)) {
      do_help();
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_MEMORY_USAGE)
    if (!strncmp_P(buf, PSTR("memory"), len)) {
      do_memory_usage((int) &marker);
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_PRINT_STACK)
    if (!strncmp_P(buf, PSTR("stack"), len)) {
      do_print_stack((int) &marker);
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_PRINT_VARIABLES)
    if (!strncmp_P(buf, PSTR("variables"), len)) {
      do_print_variables();
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_QUIT)
    if (!strncmp_P(buf, PSTR("quit"), len)) {
      print(EXITCHARACTER);
      flush();
      exit(0);
    }
#endif

#if !defined(COSA_DEBUG_NO_WHERE)
    if (!strncmp_P(buf, PSTR("where"), len)) {
      printf(PSTR("%s:%d:%s\n"), file, line, func);
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_LOOKUP_VARIABLES)
    if (*buf == '?') {
      const char* name = buf + 1;
      if (!do_lookup_variables(name))
	printf(PSTR("%s: unknown variable\n"), name);
      continue;
    }
#endif
    printf(PSTR("%s: unknown command\n"), buf);
  }
}

#if !defined(COSA_DEBUG_NO_BACKTRACE)
void
Debug::do_backtrace(const char* func)
{
  for (Variable* vp = m_var; vp != NULL; vp = vp->m_next) {
    if (func != vp->m_func) {
      func = vp->m_func;
      printf(PSTR("%p:%s\n"), vp, vp->m_func);
    }
  }
}
#endif

#if !defined(COSA_DEBUG_NO_HELP)
void
Debug::do_help()
{
  static const char help[] PROGMEM =
#if !defined(COSA_DEBUG_NO_LOOKUP_VARIABLES)
    "?VARIABLE -- print variable(s)\n"
#endif
#if !defined(COSA_DEBUG_NO_BACKTRACE)
    "backtrace -- print call stack\n"
#endif
#if !defined(COSA_DEBUG_NO_PRINT_DATA)
    "data -- print data\n"
#endif
    "go -- return to sketch\n"
#if !defined(COSA_DEBUG_NO_PRINT_HEAP)
    "heap -- print heap\n"
#endif
#if !defined(COSA_DEBUG_NO_MEMORY_USAGE)
    "memory -- print memory usage\n"
#endif
#if !defined(COSA_DEBUG_NO_QUIT)
    "quit -- exit sketch\n"
#endif
#if !defined(COSA_DEBUG_NO_PRINT_STACK)
    "stack -- print stack\n"
#endif
#if !defined(COSA_DEBUG_NO_PRINT_REGISTER)
    "variables -- print variables\n"
#endif
#if !defined(COSA_DEBUG_NO_WHERE)
    "where -- location in source code\n"
#endif
    ;
  print((str_P) help);
}
#endif

#if !defined(COSA_DEBUG_NO_LOOKUP_VARIABLES)
bool
Debug::do_lookup_variables(const char* name)
{
  bool found = false;
  for (Variable* vp = m_var; vp != NULL; vp = vp->m_next) {
    if (strcmp_P(name, vp->m_name) == 0) {
      vp->print();
      found = true;
    }
  }
  return (found);
}
#endif

#if !defined(COSA_DEBUG_NO_MEMORY_USAGE)
void
Debug::do_memory_usage(int marker)
{
  int HEAPEND = (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  printf(PSTR("data=%d,heap=%d,stack=%d,free=%d\n"),
	 DATASIZE,
	 HEAPEND - (int) &__heap_start,
	 RAMEND - marker + 1,
	 marker - HEAPEND);
}
#endif

#if !defined(COSA_DEBUG_NO_PRINT_DATA)
void
Debug::do_print_data()
{
  print(RAMSTART, (void*) RAMSTART, DATASIZE, IOStream::hex);
}
#endif

#if !defined(COSA_DEBUG_NO_PRINT_HEAP)
void
Debug::do_print_heap()
{
  int HEAPSTART = (int) &__heap_start;
  int HEAPEND = (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  int size = HEAPEND - HEAPSTART;
  if (size == 0) return;
  print(HEAPSTART, (void*) HEAPSTART, size, IOStream::hex);
}
#endif

#if !defined(COSA_DEBUG_NO_PRINT_STACK)
void
Debug::do_print_stack(int marker)
{
  int size = RAMEND - marker + 1;
  if (size == 0) return;
  print(marker, (void*) marker, size, IOStream::hex);
}
#endif

#if !defined(COSA_DEBUG_NO_PRINT_VARIABLES)
void
Debug::do_print_variables()
{
  for (Variable* vp = m_var; vp != NULL; vp = vp->m_next) vp->print();
}
#endif

void
Debug::Variable::print()
{
  debug.printf(PSTR("%p:%s:%S@%p"), this, m_func, m_name, m_ref);
  if (m_size == 1) {
    debug.printf(PSTR("=%d\n"), *((uint8_t*) m_ref));
  }
  else if (m_size == 2) {
    debug.printf(PSTR("=%d\n"), *((int*) m_ref));
  }
  else {
    debug.printf(PSTR("[%d]:"), m_size);
    if (m_size > 16) debug.println();
    debug.print((uint32_t) m_ref, m_ref, m_size, IOStream::hex);
  }
}
