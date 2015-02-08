/**
 * @file Cosa/USB/Desc.h
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

#ifndef COSA_USB_DESC_H
#define COSA_USB_DESC_H

#include "Cosa/Types.h"

#define CDC_ENABLED

// Fix: Add support for keyboard and mouse
// #define HID_ENABLED

#ifdef CDC_ENABLED
#define CDC_INTERFACE_COUNT	2
#define CDC_ENPOINT_COUNT	3
#else
#define CDC_INTERFACE_COUNT	0
#define CDC_ENPOINT_COUNT	0
#endif

#ifdef HID_ENABLED
#define HID_INTERFACE_COUNT	1
#define HID_ENPOINT_COUNT	1
#else
#define HID_INTERFACE_COUNT	0
#define HID_ENPOINT_COUNT	0
#endif

#define CDC_ACM_INTERFACE	0
#define CDC_DATA_INTERFACE	1
#define CDC_FIRST_ENDPOINT	1
#define CDC_ENDPOINT_ACM	(CDC_FIRST_ENDPOINT)
#define CDC_ENDPOINT_OUT	(CDC_FIRST_ENDPOINT+1)
#define CDC_ENDPOINT_IN		(CDC_FIRST_ENDPOINT+2)

#define HID_INTERFACE		(CDC_ACM_INTERFACE + CDC_INTERFACE_COUNT)
#define HID_FIRST_ENDPOINT	(CDC_FIRST_ENDPOINT + CDC_ENPOINT_COUNT)
#define HID_ENDPOINT_INT	(HID_FIRST_ENDPOINT)

#define INTERFACE_COUNT		(MSC_INTERFACE + MSC_INTERFACE_COUNT)

#ifdef CDC_ENABLED
#define CDC_RX CDC_ENDPOINT_OUT
#define CDC_TX CDC_ENDPOINT_IN
#endif

#ifdef HID_ENABLED
#define HID_TX HID_ENDPOINT_INT
#endif

#define IMANUFACTURER		1
#define IPRODUCT		2

#endif
