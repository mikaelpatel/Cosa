/**
 * @file CosaPinWebServer.ino
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
 * W5100 Ethernet Controller device driver example code; HTTP server.
 * Demonstrates usage of the Cosa HTTP::Server support class. The
 * example class WebServer will reply with HTTP page with reading of
 * digital pin(0..13), analog pin(0..3) samples, voltage (bandgap),
 * amount of free memory, uptime (seconds), number of requests, and
 * the connecting client address (MAC, IP and port). 
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

#include "Cosa/Memory.h"
#include "Cosa/InputPin.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/INET.hh"
#include "Cosa/INET/HTTP.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Socket/Driver/W5100.hh"

// HTML end of line
#define CRLF "\r\n"

// Example WebServer that responds with HTTP page with state of pins 
class WebServer : public HTTP::Server {
public:
  // Construct WebServer and initiate sequence counter
  WebServer() : m_nr(0) {}

  // Response member function
  virtual void on_request(IOStream& page, char* method, char* path, char* query);

private:
  // Request sequence number
  uint16_t m_nr;
};

void 
WebServer::on_request(IOStream& page, char* method, char* path, char* query)
{
  // Uptime in seconds
  uint32_t uptime =  Watchdog::millis() / 1000;
  uint16_t h = uptime / 3600;
  uint8_t m = (uptime / 60) % 60;
  uint8_t s = uptime % 60;

  // Get client connection information; MAC, IP address and port
  INET::addr_t addr;
  get_client(addr);

  // Reply page; header and footer are static, contents dynamic
  static const char header[] __PROGMEM = 
    "HTTP/1.1 200 OK" CRLF
    "Content-Type: text/html" CRLF
    "Connection: close" CRLF 
    "Refresh: 5" CRLF CRLF
    "<!DOCTYPE HTML>" CRLF
    "<HTML>" CRLF
    "<HEAD><TITLE>CosaPinWebServer</TITLE></HEAD>" CRLF 
    "<BODY>" CRLF;
  static const char footer[] __PROGMEM = 
    "</BODY>" CRLF 
    "</HTML>";
  static const char BR[] __PROGMEM = 
    "<BR/>" CRLF;

  // Construct the page; header-contents-footer
  page << header;

  // Digital pin status; table with green/red blocks and pin number
  page << PSTR("Digital Pin:") << BR;
  page << PSTR("<TABLE>") << endl;
  page << PSTR("<TR>") << endl;
  for (uint8_t i = 0; i < membersof(digital_pin_map); i++) {
    Board::DigitalPin pin;
    pin = (Board::DigitalPin) pgm_read_byte(digital_pin_map + i);
    page << PSTR("<TD style=\"text-align: center; background-color: ");
    if (InputPin::read(pin)) 
      page << PSTR("red\">1"); 
    else 
      page << PSTR("green\">0"); 
    page << PSTR("</TD>") << endl;
  }
  page << PSTR("</TR>") << endl;
  page << PSTR("<TR>") << endl;
  for (uint8_t i = 0; i < membersof(digital_pin_map); i++) {
    page << PSTR("<TD>D") << i << PSTR("</TD>") << endl;
  }
  page << PSTR("</TR>") << endl;
  page << PSTR("</TABLE>") << BR;

  // Analog pin reading: bar chart with percent of 1024 and pin number
  page << PSTR("Analog Pin:") << BR;
  page << PSTR("<TABLE class=\"noborders\">") << endl;
  page << PSTR("<TR>") << endl;
  for (uint8_t i = 0; i < membersof(analog_pin_map); i++) {
    Board::AnalogPin pin;
    pin = (Board::AnalogPin) pgm_read_byte(analog_pin_map + i);
    uint16_t value = (AnalogPin::sample(pin) * 100L) / 1024;
    page << PSTR("<TD style=\"text-align: center; vertical-align: bottom\">");
    page << value;
    page << PSTR("<DIV style=\"display: block;");
    page << PSTR(" background-color: red;");
    page << PSTR(" width: 20px;");
    page << PSTR(" height: ") << value << PSTR("px\">");
    page << PSTR("</DIV>");
    page << PSTR("</TD>") << endl;
  }
  page << PSTR("</TR>") << endl;
  page << PSTR("<TR>") << endl;
  for (uint8_t i = 0; i < membersof(analog_pin_map); i++) {
    page << PSTR("<TD>A") << i << PSTR("</TD>") << endl;
  }
  page << PSTR("</TR>") << endl;
  page << PSTR("</TABLE>") << endl;
  page << BR;
  
  // Battery and memory status
  page << PSTR("Vcc (mV): ") << AnalogPin::bandgap() << BR;
  page << PSTR("Memory (byte): ") << free_memory() << BR;
  page << BR;

  // Webserver status; uptime, request, mehod, path and query
  page << PSTR("Uptime (h:m:s): ") << h << ':' << m << ':' << s << BR;
  page << PSTR("Request: ") << ++m_nr << BR;
  page << PSTR("Method: ") << method << BR;
  page << PSTR("Path: ") << path << BR;
  page << PSTR("Query: "); if (query != NULL) page << query; page << BR;
  page << BR;

  // Webclient information
  page << PSTR("MAC: "); INET::print_mac(page, addr.mac); page << BR;
  page << PSTR("IP: "); INET::print_addr(page, addr.ip, addr.port); page << BR;
  page << footer;
}

// Network configuration
#define IP 192,168,1,100
#define SUBNET 255,255,255,0
#define GATEWAY 192,168,1,1
#define PORT 80

// W5100 Ethernet Controller with MAC-address
static const uint8_t mac[6] __PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);
WebServer server;

// Disable SD on Ethernet Shield
#define USE_ETHERNET_SHIELD
#if defined(USE_ETHERNET_SHIELD)
#include "Cosa/OutputPin.hh"
OutputPin sd(Board::D4, 1);
#endif

void setup()
{
  // Initiate uart and trace output stream. And watchdog
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPinWebServer: started"));
  Watchdog::begin();

  // Initiate ethernet controller with address
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };
  ASSERT(ethernet.begin(ip, subnet));

  // Start the server
  ASSERT(server.begin(ethernet.socket(Socket::TCP, PORT)));
}

void loop()
{
  // Service incoming requests
  server.run();
}
