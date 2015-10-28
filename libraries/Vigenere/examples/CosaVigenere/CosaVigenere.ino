/**
 * @file CosaVigenere.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Cosa demonstration of Vigenere cipher.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Vigenere.h>

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Memory.h"
#include "Cosa/RTT.hh"

static const char msg[] __PROGMEM =
"The Vigenere cipher is a method of encrypting alphabetic text by\n"
"using a series of different Caesar ciphers based on the letters of\n"
"a keyword. It is a simple form of polyalphabetic substitution.\n"
"\n"
"The Vigenere cipher has been reinvented many times. The method was\n"
"originally described by Giovan Battista Bellaso in his 1553 book \n"
"La cifra del. Sig. Giovan Battista Bellaso; however, the scheme was\n"
"later misattributed to Blaise de Vigenere in the 19th century,\n"
"and is now widely known as the Vigenere cipher.\n"
"\n"
"Though the cipher is easy to understand and implement,\n"
"for three centuries it resisted all attempts to break it;\n"
"this earned it the description le chiffre indechiffrable\n"
"(French for 'the indecipherable cipher'). Many people have\n"
"tried to implement encryption schemes that are essentially\n"
"Vigenere ciphers. Friedrich Kasiski was the first to publish\n"
"a general method of deciphering a Vigenere cipher.\n";

void setup()
{
  RTT::begin();
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVigenere: started"));

  // Check memory size
  TRACE(sizeof(uart));
  TRACE(sizeof(trace));
  TRACE(free_memory());

  // Test#1: Encrypt the message and measure processing time
  trace << endl << PSTR("BASELINE") << endl;
  char c;
  const char* s = msg;
  uint8_t sum = 0;
  uint32_t start = RTT::micros();
  while ((c = pgm_read_byte(s++)) != 0)
    sum += c;
  uint32_t base = RTT::micros() - start;
  TRACE(base);
  TRACE(sum);
  char key[] = "QUEENLY";
  Vigenere<32> sender(key);
  TRACE(sizeof(sender));
  s = msg;
  sum = 0;
  start = RTT::micros();
  while ((c = pgm_read_byte(s++)) != 0)
    sum += sender.encrypt(c);
  uint32_t us = RTT::micros() - start - base;
  uint16_t len = strlen_P(msg);
  trace << len << PSTR(" bytes, ")
	<< us << PSTR(" us (")
	<< (us * 1000) / len << PSTR(" ns/byte)")
	<< endl;
  TRACE(sum);

  // Test#2: Running the message through the cryptograph
  trace << endl << PSTR("ENCRYPT MESSAGE") << endl;
  sender.restart();
  s = msg;
  while ((c = pgm_read_byte(s++)) != 0)
    trace << sender.decrypt(c);
  trace << endl;

  // Test#3: Running the message through the cryptograph
  trace << endl << PSTR("DECRYPT MESSAGE") << endl;
  Vigenere<32> receiver(key);
  sender.restart();
  s = msg;
  while ((c = pgm_read_byte(s++)) != 0)
    trace << receiver.decrypt(sender.encrypt(c));

  // Test#4: Fill buffer
  trace << endl << PSTR("ENCRYPT/DECRYPT MESSAGE") << endl;
  sender.restart();
  receiver.restart();
  char buf[128];
  memcpy_P(buf, &msg[0], sizeof(buf));
  sender.encrypt(buf, sizeof(buf));
  receiver.decrypt(buf, sizeof(buf));
  for (size_t i = 0; i < sizeof(buf); i++)
    trace << buf[i];
  trace << endl;
}

void loop()
{
  // Check amount of free memory
  TRACE(free_memory());
  ASSERT(true == false);
}
