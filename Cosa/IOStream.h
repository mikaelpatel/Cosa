/**
 * @file Cosa/IOStream.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Basic in-/output stream support class. Requires implementation of
 * Stream::Device.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_IOSTREAM_H__
#define __COSA_IOSTREAM_H__

#include "Cosa/Types.h"
#include <stdarg.h>

class IOStream {

public:
  /**
   * Device for output/input of character or string.
   */
  class Device {
  public:
    /**
     * Write character to device.
     * @param[in] c character to write.
     * @return character written or EOF(-1).
     */
    virtual int putchar(char c);

    /**
     * Write null terminated string to device.
     * @param[in] s string to write.
     * @return zero(0) or negative error code.
     */
    virtual int puts(char* s);

    /**
     * Write null terminated string from program memory to device.
     * @param[in] s string in program memory to write.
     * @return zero(0) or negative error code.
     */
    virtual int puts_P(const char* s);

    /**
     * Write data from buffer with given size to device.
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @return number of bytes written or EOF(-1).
     */
    virtual int write(void* buf, uint8_t size);

    /**
     * Read character from device.
     * @return character or EOF(-1).
     */
    virtual int getchar();

    /**
     * Read string terminated by new-line or until size into given
     * string buffer.
     * @param[in] s string buffer to read into.
     * @param[in] count max number of bytes to read.
     * @return number of characters read or EOF(-1).
     */
    virtual char* gets(char *s, uint8_t count);

    /**
     * Read data from buffer with given size from device.
     * @param[in] buf buffer to read into.
     * @param[in] size number of bytes to read.
     * @return number of bytes read or EOF(-1).
     */
    virtual int read(void* buf, uint8_t size);

    /**
     * Flush internal device buffers. Wait for device to become idle.
     * @return zero(0) or negative error code.
     */
    virtual int flush();

    /**
     * The default implementation of device.
     */
    static Device null;
  };

  /**
   * Construct stream with given device.
   * @param[in] dev stream device.
   */
  IOStream(Device* dev = &Device::null) : _dev(dev) {}
  
  /**
   * Get current device.
   * @return device
   */
  Device* get_device() 
  { 
    return (_dev); 
  }

  /**
   * Set io stream device.
   * @param[in] dev stream device.
   */
  void set(Device* dev) 
  { 
    _dev = dev;
  }

  /**
   * Print integer as string with given base to stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(int value, uint8_t base = 10);

  /**
   * Print long integer 32-bit value in given base torace stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(long int value, uint8_t base = 10);

  /**
   * Print unsigned integer as string with given base to stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(unsigned int value, uint8_t base = 10);

  /**
   * Print unsigned long integer 32-bit value in given base to stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(unsigned long int value, uint8_t base = 10);

  /**
   * Print pointer as a hexidecimal number to stream.
   * @param[in] ptr pointer to data memory.
   */
  void print(void *ptr) { print((unsigned int) ptr, 16); }

  /**
   * Print pointer to program memory as a hexidecimal number to 
   * stream.
   * @param[in] ptr pointer to program memory.
   */
  void print(const void *ptr) { print((unsigned int) ptr, 16); }

  /**
   * Print character to stream.
   * @param[in] c character to print.
   */
  void print(char c) { _dev->putchar(c); }

  /**
   * Print string in data memory to stream.
   * @param[in] ptr pointer to data memory string.
   */
  void print(char* s) { _dev->puts(s); }

  /**
   * Print string in program memory to stream.
   * Use macro PSTR() to generate a string constants in 
   * program memory.
   * @param[in] ptr pointer to program memory string.
   */
  void print_P(const char* s) { _dev->puts_P(s); }

  /**
   * Print end of line to stream.
   */
  void println() { _dev->putchar('\n'); }

  /**
   * Formated print with variable argument list. The format string
   * should be in program memory. Use the macro PSTR().
   * @param[in] format string in program memory.
   * @param[in] args variable argument list.
   */
  void vprintf_P(const char* format, va_list args);

  /**
   * Formated print with variable argument list. The format string
   * should be in program memory. Use the macro PSTR().
   * @param[in] format string in program memory.
   * @param[in] ... variable argument list.
   */
  void printf_P(const char* format, ...)
  {
    va_list args;
    va_start(args, format);
    vprintf_P(format, args);
    va_end(args);
  }

 private:
  Device* _dev;

  /**
   * Print number prefix for non decimal base.
   * @param[in] base representation.
   */
  void print_prefix(uint8_t base);
};

#endif
