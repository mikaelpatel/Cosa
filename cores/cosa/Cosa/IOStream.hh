/**
 * @file Cosa/IOStream.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#ifndef COSA_IOSTREAM_HH
#define COSA_IOSTREAM_HH

#include "Cosa/Types.h"

/**
 * Basic in-/output stream support class. Requires implementation of
 * Stream::Device.
 */
class IOStream {
public:
  /** End Of File, returned when device operations fails (empty or full). */
  static const int EOF = -1;

  /** End Of Line character sequences */
  static const char CR[] PROGMEM;
  static const char LF[] PROGMEM;
  static const char CRLF[] PROGMEM;

  /**
   * Base conversion.
   */
  enum Base {
    bcd = 0,
    bin = 2,
    oct = 8,
    dec = 10,
    hex = 16
  } __attribute__((packed));

  /**
   * End of line modes.
   */
  enum Mode {
    CR_MODE = 0,
    LF_MODE = 1,
    CRLF_MODE = 2
  } __attribute__((packed));

  /**
   * Device for in/output of characters or strings.
   */
  class Device {
  public:
    /**
     * Default constructor for IOStream devices. Initiate non-blocking and
     * CRLF end of line mode.
     */
    Device() :
      m_blocking(false),
      m_eol(CR_MODE)
    {}

    /**
     * Set non-blocking mode.
     */
    void non_blocking()
    {
      m_blocking = false;
    }

    /**
     * Set blocking mode.
     */
    void blocking()
    {
      m_blocking = true;
    }

    /**
     * Is blocking mode?
     * @return bool.
     */
    bool is_blocking() const
    {
      return (m_blocking);
    }

    /**
     * Set end of line mode.
     * @param[in] mode for end of line.
     */
    void eol(Mode mode)
    {
      m_eol = mode;
    }

    /**
     * Get end of line mode.
     * @return mode.
     */
    Mode eol() const
    {
      return (m_eol);
    }

    /**
     * @override{IOStream::Device}
     * Number of bytes available (possible to read).
     * @return bytes.
     */
    virtual int available();

    /**
     * @override{IOStream::Device}
     * Number of bytes room (write without blocking).
     * @return bytes.
     */
    virtual int room();

    /**
     * @override{IOStream::Device}
     * Write character to device.
     * @param[in] c character to write.
     * @return character written or EOF(-1).
     */
    virtual int putchar(char c);

    /**
     * @override{IOStream::Device}
     * Write null terminated string to device. Terminating
     * null is not written.
     * @param[in] s string to write.
     * @return zero(0) or negative error code.
     */
    virtual int puts(const char* s);

    /**
     * @override{IOStream::Device}
     * Write null terminated string from program memory to device.
     * Terminating null is not written.
     * @param[in] s string in program memory to write.
     * @return zero(0) or negative error code.
     */
    virtual int puts(str_P s);

    /**
     * @override{IOStream::Device}
     * Write data from buffer with given size to device.
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @return number of bytes written or EOF(-1).
     */
    virtual int write(const void* buf, size_t size);

    /**
     * @override{IOStream::Device}
     * Write data from buffer in program memory with given size to device.
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @return number of bytes written or EOF(-1).
     */
    virtual int write_P(const void* buf, size_t size);

    /**
     * @override{IOStream::Device}
     * Write data from buffers in null terminated io vector.
     * @param[in] vec io vector with buffers to write.
     * @return number of bytes written or EOF(-1).
     */
    virtual int write(const iovec_t* vec);

    /**
     * @override{IOStream::Device}
     * Peek at the next character from device.
     * @return character or EOF(-1).
     */
    virtual int peekchar();

    /**
     * @override{IOStream::Device}
     * Peek for the given character in device buffer. Return number
     * of characters or EOF(-1).
     * @param[in] c character to peek for.
     * @return available or EOF(-1).
     */
    virtual int peekchar(char c);

    /**
     * @override{IOStream::Device}
     * Read character from device.
     * @return character or EOF(-1).
     */
    virtual int getchar();

    /**
     * @override{IOStream::Device}
     * Read string terminated by new-line or until size into given
     * string buffer. Returns pointer to string or NULL if empty line.
     * @param[in] s string buffer to read into.
     * @param[in] count max number of bytes to read.
     * @return string pointer or NULL.
     */
    virtual char* gets(char *s, size_t count);

    /**
     * @override{IOStream::Device}
     * Read data to given buffer with given size from device.
     * @param[in] buf buffer to read into.
     * @param[in] size number of bytes to read.
     * @return number of bytes read or EOF(-1).
     */
    virtual int read(void* buf, size_t size);

    /**
     * @override{IOStream::Device}
     * Read data to given buffers in null terminated io vector.
     * @param[in] vec io vector with buffers to read into.
     * @return number of bytes read or EOF(-1).
     */
    virtual int read(iovec_t* vec);

    /**
     * @override{IOStream::Device}
     * Flush internal device buffers. Wait for device to become idle.
     * @return zero(0) or negative error code.
     */
    virtual int flush();

    /**
     * @override{IOStream::Device}
     * Empty internal device buffers.
     */
    virtual void empty();

  protected:
    /** Blocking state */
    bool m_blocking;

    /** End of line mode */
    Mode m_eol;
  };

  /**
   * Construct stream with given device.
   * @param[in] dev stream device.
   */
  IOStream(Device* dev);
  IOStream();

  /**
   * Get current device.
   * @return device.
   */
  Device* device() const
  {
    return (m_dev);
  }

  /**
   * Set io stream device.
   * @param[in] dev stream device.
   * @return previous device.
   */
  Device* device(Device* dev)
  {
    Device* previous = m_dev;
    m_dev = dev;
    return (previous);
  }

  /**
   * Get io stream end of line string.
   * @return string for end of line.
   */
  str_P eol() const
  {
    return (m_eols);
  }

  /**
   * Get io stream end of line string.
   * @return string for end of line.
   */
  str_P EOL() const
  {
    return (m_eols);
  }

  /**
   * Set io stream end of line string.
   * @param[in] s string for end of line.
   */
  void eol(str_P s)
  {
    m_eols = s;
  }

  /**
   * Set minimum width for double numbers. The width is signed value,
   * negative for left adjustment.
   * @param[in] value width.
   * @return previous width.
   */
  int8_t width(int8_t value)
  {
    int8_t res = m_width;
    m_width = value;
    return (res);
  }

  /**
   * Set number of digits after decimal point for double numbers.
   * @param[in] value precision.
   * @return previous precision.
   */
  uint8_t precision(uint8_t value)
  {
    uint8_t res = m_prec;
    m_prec = value;
    return (res);
  }

  /**
   * Print integer as string with given base to stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(int value, Base base = dec);

  /**
   * Print long integer 32-bit value in given base torace stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(long int value, Base base = dec);

  /**
   * Print unsigned integer as string with given base to stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(unsigned int value, Base base = dec);

  /**
   * Print unsigned long integer 32-bit value in given base to stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(unsigned long int value, Base base = dec);

  /**
   * Print unsigned integer as string with given base to stream.
   * @param[in] value to print.
   * @param[in] digits to print.
   * @param[in] base to represent value in.
   */
  void print(unsigned int value, uint8_t digits, Base base);

  /**
   * Print unsigned long integer 32-bit value in given base to stream.
   * @param[in] value to print.
   * @param[in] digits to print.
   * @param[in] base to represent value in.
   */
  void print(unsigned long int value, uint8_t digits, Base base);

  /**
   * Print double with the minimum field width of the output string
   * (including the '.' and the possible sign for negative values) is
   * given in width, and prec determines the number of digits after
   * the decimal sign. width is signed value, negative for left
   * adjustment.
   * @param[in] value to print.
   * @param[in] width minimum field width.
   * @param[in] prec number of digits.
   */
  void print(double value, int8_t width, uint8_t prec);

  /**
   * Print buffer contents in given base to stream.
   * @param[in] src address prefix.
   * @param[in] ptr pointer to data memory.
   * @param[in] size number of bytes.
   * @param[in] base to represent value in (default 10).
   * @param[in] max number of numbers per line (default 16).
   */
  void print(uint32_t src, const void *ptr, size_t size,
	     Base base = dec, uint8_t max = 16);

  /**
   * Print buffer contents in given base to stream.
   * @param[in] ptr pointer to data memory.
   * @param[in] size number of bytes.
   * @param[in] base to represent value in (default 10).
   * @param[in] max number of numbers per line (default 16).
   */
  void print(const void *ptr, size_t size,
	     Base base = dec, uint8_t max = 16)
  {
    print((uint32_t) ptr, ptr, size, base, max);
  }

  /**
   * Print pointer as a hexadecimal number to stream.
   * @param[in] ptr pointer to data memory.
   */
  void print(void *ptr)
  {
    print((unsigned int) ptr, hex);
  }

  /**
   * Print pointer to program memory as a hexadecimal number to
   * stream.
   * @param[in] ptr pointer to program memory.
   */
  void print(const void *ptr)
  {
    print((unsigned int) ptr, hex);
  }

  /**
   * Print character to stream.
   * @param[in] c character to print.
   */
  void print(char c)
    __attribute__((always_inline))
  {
    if (m_dev != NULL) m_dev->putchar(c);
  }

  /**
   * Print string in data memory to stream.
   * @param[in] s pointer to data memory string.
   */
  void print(const char* s)
    __attribute__((always_inline))
  {
    if (m_dev != NULL) m_dev->puts(s);
  }

  /**
   * Print string in program memory to stream. Use macro PSTR() to generate
   * a string constants in program memory.
   * @param[in] s pointer to program memory string.
   */
  void print(str_P s)
    __attribute__((always_inline))
  {
    if (m_dev != NULL) m_dev->puts(s);
  }

  /**
   * Print end of line to stream.
   */
  void println()
    __attribute__((always_inline))
  {
    if (m_dev != NULL) m_dev->puts(m_eols);
  }

  /**
   * Format print with variable argument list. The format string
   * should be in program memory. Use the macro PSTR().
   * @param[in] format string in program memory.
   * @param[in] args variable argument list.
   */
  void vprintf(str_P format, va_list args);

  /**
   * Formated print with variable argument list. The format string
   * should be in program memory. Use the macro PSTR().
   * @param[in] format string in program memory.
   * @param[in] ... variable argument list.
   */
  void printf(str_P format, ...)
  {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
  }

  /**
   * Print contents of iostream to stream.
   * @param[in] buffer input/output buffer.
   */
  void print(IOStream::Device* buffer);

  /**
   * Flush contents of iostream to stream.
   */
  void flush()
    __attribute__((always_inline))
  {
    if (m_dev != NULL) m_dev->flush();
  }

  /**
   * Scan next token from the input stream. Returns pointer to string
   * or NULL if not stream is empty.
   * @param[in] s string buffer to read into.
   * @param[in] count max number of bytes to read.
   * @return string pointer or NULL.
   */
  char* scan(char *s, size_t count);

  /**
   * Read line into the given buffer. Characters received from the
   * device is appended to the end of the string until end of line
   * is received. The buffer pointer is returned with a complete line
   * has been received otherwise NULL. The function will append until
   * the given size of reached.
   * @param[in] buf buffer for received line.
   * @param[in] size of buffer.
   * @param[in] echo character mode (Default true).
   * @return NULL or pointer to buffer when line received.
   */
  char* readline(char* buf, size_t size, bool echo = true);

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
    m_base = dec;
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
    m_base = dec;
    return (*this);
  }

  /**
   * Print double as string with the current field min width and
   * number of decimals.
   * Reset base to decimal.
   * @param[in] n value to print.
   * @return iostream.
   */
  IOStream& operator<<(double n)
  {
    print(n, m_width, m_prec);
    m_base = dec;
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
    m_base = dec;
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
    m_base = dec;
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
    m_base = dec;
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
    m_base = dec;
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
    print(s);
    return (*this);
  }

  /**
   * Print null terminated string in program memory to stream.
   * @param[in] s string in program memory to print.
   * @return iostream.
   */
  IOStream& operator<<(str_P s)
  {
    print(s);
    return (*this);
  }

  /**
   * Print contents of iobuffer to stream.
   * @param[in] buffer input/output buffer.
   * @return iostream.
   */
  IOStream& operator<<(IOStream& buffer)
  {
    print(&buffer);
    return (*this);
  }

  /**
   * Print contents of iovector to stream.
   * @param[in] vec iovector.
   * @return iostream.
   */
  IOStream& operator<<(const iovec_t* vec)
  {
    if (m_dev != NULL) m_dev->write(vec);
    return (*this);
  }

#if !defined(COSA_IOSTREAM_STDLIB_DTOA)
  /* Faster version of standard number to string conversion */
  static char* ultoa(unsigned long __val, char *__s, int base);
  static char* ltoa(long __val, char *__s, int base);
  static char* utoa(unsigned int __val, char *__s, int base);
  static char* itoa(int __val, char *__s, int base);
#endif

  friend IOStream& bcd(IOStream& outs);
  friend IOStream& bin(IOStream& outs);
  friend IOStream& oct(IOStream& outs);
  friend IOStream& dec(IOStream& outs);
  friend IOStream& hex(IOStream& outs);
  friend IOStream& flush(IOStream& outs);

protected:
  Device* m_dev;	     //!< Delegated device.
  Base m_base;		     //!< Base for next output operator.
  int8_t m_width;	     //!< Minimum width of output string.
  uint8_t m_prec;	     //!< Number of digits after decimal sign.
  str_P m_eols;		     //!< End of line string (program memory).

  /** Maximum size of required buffer for string conversion. */
  static const size_t BUF_MAX = sizeof(uint32_t) * CHARBITS + 1;

  /**
   * Print number prefix for non decimal base.
   * @param[in] base representation.
   */
  void print_prefix(Base base);
};

/**
 * Set current base to bcd for next operator print.
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream&
bcd(IOStream& outs)
{
  outs.m_base = IOStream::bcd;
  return (outs);
}

/**
 * Set current base to binary(2) for next operator print.
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream&
bin(IOStream& outs)
{
  outs.m_base = IOStream::bin;
  return (outs);
}

/**
 * Set current base to octal(8) for next operator print.
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream&
oct(IOStream& outs)
{
  outs.m_base = IOStream::oct;
  return (outs);
}

/**
 * Set current base to deciaml(10) for next operator print.
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream&
dec(IOStream& outs)
{
  outs.m_base = IOStream::dec;
  return (outs);
}

/**
 * Set current base to hexadecimal(16) for next operator print.
 * @param[in] outs stream to set base.
 * @return iostream.
 */
inline IOStream&
hex(IOStream& outs)
{
  outs.m_base = IOStream::hex;
  return (outs);
}

/**
 * Print horizontal tab '\t'.
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream&
tab(IOStream& outs)
{
  outs.print('\t');
  return (outs);
}

/**
 * Print carriage-return-line-feed.
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream&
endl(IOStream& outs)
{
  outs.println();
  return (outs);
}

/**
 * Print end of string '\0'; null character
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream&
ends(IOStream& outs)
{
  outs.print('\0');
  return (outs);
}

/**
 * Print form feed '\f'; new page/clear screen
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream&
clear(IOStream& outs)
{
  outs.print('\f');
  return (outs);
}

/**
 * Flush buffer to device.
 * @param[in] outs stream.
 * @return iostream.
 */
inline IOStream&
flush(IOStream& outs)
{
  if (outs.m_dev != NULL) outs.m_dev->flush();
  return (outs);
}
#endif
