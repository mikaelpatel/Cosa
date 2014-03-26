/**
 * @file Cosa/INET/NTP.cpp
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

#include "Cosa/INET/NTP.hh"
#include "Cosa/Watchdog.hh"

NTP::NTP(Socket* sock, uint8_t server[4], int8_t zone) : 
  m_sock(sock), 
  m_zone(zone) 
{
  memcpy(m_server, server, sizeof(m_server));
}

NTP::~NTP() 
{ 
  m_sock->close(); 
}

clock_t
NTP::time()
{
  const int PACKET_MAX = 48;      
  uint8_t packet[PACKET_MAX];  
  int res;
  memset(packet, 0, PACKET_MAX); 
  packet[0] = 0b11100011;
  packet[1] = 0;
  packet[2] = 6;
  packet[3] = 0xEC;
  packet[12] = 49; 
  packet[13] = 0x4E;
  packet[14] = 49;
  packet[15] = 52;		   
  res = m_sock->send(packet, sizeof(packet), m_server, PORT);
  Watchdog::delay(TIMEOUT);
  uint8_t dest[4];
  uint16_t port;
  res = m_sock->recv(packet, sizeof(packet), dest, port);
  if (res != sizeof(packet)) return (0L);
  int32_t* tp = (int32_t*) &packet[40];
  return (ntoh(*tp) + (m_zone * 3600L));
}

int 
NTP::gettimeofday(time_t& time)
{
  clock_t clock = this->time();
  if (clock == 0L) return (-1);
  time = clock;
  return (0);
}
