/**
 * @file Cosa/USB/USBAPI.h
 * @version 1.0
 *
 * @section License
 * Copyright (c) 2010, Peter Barrett (original author)
 * Copyright (C) 2013-2015, Mikael Patel (refactoring and extensions)
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

#ifndef COSA_USB_API_H
#define COSA_USB_API_H

#include "Cosa/Types.h"

#if defined(USBCON)

class USBDevice_
{
 public:
  USBDevice_();
  bool configured();
  bool attach();
  void detach();
  void poll();
};
extern USBDevice_ USBDevice;

typedef struct {
  uint8_t bmRequestType;
  uint8_t bRequest;
  uint8_t wValueL;
  uint8_t wValueH;
  uint16_t wIndex;
  uint16_t wLength;
} Setup;

int HID_GetInterface(uint8_t* interfaceNum);
int HID_GetDescriptor(int i);
bool HID_Setup(Setup& setup);
void HID_SendReport(uint8_t id, const void* data, int len);

int MSC_GetInterface(uint8_t* interfaceNum);
int MSC_GetDescriptor(int i);
bool MSC_Setup(Setup& setup);
bool MSC_Data(uint8_t rx,uint8_t tx);

int CDC_GetInterface(uint8_t* interfaceNum);
int CDC_GetDescriptor(int i);
bool CDC_Setup(Setup& setup);

#define TRANSFER_PGM		0x80
#define TRANSFER_RELEASE	0x40
#define TRANSFER_ZERO		0x20

int USB_SendControl(uint8_t flags, const void* d, int len);
int USB_RecvControl(void* d, int len);

uint8_t	USB_Available(uint8_t ep);
int USB_Send(uint8_t ep, const void* data, int len);
int USB_Recv(uint8_t ep, void* data, int len);
int USB_Recv(uint8_t ep);
void USB_Flush(uint8_t ep);

#endif
#endif
