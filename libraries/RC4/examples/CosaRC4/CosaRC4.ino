/**
 * @file CosaRC4.ino
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
 * Cosa demonstration of RC4 cipher.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <RC4.h>

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Memory.h"

static const char msg[] __PROGMEM =
"RC4 was designed by Ron Rivest of RSA Security in 1987. While it is\n"
"officially termed Rivest Cipher 4, the RC acronym is alternatively\n"
"understood to stand for Ron's Code.\n"
"\n"
"RC4 was initially a trade secret, but in September 1994 a description\n"
"of it was anonymously posted to the Cypherpunks mailing list.[4] It was\n"
"soon posted on the sci.crypt newsgroup, and from there to many sites on\n"
"the Internet. The leaked code was confirmed to be genuine as its output\n"
"was found to match that of proprietary software using licensed RC4.\n"
"Because the algorithm is known, it is no longer a trade secret.\n"
"The name RC4 is trademarked, so RC4 is often referred to as ARCFOUR\n"
"or ARC4 (meaning alleged RC4) to avoid trademark problems. RSA Security\n"
"has never officially released the algorithm; Rivest has, however,\n"
"linked to the English Wikipedia article on RC4 in his own course notes.\n"
"RC4 has become part of some commonly used encryption protocols and\n"
"standards, including WEP and WPA for wireless cards and TLS.\n"
"\n"
"The main factors in RC4's success over such a wide range of applications\n"
"are its speed and simplicity: efficient implementations in both software\n"
"and hardware are very easy to develop.\n";

/**
 * OpenSSL test vectors.
 * https://github.com/openssl/openssl/blob/master/crypto/rc4/rc4test.c
 * Note that these are not stored in PROGMEM and take a lot of SRAM.
 */
static unsigned char keys[7][30] = {
  {8,0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef},
  {8,0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef},
  {8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
  {4,0xef,0x01,0x23,0x45},
  {8,0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef},
  {4,0xef,0x01,0x23,0x45},
};

static unsigned char data_len[7] = { 8,8,8,20,28,10 };
static unsigned char data[7][30] = {
  {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xff},
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff},
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff},
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0xff},
  {0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0,
   0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0,
   0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0,
   0x12,0x34,0x56,0x78,0xff},
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff},
  {0},
};

static unsigned char output[7][30] = {
  {0x75,0xb7,0x87,0x80,0x99,0xe0,0xc5,0x96,0x00},
  {0x74,0x94,0xc2,0xe7,0x10,0x4b,0x08,0x79,0x00},
  {0xde,0x18,0x89,0x41,0xa3,0x37,0x5d,0x3a,0x00},
  {0xd6,0xa1,0x41,0xa7,0xec,0x3c,0x38,0xdf,
   0xbd,0x61,0x5a,0x11,0x62,0xe1,0xc7,0xba,
   0x36,0xb6,0x78,0x58,0x00},
  {0x66,0xa0,0x94,0x9f,0x8a,0xf7,0xd6,0x89,
   0x1f,0x7f,0x83,0x2b,0xa8,0x33,0xc0,0x0c,
   0x89,0x2e,0xbe,0x30,0x14,0x3c,0xe2,0x87,
   0x40,0x01,0x1e,0xcf,0x00},
  {0xd6,0xa1,0x41,0xa7,0xec,0x3c,0x38,0xdf,0xbd,0x61,0x00},
  {0},
};

void setup()
{
  RTT::begin();
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaRC4: started"));
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
  const char key[] = "QUEENLY";
  RC4 sender(key, strlen(key));
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
  sender.restart(key, strlen(key));
  s = msg;
  while ((c = pgm_read_byte(s++)) != 0)
    trace << sender.decrypt(c);
  trace << endl;

  // Test#3: Running the message through the cryptograph
  trace << endl << PSTR("DECRYPT MESSAGE") << endl;
  RC4 receiver(key, strlen(key));
  sender.restart(key, strlen(key));
  s = msg;
  while ((c = pgm_read_byte(s++)) != 0)
    trace << receiver.decrypt(sender.encrypt(c));
  trace << endl;

  // Test#4: Fill buffer and encrypt/decrypt before print
  trace << endl << PSTR("ENCRYPT MESSAGE") << endl;
  sender.restart(key, strlen(key));
  receiver.restart(key, strlen(key));
  char buf[128];
  memcpy_P(buf, &msg[0], sizeof(buf));
  sender.encrypt(buf, sizeof(buf));
  receiver.decrypt(buf, sizeof(buf));
  for (size_t i = 0; i < sizeof(buf); i++)
    trace << buf[i];
  trace << endl;

  // Test#5: OpenSSL test vectors
  trace << endl << PSTR("OPENSSL TEST VECTORS") << endl;
  bool failed = false;
  for (uint8_t i = 0; i < 7; i++) {
    sender.restart(&keys[i][1], keys[i][0]);
    memcpy(buf, data[i], data_len[i]);
    sender.encrypt(buf, data_len[i]);
    if (memcmp(buf, output[i], data_len[i])) {
      trace << i << PSTR(": TEST FAILED") << endl;
      failed = true;
    }
    receiver.restart(&keys[i][1], keys[i][0]);
    receiver.decrypt(buf, data_len[i]);
    if (memcmp(buf, data[i], data_len[i])) {
      trace << i << PSTR(": TEST FAILED") << endl;
      failed = true;
    }
  }
  if (failed) return;
  trace << PSTR("OK") << endl;
}

void loop()
{
  // Check amount of free memory
  TRACE(free_memory());
  ASSERT(true == false);
}
