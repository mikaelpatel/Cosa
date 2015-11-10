/**
 * @file Cosa/CDC.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (c) 2011, Peter Barrett (original author)
 * Copyright (C) 2013-2015, Mikael Patel (refactoring and extension)
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

#include "Cosa/CDC.hh"

#if defined(USBCON)

#include "Cosa/USB/Platform.h"
#include "Cosa/IOBuffer.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Power.hh"
#include <avr/wdt.h>

static IOBuffer<CDC::BUFFER_MAX> ibuf;
CDC __attribute__((weak)) cdc(&ibuf);

typedef struct {
  uint32_t dwDTERate;
  uint8_t bCharFormat;
  uint8_t bParityType;
  uint8_t bDataBits;
  uint8_t lineState;
} LineInfo;

static volatile LineInfo _usbLineInfo = {
  57600, 0x00, 0x00, 0x00, 0x00
};

int CDC_GetInterface(uint8_t* interfaceNum) __attribute__ ((weak));
bool CDC_Setup(Setup& setup) __attribute__ ((weak));

extern const CDCDescriptor _cdcInterface PROGMEM;
const CDCDescriptor _cdcInterface = {
  D_IAD(0,2,CDC_COMMUNICATION_INTERFACE_CLASS,CDC_ABSTRACT_CONTROL_MODEL,1),
  // CDC communication interface
  D_INTERFACE(CDC_ACM_INTERFACE,1,CDC_COMMUNICATION_INTERFACE_CLASS,CDC_ABSTRACT_CONTROL_MODEL,0),
  // Header (1.10 bcd)
  D_CDCCS(CDC_HEADER,0x10,0x01),
  // Device handles call management (not)
  D_CDCCS(CDC_CALL_MANAGEMENT,1,1),
  // SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported
  D_CDCCS4(CDC_ABSTRACT_CONTROL_MANAGEMENT,6),
  // Communication interface is master, data interface is slave 0
  D_CDCCS(CDC_UNION,CDC_ACM_INTERFACE,CDC_DATA_INTERFACE),
  D_ENDPOINT(USB_ENDPOINT_IN (CDC_ENDPOINT_ACM),USB_ENDPOINT_TYPE_INTERRUPT,0x10,0x40),
  // CDC data interface
  D_INTERFACE(CDC_DATA_INTERFACE,2,CDC_DATA_INTERFACE_CLASS,0,0),
  D_ENDPOINT(USB_ENDPOINT_OUT(CDC_ENDPOINT_OUT),USB_ENDPOINT_TYPE_BULK,0x40,0),
  D_ENDPOINT(USB_ENDPOINT_IN (CDC_ENDPOINT_IN ),USB_ENDPOINT_TYPE_BULK,0x40,0)
};

int
CDC_GetInterface(uint8_t* interfaceNum)
{
  interfaceNum[0] += 2;
  return USB_SendControl(TRANSFER_PGM,&_cdcInterface,sizeof(_cdcInterface));
}

bool
CDC_Setup(Setup& setup)
{
  uint8_t r = setup.bRequest;
  uint8_t requestType = setup.bmRequestType;

  if (REQUEST_DEVICETOHOST_CLASS_INTERFACE == requestType) {
    if (CDC_GET_LINE_CODING == r) {
      USB_SendControl(0,(void*)&_usbLineInfo,7);
      return (true);
    }
  }

  if (REQUEST_HOSTTODEVICE_CLASS_INTERFACE == requestType) {
    if (CDC_SET_LINE_CODING == r) {
      USB_RecvControl((void*)&_usbLineInfo,7);
      return (true);
    }

    if (CDC_SET_CONTROL_LINE_STATE == r) {
      _usbLineInfo.lineState = setup.wValueL;

      // auto-reset into the bootloader is triggered when the port, already
      // open at 1200 bps, is closed.  this is the signal to start the watchdog
      // with a relatively long period so it can finish housekeeping tasks
      // like servicing endpoints before the sketch ends
      if (1200 == _usbLineInfo.dwDTERate) {
	// We check DTR state to determine if host port is open (bit 0
	// of lineState).
	if ((_usbLineInfo.lineState & 0x01) == 0) {
	  *(uint16_t*) 0x0800 = 0x7777;
	  wdt_enable(WDTO_120MS);
	}
	else {
	  // Most OSs do some intermediate steps when configuring
	  // ports and DTR can twiggle more than once before
	  // stabilizing. To avoid spurious resets we set the watchdog
	  // to 120ms and eventually cancel if DTR goes back high.
	  wdt_disable();
	  wdt_reset();
	  *(uint16_t *)0x0800 = 0x0;
	}
      }
      return (true);
    }

    if (CDC_SEND_BREAK == r) {
      // lost serial connection; mark lineState as gone
      _usbLineInfo.lineState = 0;
      return (true);
    }
  }
  return (false);
}

bool
CDC::begin(uint32_t baudrate, uint8_t format)
{
  UNUSED(baudrate);
  UNUSED(format);
  Power::usb_enable();
  if (!Watchdog::is_initiated()) Watchdog::begin();
  if (!USBDevice.attach()) return (false);
  for (uint8_t retry = 0; retry < 30; retry++) {
    if (_usbLineInfo.lineState > 0) return (true);
    delay(200);
  }
  delay(1000);
  return (false);
}

void
CDC::accept()
{
  while (m_ibuf->room()) {
    int c = USB_Recv(CDC_RX);
    if (c == IOStream::EOF) break;
    m_ibuf->putchar(c);
  }
}

int
CDC::flush(void)
{
  USB_Flush(CDC_TX);
  return (0);
}

void
CDC::empty(void)
{
  m_ibuf->empty();
}

int
CDC::write(const void* buf, size_t size)
{
  if ((_usbLineInfo.lineState & 0x01) &&
      (USB_Send(CDC_TX, buf, size) != (int) size))
    return (IOStream::EOF);

  return (size);
}

#endif
