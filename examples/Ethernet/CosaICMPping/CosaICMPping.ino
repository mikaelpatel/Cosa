/**
 * @file CosaICMPping.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * W5100 Ethernet Controller device driver example code; ICMP ping.
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

#include "Cosa/INET.hh"
#include "Cosa/Socket/Driver/W5100.hh"

#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Disable SD on Ethernet Shield
#define USE_ETHERNET_SHIELD
#if defined(USE_ETHERNET_SHIELD)
#include "Cosa/OutputPin.hh"
OutputPin sd(Board::D4, 1);
#endif

// Network configuration
#define MAC 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed
#define IP 192,168,1,150
#define SUBNET 255,255,255,0
static const uint8_t mac[6] __PROGMEM = { MAC };
static const char hostname[] __PROGMEM = "CosaICMPping";

// W5100 Ethernet Controller
W5100 ethernet(mac);

struct icmphdr
{
  uint8_t type;			/* message type */
  uint8_t code;			/* type sub-code */
  uint16_t checksum;
  union
  {
    struct
    {
      uint16_t	id;
      uint16_t	seq;
    } echo;			/* echo datagram */
    uint32_t	gateway;	/* gateway address */
    struct
    {
      uint16_t	reserved;
      uint16_t	mtu;
    } frag;			/* path mtu discovery */
  };
  uint32_t    timestamp;
};

#define ICMP_ECHOREPLY		0	/* Echo Reply			*/
#define ICMP_DEST_UNREACH	3	/* Destination Unreachable	*/
#define ICMP_SOURCE_QUENCH	4	/* Source Quench		*/
#define ICMP_REDIRECT		5	/* Redirect (change route)	*/
#define ICMP_ECHO		8	/* Echo Request			*/
#define ICMP_TIME_EXCEEDED	11	/* Time Exceeded		*/
#define ICMP_PARAMETERPROB	12	/* Parameter Problem		*/
#define ICMP_TIMESTAMP		13	/* Timestamp Request		*/
#define ICMP_TIMESTAMPREPLY	14	/* Timestamp Reply		*/
#define ICMP_INFO_REQUEST	15	/* Information Request		*/
#define ICMP_INFO_REPLY		16	/* Information Reply		*/
#define ICMP_ADDRESS		17	/* Address Mask Request		*/
#define ICMP_ADDRESSREPLY	18	/* Address Mask Reply		*/
#define NR_ICMP_TYPES		18

/* Codes for UNREACH. */
#define ICMP_NET_UNREACH	0	/* Network Unreachable		*/
#define ICMP_HOST_UNREACH	1	/* Host Unreachable		*/
#define ICMP_PROT_UNREACH	2	/* Protocol Unreachable		*/
#define ICMP_PORT_UNREACH	3	/* Port Unreachable		*/
#define ICMP_FRAG_NEEDED	4	/* Fragmentation Needed/DF set	*/
#define ICMP_SR_FAILED		5	/* Source Route failed		*/
#define ICMP_NET_UNKNOWN	6
#define ICMP_HOST_UNKNOWN	7
#define ICMP_HOST_ISOLATED	8
#define ICMP_NET_ANO		9
#define ICMP_HOST_ANO		10
#define ICMP_NET_UNR_TOS	11
#define ICMP_HOST_UNR_TOS	12
#define ICMP_PKT_FILTERED	13	/* Packet filtered */
#define ICMP_PREC_VIOLATION	14	/* Precedence violation */
#define ICMP_PREC_CUTOFF	15	/* Precedence cut off */
#define NR_ICMP_UNREACH		15	/* instead of hardcoding immediate value */

/* Codes for REDIRECT. */
#define ICMP_REDIR_NET		0	/* Redirect Net			*/
#define ICMP_REDIR_HOST		1	/* Redirect Host		*/
#define ICMP_REDIR_NETTOS	2	/* Redirect Net for TOS		*/
#define ICMP_REDIR_HOSTTOS	3	/* Redirect Host for TOS	*/

/* Codes for TIME_EXCEEDED. */
#define ICMP_EXC_TTL		0	/* TTL count exceeded		*/
#define ICMP_EXC_FRAGTIME	1	/* Fragment Reass time exceeded	*/

// This is missing in the W5100 device driver to make the IPRAW mode
// receive frame more explicit
struct IPRAW_HEADER {
  uint8_t src[4];
  uint16_t port;
};

// An address to ping
#define DEST 192,168,1,1

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaICMPping: started"));
  Watchdog::begin();
  RTC::begin();
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };
  ASSERT(ethernet.begin(ip, subnet));
}

void loop()
{
  Socket* sock = ethernet.socket(Socket::IPRAW, IPPROTO_ICMP);
  ASSERT(sock != NULL);

  INFO("Build ICMP echo request block", 0);
  icmphdr req;
  static uint16_t seq = 0;
  req.type = ICMP_ECHO;
  req.code = 0;
  req.checksum = 0;
  req.echo.id = 0xC05A;
  req.echo.seq = ++seq;
  req.timestamp = RTC::micros();
  req.checksum = hton(INET::checksum(&req, sizeof(req)));
  ASSERT(INET::checksum(&req, sizeof(req)) == 0);

  INFO("Send the request", 0);
  uint8_t dest[4] = { DEST };
  int res = sock->send(&req, sizeof(req), dest, 0);
  if (res < 0) goto error;

  INFO("Receive the reply", 0);
  uint32_t roundtrip;
  icmphdr reply;
  uint8_t src[4];
  uint16_t port;
  for (uint8_t retry = 0; retry < 10; retry++) {
    if ((res = sock->available()) > 0) break;
    delay(1);
  }
  if (res != sizeof(reply) + sizeof(IPRAW_HEADER)) goto error;
  res = sock->recv(&reply, sizeof(reply), src, port);
  roundtrip = RTC::micros();

  // Sanity check the reply; right size, checksum, id
  if (res != sizeof(reply)) goto error;
  if (reply.type != ICMP_ECHOREPLY) goto error;
  if (req.echo.id != 0xC05A) goto error;
  if (req.echo.seq != seq) goto error;
  if (INET::checksum(&reply, sizeof(reply)) != 0) goto error;

  // Calculate the time for the ping roundtrip
  roundtrip -= reply.timestamp;
  INFO("Roundtrip: %ul us", roundtrip);

 error:
  sock->close();
  sleep(3);
}
