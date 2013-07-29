/**
 * @file CosaVigenere.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * Cosa demonstration of Vigenere cipher.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Crypto/Vigenere.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/RTC.hh"

char msg[] PROGMEM = 
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
"a general method of deciphering a Vigenere cipher.";

void setup()
{
  RTC::begin();
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVigenere: started"));

  // Test#1: Encode the message and measure processing time
  Vigenere<32> sender("QUEENLY");
  TRACE(sizeof(sender));
  char c;
  const char* s = msg;
  uint8_t sum = 0;
  uint32_t start = RTC::micros();
  while ((c = pgm_read_byte(s++)) != 0)
    sum += sender.encode(c);
  uint32_t us = RTC::micros() - start;
  uint16_t len = strlen_P(msg);
  TRACE(sum);
  trace << len << PSTR(" bytes, ")
	<< us << PSTR(" us (")
	<< (us * 1000) / len << PSTR(" ns/byte)")
	<< endl;

  // Test#2: Running the message through the encoder
  trace << PSTR("ENCODED MESSAGE") << endl;
  sender.restart();
  s = msg;
  while ((c = pgm_read_byte(s++)) != 0)
    trace << sender.encode(c);

  // Test#3: Running the message through the encoder/decoder
  trace << PSTR("DECODED MESSAGE") << endl;
  Vigenere<32> receiver("QUEENLY");
  sender.restart();
  s = msg;
  while ((c = pgm_read_byte(s++)) != 0)
    trace << receiver.decode(sender.encode(c));
}
