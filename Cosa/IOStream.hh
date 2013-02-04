/**
 * @file Cosa/IOStream.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * Stream::Device and/or Stream::Filter.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_IOSTREAM_HH__
#define __COSA_IOSTREAM_HH__

#include "Cosa/Types.h"

class IOStream {
public:
  /**
   * Device for in/output of characters or strings.
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
     * Write null terminated string to device. Terminating
     * null is not written.
     * @param[in] s string to write.
     * @return zero(0) or negative error code.
     */
    virtual int puts(char* s);

    /**
     * Write null terminated string from program memory to device.
     * Terminating null is not written.
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
    virtual int write(void* buf, size_t size);

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
    virtual char* gets(char *s, size_t count);

    /**
     * Read data to given buffer with given size from device.
     * @param[in] buf buffer to read into.
     * @param[in] size number of bytes to read.
     * @return number of bytes read or EOF(-1).
     */
    virtual int read(void* buf, size_t size);

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
   * Filter for device (decorator). Default implementation is a 
   * null filter.
   */
  class Filter : public Device {
  protected:
    Device* m_dev;

  public:
    Filter(Device* dev) : m_dev(dev) {}

    /**
     * Write character to device.
     * @param[in] c character to write.
     * @return character written or EOF(-1).
     */
    virtual int putchar(char c) 
    { 
      return (m_dev->putchar(c)); 
    }

    /**
     * Write null terminated string to device.
     * Terminating character is not written.
     * @param[in] s string to write.
     * @return zero(0) or negative error code.
     */
    virtual int puts(char* s)
    {
      return (m_dev->puts(s));
    }

    /**
     * Write null terminated string from program memory to device.
     * Terminating character is not written.
     * @param[in] s string in program memory to write.
     * @return zero(0) or negative error code.
     */
    virtual int puts_P(const char* s)
    {
      return (m_dev->puts_P(s));
    }

    /**
     * Write data from buffer with given size to device.
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @return number of bytes written or EOF(-1).
     */
    virtual int write(void* buf, size_t size)
    {
      return (m_dev->write(buf, size));
    }

    /**
     * Read character from device.
     * @return character or EOF(-1).
     */
    virtual int getchar()
    {
      return (m_dev->getchar());
    }

    /**
     * Read string terminated by new-line or until size into given
     * string buffer.
     * @param[in] s string buffer to read into.
     * @param[in] count max number of bytes to read.
     * @return number of characters read or EOF(-1).
     */
    virtual char* gets(char *s, size_t count)
    {
      return (m_dev->gets(s, count));
    }

    /**
     * Read data to buffer with given size from device.
     * @param[in] buf buffer to read into.
     * @param[in] size number of bytes to read.
     * @return number of bytes read or EOF(-1).
     */
    virtual int read(void* buf, size_t size)
    {
      return (m_dev->read(buf, size));
    }

    /**
     * Flush internal device buffers. Wait for device to become idle.
     * @return zero(0) or negative error code.
     */
    virtual int flush()
    {
      return (m_dev->flush());
    }
  };

  /**
   * Construct stream with given device. Default is the null device.
   * @param[in] dev stream device.
   */
  IOStream(Device* dev = &Device::null) : m_dev(dev), m_base(10) {}
  
  /**
   * Get current device.
   * @return device.
   */
  Device* get_device() 
  { 
    return (m_dev); 
  }

  /**
   * Set io stream device.
   * @param[in] dev stream device.
   * @return previous device.
   */
  Device* set_device(Device* dev) 
  { 
    Device* previous = m_dev;
    if (dev == 0) dev = &Device::null;
    m_dev = dev;
    return (previous);
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
   * Print buffer contents in given base to stream.
   * @param[in] ptr pointer to data memory.
   * @param[in] size number of bytes.
   * @param[in] base to represent value in (default 10).
   * @param[in] max number of numbers per line (default 16).
   */
  void print(void *ptr, size_t size, uint8_t base = 10, uint8_t max = 16);

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
  void print(const void *ptr) 
  { 
    print((unsigned int) ptr, 16); 
  }

  /**
   * Print character to stream.
   * @param[in] c character to print.
   */
  void print(char c) 
  { 
    m_dev->putchar(c); 
  }

  /**
   * Print string in data memory to stream.
   * @param[in] s pointer to data memory string.
   */
  void print(char* s) 
  { 
    m_dev->puts(s); 
  }

  /**
   * Print string in program memory to stream.
   * Use macro PSTR() to generate a string constants in 
   * program memory.
   * @param[in] ptr pointer to program memory string.
   */
  void print_P(const char* s) 
  { 
    m_dev->puts_P(s); 
  }

  /**
   * Print end of line to stream.
   */
  void println() 
  { 
    m_dev->putchar('\n'); 
  }

  /**
   * Format print with variable argument list. The format string
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

  /**
   * Stream manipulator function prototype. To allow implementation
   * of base change and end of line.
   * @param[in] iostream.
   * @return iostream.
   */
  typedef IOStream& (*Manipulator)(IOStream&);

  /**
   * Output operator for stream manipulator. Apply function.
   * @param[in] func iostream manipulator.
   * @return iostream.
   */
  IOStream& operator<<(Manipulator func)
  { 
    return (func(*this)); 
  }

  /**
   * Print integer as string in the current base to stream.
   * Reset base to decimal.
   * @param[in] n value to print.
   * @return iostream.
   */
  IOStream& operator<<(int n) 
  { 
    print(n, m_base); 
    m_base = 10;
    return (*this);
  }

  /**
   * Print long integer as string in the current base to stream.
   * Reset base to decimal.
   * @param[in] n value to print.
   * @return iostream.
   */
  IOStream& operator<<(long int n)
  { 
    print(n, m_base); 
    m_base = 10;
    return (*this); 
  }

  /**
   * Print unsigned integer as string in the current base to stream.
   * Reset base to decimal.
   * @param[in] n value to print.
   * @return iostream.
   */
  IOStream& operator<<(unsigned int n)
  { 
    print(n, m_base); 
    m_base = 10;
    return (*this); 
  }

  /**
   * Print unsigned long integer as string in the current base to stream.
   * Reset base to decimal.
   * @param[in] n value to print.
   * @return iostream.
   */
  IOStream& operator<<(unsigned long int n)
  { 
    print(n, m_base); 
    m_base = 10;
    return (*this); 
  }

  /**
   * Print pointer as a hexadecimal number string to stream.
   * @param[in] ptr pointer to print.
   * @return iostream.
   */
  IOStream& operator<<(void* ptr) 
  { 
    print(ptr); 
    return (*this); 
  }

  /**
   * Print program memory pointer as a hexadecimal number 
   * string to stream.
   * @param[in] ptr program memory pointer to print.
   * @return iostream.
   */
  IOStream& operator<<(const void* ptr) 
  { 
    print(ptr); 
    return (*this); 
  }

  /**
   * Print character to stream.
   * @param[in] c character to print.
   * @return iostream.
   */
  IOStream& operator<<(char c) 
  { 
    print(c); 
    return (*this); 
  }

  /**
   * Print null terminated string to stream.
   * @param[in] s string to print.
   * @return iostream.
   */
  IOStream& operator<<(char* s) 
  { 
    print(s); 
    return (*this); 
  }

  /**
   * Print null terminated string in program memory to stream.
   * @param[in] s string in program memory to print.
   * @return iostream.
   */
  IOStream& operator<<(const char* s) 
  { 
    print_P(s); 
    return (*this); 
  }

  friend IOStream& bin(IOStream& outs);
  friend IOStream& oct(IOStream& outs);
  friend IOStream& dec(IOStream& outs);
  friend IOStream& hex(IOStream& outs);

 private:
  Device* m_dev;
  uint8_t m_base;

  /**
   * Print number prefix for non decimal base.
   * @param[in] base representation.
   */
  void print_prefix(uint8_t base);
};

/**
 * Set current base to binary(2) for next operator print.
 * @param[in] outs stream to set base.
 * @return iostream.
 */
extern IOStream& bin(IOStream& outs);

/**
 * Set current base to octal(8) for next operator print.
 * @param[in] outs stream to set base.
 * @return iostream.
 */
extern IOStream& oct(IOStream& outs);

/**
 * Set current base to deciaml(10) for next operator print.
 * @param[in] outs stream to set base.
 * @return iostream.
 */
extern IOStream& dec(IOStream& outs);

/**
 * Set current base to hexadecimal(16) for next operator print.
 * @param[in] outs stream to set base.
 * @return iostream.
 */
extern IOStream& hex(IOStream& outs);

/**
 * Print end of line; carriage-return-line-feed.
 * @param[in] outs stream to print end of line to.
 * @return iostream.
 */
extern IOStream& endl(IOStream& outs);

#endif
