/**
 * @file CosaSDWebServer.ino
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
 * W5100 Ethernet Controller device driver example code; HTTP server
 * with GET request from file on SD/FAT16.
 *
 * @section Circuit
 * This sketch is designed for the Ethernet Shield.
 * @code
 *                       W5100/ethernet
 *                       +------------+
 * (D10)--------------29-|CSN         |
 * (D11)--------------28-|MOSI        |
 * (D12)--------------27-|MISO        |
 * (D13)--------------30-|SCK         |
 * (D2)-----[ ]-------56-|IRQ         |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Socket/Driver/W5100.hh"
#include "Cosa/INET/HTTP.hh"
#include "Cosa/SPI/Driver/SD.hh"
#include "Cosa/FS/FAT16.hh"

// HTML end of line
#define CRLF "\r\n"

// Example WebServer that responds by reading SD/FAT16 file
class WebServer : public HTTP::Server {
public:
  virtual void on_request(IOStream& page, char* method, char* path, char* query);
};

void
WebServer::on_request(IOStream& page, char* method, char* path, char* query)
{
  // Trace uptime in hour:minutes:seconds
  uint32_t uptime =  Watchdog::millis() / 1000;
  uint16_t h = uptime / 3600;
  uint8_t m = (uptime / 60) % 60;
  uint8_t s = uptime % 60;
  trace << h << ':' << m << ':' << s << ':';

  // Trace client address and port
  INET::addr_t addr;
  get_client(addr);
  INET::print_addr(trace, addr.ip, addr.port);

  // Filter path for root
  if (!strcmp_P(path, PSTR("/")))
    path = (char*) "index.htm";
  else
    path += 1;

  // Trace request to trace iostream
  trace << ':' << method << ' ' << path;
  if (query != NULL) trace << query;

  // Attempt to open the file. Report back if not found
  FAT16::File file;
  if (!file.open(path, O_READ)) {
    // Strip possible extra extension character and try again
    path[strlen(path) - 1] = 0;
    if (!file.open(path, O_READ)) {
      trace << PSTR(" Not Found") << endl;
      page << PSTR("HTTP/1.1 404 Not Found" CRLF);
      file.close();
      return;
    }
  }

  // Create response header and read the file
  trace << PSTR(" OK") << endl;
  page << PSTR("HTTP/1.1 200 OK" CRLF
	       "Content-Type: text/html" CRLF
	       "Connection: close" CRLF CRLF);

  // Use block read/write to improve performance
  static const size_t BUF_MAX = 64;
  uint8_t buf[BUF_MAX];
  int count;
  while ((count = file.read(buf, sizeof(buf))) > 0) {
    page.get_device()->write(buf, count);
  }
  file.close();
}

// Network configuration
#define IP 192,168,1,150
#define SUBNET 255,255,255,0
#define GATEWAY 192,168,1,1
#define PORT 80

// W5100 Ethernet Controller with MAC-address
static const uint8_t mac[6] __PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);
WebServer server;

// SD driver and clock configuration
SD sd(Board::D4);
#define SLOW_CLOCK SPI::DIV4_CLOCK
#define FAST_CLOCK SPI::DIV2_CLOCK
#define CLOCK FAST_CLOCK

void setup()
{
  // Initiate uart and trace output stream. And watchdog
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaSDWebServer: started"));
  Watchdog::begin();

  // Initiate ethernet controller with address and start server
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };
  ASSERT(ethernet.begin(ip, subnet));
  ASSERT(server.begin(ethernet.socket(Socket::TCP, PORT)));

  // Initiate the SD/FAT16 driver
  ASSERT(sd.begin(CLOCK));
  ASSERT(FAT16::begin(&sd));
}

void loop()
{
  // Service incoming requests
  server.run();
}

