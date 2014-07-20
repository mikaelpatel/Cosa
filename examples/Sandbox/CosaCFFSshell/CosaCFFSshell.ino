/**
 * @file CosaCFFSshell.ino
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
 * @section Description
 * Tiny command shell to demonstration the S25FL127S SPI Flash Memory
 * device driver and file system.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/FS/CFFS.hh"
#include "Cosa/Time.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

#if defined(ANARDUINO_MINIWIRELESS)
OutputPin rf_cs(Board::D10, 1);
#endif

S25FL127S flash;
IOStream cout;

void setup()
{
  Watchdog::begin();
  RTC::begin();
  uart.begin(9600);
  uart.set_blocking(SLEEP_MODE_IDLE);
  cout.set_device(&uart);
  flash.begin();
  CFFS::begin(&flash);
}

void loop()
{
  static bool echo = false;
  const size_t BUF_MAX = 256;
  char buf[BUF_MAX];
  char* filename;
  int res = 0;
  
  cout << PSTR("arduino:$ ");
  if (uart.gets(buf, sizeof(buf)) == NULL) return;
  if (echo) cout << buf << endl;

  if (strncmp_P(buf, PSTR("cat"), 3) == 0) {
    CFFS::File file;
    filename = buf + 4;
    res = file.open(filename, O_READ);
    if (res == 0) {
      IOStream::Device* dev = cout.get_device();
      int res;
      while ((res = file.read(buf, sizeof(buf))) > 0)
	dev->write(buf, res);
    }
    else {
      cout << PSTR("cat");
    }
  }

  else if (strncmp_P(buf, PSTR("cd"), 2) == 0) {
    filename = buf + 3;
    res = CFFS::cd(filename);
    if (res != 0) {
      cout << PSTR("cd");
    }
  }

  else if (strncmp_P(buf, PSTR("echo"), 4) == 0) {
    echo = strcmp_P(buf + 5, PSTR("off"));
  }

  else if (strcmp_P(buf, PSTR("ls")) == 0) {
    CFFS::ls(cout);
  }

  else if (strncmp_P(buf, PSTR("od"), 2) == 0) {
    CFFS::File file;
    filename = buf + 3;
    res = file.open(filename, O_READ);
    if (res == 0) {
      while (file.read(buf, sizeof(buf)) > 0) 
	cout.print(buf, sizeof(buf), IOStream::hex, 128 + 16);
    }
    else {
      cout << PSTR("od");
    }
  }
  else if (strcmp_P(buf, PSTR("time")) == 0) {
    time_t now(RTC::seconds());
    cout << now << endl;
  }

  if (res == 0) return;
  cout << PSTR(": ") << filename << PSTR(": No such file or directory") << endl;
}
