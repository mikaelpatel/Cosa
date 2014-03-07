/**
 * @file CosaThingSpeakTalkBack.ino
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
 * ThingSpeak talkback demonstration.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/IoT/ThingSpeak.hh"
#include "Cosa/Socket/Driver/W5100.hh"

#ifndef NDEBUG
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#else
#define TRACE(x) x
#endif

// Ethernet controller
static const char HOSTNAME[] __PROGMEM = "CosaThingSpeakTalkBack";
W5100 ethernet;

// Thingspeak client
static const char KEY[] __PROGMEM = "72PKOHOK3DKJW2TQ";
uint16_t ID = 206;
ThingSpeak::Client client;
ThingSpeak::TalkBack talkback(&client, KEY, ID);

// Thingspeak TalkBack command handler; simple trace
class Command : public ThingSpeak::TalkBack::Command {
public:
  Command(ThingSpeak::TalkBack* talkback, const char* string) : 
    ThingSpeak::TalkBack::Command(talkback, string)
  {}
  virtual void execute();
};

void 
Command::execute() 
{ 
  trace << Watchdog::millis() << ':' << m_string << endl; 
}

const char LED_ON[] __PROGMEM = "LED_ON";
Command led_on(&talkback, LED_ON);

const char LED_OFF[] __PROGMEM = "LED_OFF";
Command led_off(&talkback, LED_OFF);

const char REBOOT[] __PROGMEM = "REBOOT";
Command reboot(&talkback, REBOOT);

const char PONG[] __PROGMEM = "PONG";
Command pong(&talkback, PONG);

// Thingspeak TalkBack command handler; add command(PONG) back
// to the command queue; 
class Ping : public Command {
public:
  Ping(ThingSpeak::TalkBack* talkback, const char* string) : 
    Command(talkback, string)
  {}
  virtual void execute();
};

void 
Ping::execute() 
{ 
  Command::execute();
  m_talkback->add_command_P(PSTR("PONG"));
}

const char PING[] __PROGMEM = "PING";
Ping ping(&talkback, PING);

void setup()
{
#ifndef NDEBUG
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaThingSpeakTalkBack: started"));
#endif
  Watchdog::begin();
  TRACE(ethernet.begin_P(HOSTNAME));
  TRACE(client.begin(ethernet.socket(Socket::TCP)));
}

void loop()
{
  // Take a nap if there was nothing to execute
  if (talkback.execute_next_command() != 0) SLEEP(30);
}
