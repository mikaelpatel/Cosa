/**
 * @file Cosa/IoT/ThingSpeak.cpp
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IoT/ThingSpeak.hh"
#include "Cosa/Watchdog.hh"

// ThingSpeak server network address
#define API_THINGSPEAK_COM 184,106,153,149
static const char CRLF[] __PROGMEM = "\r\n";

ThingSpeak::Client::Client() :
  m_sock(NULL)
{}
  
ThingSpeak::Client::~Client()
{
  end();
}

bool 
ThingSpeak::Client::begin(Socket* sock)
{
  if (m_sock != NULL) return (false);
  m_sock = sock;
  return (true);
}

bool 
ThingSpeak::Client::end()
{
  if (m_sock == NULL) return (false);
  m_sock->close();
  m_sock = NULL;
  return (true);
}

ThingSpeak::Channel::Channel(Client* client, const char* key) :
  m_client(client),
  m_key(key)
{}

int 
ThingSpeak::Channel::post(const char* fields)
{
  // Use an iostream for the http post request
  Socket* sock = m_client->m_sock;
  IOStream page(sock);

  // Connect to the server
  uint8_t server[4] = { API_THINGSPEAK_COM };
  int res = sock->connect(server, 80);
  if (res != 0) goto error;
  while ((res = sock->isconnected()) == 0) Watchdog::delay(16);
  if (res == 0) res = -3;
  if (res < 0) goto error;

  // Generate the http post request
  page << PSTR("POST /update HTTP/1.1") << CRLF
       << PSTR("Host: api.thingspeak.com") << CRLF
       << PSTR("Connection: close") << CRLF
       << PSTR("X-THINGSPEAKAPIKEY: ") << m_key << CRLF
       << PSTR("Content-Type: application/x-www-form-urlencoded") << CRLF
       << PSTR("Content-Length: ") << strlen(fields) << CRLF
       << CRLF
       << (char*) fields;
  sock->flush();
  res = 0;

 error:
  // Disconnect and close the socket. Reopen for the next post (if any)
  sock->disconnect();
  sock->close();
  sock->open(Socket::TCP, 0, 0);
  return (res);
}

#include "Cosa/Trace.hh"

void 
ThingSpeak::Update::set_field(uint8_t id, uint16_t value, uint8_t decimals, 
			      bool sign)
{
  uint16_t scale = 1;
  for (uint8_t i = 0; i < decimals; i++) scale *= 10;
  if (!m_buf.is_empty()) m_cout << '&';
  m_cout << PSTR("field") << id << '=';
  if (sign) m_cout << '-';
  m_cout << value / scale;  
  if (decimals == 0) return;
  uint16_t rem = value % scale;
  m_cout << '.';
  scale /= 10;
  while (scale != 0 && scale > rem) {
    m_cout << '0';
    scale /= 10;
  }
  if (rem != 0) m_cout << rem;
}

void 
ThingSpeak::Update::set_field(uint8_t id, uint32_t value, uint8_t decimals, 
			      bool sign)
{
  uint16_t scale = 1;
  for (uint8_t i = 0; i < decimals; i++) scale *= 10;
  if (!m_buf.is_empty()) m_cout << '&';
  m_cout << PSTR("field") << id << '=';
  if (sign) m_cout << '-';
  m_cout << value / scale;  
  if (decimals == 0) return;
  uint16_t rem = value % scale;
  m_cout << '.';
  scale /= 10;
  while (scale != 0 && scale > rem) {
    m_cout << '0';
    scale /= 10;
  }
  if (rem != 0) m_cout << rem;
}


