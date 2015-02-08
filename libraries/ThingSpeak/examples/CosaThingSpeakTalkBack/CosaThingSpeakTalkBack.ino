/**
 * @file CosaThingSpeakTalkBack.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
 * ThingSpeak talkback demonstration; shows how to create a command
 * handler class, execute commands from the TalkBack command queue,
 * and push additional commands to the queue.
 *
 * @section Circuit
 * Commands LED_ON/LED_OFF will turn on and off the built-in LED.
 *
 * @section Circuit
 * This sketch is designed for the Ethernet Shield.
 *
 *                       W5100/ethernet
 *                       +------------+
 * (D10)--------------29-|CSN         |
 * (D11)--------------28-|MOSI        |
 * (D12)--------------27-|MISO        |
 * (D13)--------------30-|SCK         |
 * (D2)-----[ ]-------56-|IRQ         |
 *                       +------------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "ThingSpeak.hh"
#include "Cosa/Watchdog.hh"
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
  Command(ThingSpeak::TalkBack* talkback, str_P string) :
    ThingSpeak::TalkBack::Command(talkback, string)
  {}
  virtual void execute();
};

void
Command::execute()
{
  trace << Watchdog::millis() << ':' << m_string << endl;
}

const char REBOOT_COMMAND[] __PROGMEM = "REBOOT";
Command reboot(&talkback, REBOOT_COMMAND);

// Thingspeak TalkBack command handler; set given pin according
// to state variable
class LED : public Command {
public:
  LED(ThingSpeak::TalkBack* talkback, const char* string,
      Board::DigitalPin pin, uint8_t state) :
    Command(talkback, string),
    m_led(pin),
    m_state(state)
  {}
  virtual void execute();
private:
  OutputPin m_led;
  uint8_t m_state;
};

void
LED::execute()
{
  Command::execute();
  m_led.set(m_state);
}

const char LED_ON_COMMAND[] __PROGMEM = "LED_ON";
LED led_on(&talkback, LED_ON_COMMAND, Board::LED, 1);

const char LED_OFF_COMMAND[] __PROGMEM = "LED_OFF";
LED led_off(&talkback, LED_OFF_COMMAND, Board::LED, 0);

// Thingspeak TalkBack command handler; add given command back
// to the command queue
class Ping : public Command {
public:
  Ping(ThingSpeak::TalkBack* talkback, Command* pong) :
    Command(talkback, PSTR("PING")),
    m_pong(pong)
  {}
  virtual void execute();
private:
  Command* m_pong;
};

void
Ping::execute()
{
  Command::execute();
  m_talkback->add_command_P(m_pong->get_string());
}

const char PONG_COMMAND[] __PROGMEM = "PONG";
Command pong(&talkback, PONG_COMMAND);
Ping ping(&talkback, &pong);

void setup()
{
#ifndef NDEBUG
  // Setup trace iostream to serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaThingSpeakTalkBack: started"));
#endif
  // Start the watchdog
  Watchdog::begin();

  // Setup Ethernet controller and ThingSpeak with given ethernet socket
  TRACE(ethernet.begin_P(HOSTNAME));
  TRACE(client.begin(ethernet.socket(Socket::TCP)));

  // Add the commands
  talkback.add(&reboot);
  talkback.add(&led_on);
  talkback.add(&led_off);
  talkback.add(&ping);
  talkback.add(&pong);
}

void loop()
{
  while (talkback.execute_next_command() == 0)
    ;
  sleep(15);
}
