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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IoT/ThingSpeak.hh"

// ThingSpeak server network address
#define API_THINGSPEAK_COM 184,106,153,149
static const char CRLF[] __PROGMEM = "\r\n";

ThingSpeak::Client::Client() :
  m_sock(NULL)
{
}
  
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

int 
ThingSpeak::Client::connect()
{
  uint8_t server[4] = { API_THINGSPEAK_COM };
  int res = m_sock->connect(server, 80);
  if (res != 0) return (res);
  while ((res = m_sock->isconnected()) == 0) delay(16);
  if (res < 0) return (-2);
  return (0);
}

int 
ThingSpeak::Client::disconnect()
{
  m_sock->disconnect();
  m_sock->close();
  m_sock->open(Socket::TCP, 0, 0);
  return (0);
}

ThingSpeak::Channel::Channel(Client* client, const char* key) :
  m_client(client),
  m_key(key)
{
}

int 
ThingSpeak::Channel::post(const char* entry, str_P status)
{
  // Use an iostream for the http post request
  Socket* sock = m_client->m_sock;
  IOStream page(sock);
  size_t length = strlen(entry);
  if (status != NULL) length += strlen_P((const char*) status) + 8;

  // Connect to the server
  int res = m_client->connect();
  if (res < 0) goto error;

  // Generate the http post request with entry and status
  page << PSTR("POST /update HTTP/1.1") << CRLF
       << PSTR("Host: api.thingspeak.com") << CRLF
       << PSTR("Connection: close") << CRLF
       << PSTR("X-THINGSPEAKAPIKEY: ") << m_key << CRLF
       << PSTR("Content-Type: application/x-www-form-urlencoded") << CRLF
       << PSTR("Content-Length: ") << strlen(entry) << CRLF
       << CRLF
       << (char*) entry;
  if (status != NULL) page << PSTR("&status=") << status;
  sock->flush();
  res = 0;

 error:
  // Disconnect and close the socket. Reopen for the next post (if any)
  m_client->disconnect();
  return (res);
}

void 
ThingSpeak::Entry::set_field(uint8_t id, uint16_t value, uint8_t decimals, 
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
  m_cout.print(rem, decimals, IOStream::dec);
}

void 
ThingSpeak::Entry::set_field(uint8_t id, uint32_t value, uint8_t decimals, 
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
  m_cout.print(rem, decimals, IOStream::dec);
}

ThingSpeak::TalkBack::TalkBack(Client* client, const char* key, uint16_t id) :
  m_client(client), 
  m_key(key), 
  m_id(id),
  m_first(NULL) 
{}

int 
ThingSpeak::TalkBack::execute_next_command()
{
  // Use an iostream for the http post request
  Socket* sock = m_client->m_sock;
  IOStream page(sock);

  // Connect to the server
  int res = m_client->connect();
  if (res < 0) goto error;

  // Generate the http post request with talkback id and key
  page << PSTR("POST /talkbacks/") << m_id
       << PSTR("/commands/execute?api_key=") << m_key
       << PSTR(" HTTP/1.1") << CRLF
       << PSTR("Host: api.thingspeak.com") << CRLF
       << PSTR("Connection: close") << CRLF
       << PSTR("Content-Length: 0") << CRLF
       << CRLF;
  sock->flush();

  // Wait for the reply
  while ((res = sock->available()) == 0) delay(16);
  if (res < 0) goto error;

  // Parse reply header
  Command* command;
  uint8_t length;
  char line[64];
  sock->gets(line, sizeof(line));
  res = -1;
  if (strcmp_P(line, (const char*) PSTR("HTTP/1.1 200 OK\r"))) goto error;
  do {
    sock->gets(line, sizeof(line));
  } while ((sock->available() > 0) && (strcmp_P(line, (const char*) PSTR("\r"))));
  if (sock->available() <= 0) goto error;

  // Parse reply length and command string
  sock->gets(line, sizeof(line));
  length = (uint8_t) strtol(line, NULL, 16);
  if (length <= 0) goto error;
  sock->gets(line, sizeof(line));
  line[length] = 0;

  // Lookup the command and execute. Disconnect before and the command might
  // issue an add command request
  res = -5;
  command = lookup(line);
  if (command == NULL) goto error;
  m_client->disconnect();
  command->execute();
  return (0);
  
 error:
  m_client->disconnect();
  return (res);
}

int 
ThingSpeak::TalkBack::add_command_P(str_P string, uint8_t position)
{
  // Use an iostream for the http post request
  Socket* sock = m_client->m_sock;
  IOStream page(sock);

  // Connect to the server
  int res = m_client->connect();
  if (res < 0) goto error;

  // Generate the http post request with talkback id, key, command and position
  page << PSTR("POST /talkbacks/") << m_id
       << PSTR("/commands?api_key=") << m_key
       << PSTR("&command_string=") << string;
  if (position != 0) page << PSTR("&position=") << position;
  page << PSTR(" HTTP/1.1") << CRLF
       << PSTR("Host: api.thingspeak.com") << CRLF
       << PSTR("Connection: close") << CRLF
       << PSTR("Content-Length: 0") << CRLF
       << CRLF;
  sock->flush();
  
  // Wait for the reply
  while ((res = sock->available()) == 0) delay(16);
  if (res < 0) goto error;

  // Parse reply header
  char line[64];
  sock->gets(line, sizeof(line));
  res = strcmp_P(line, (const char*) PSTR("HTTP/1.1 200 OK\r")) ? -1 : 0;
  
 error:
  m_client->disconnect();
  return (res);
}

ThingSpeak::TalkBack::Command* 
ThingSpeak::TalkBack::lookup(const char* name)
{
  for (Command* c = m_first; c != NULL; c = c->m_next)
    if (!strcmp_P(name, (const char*) c->m_string)) return (c);
  return (NULL);
}
