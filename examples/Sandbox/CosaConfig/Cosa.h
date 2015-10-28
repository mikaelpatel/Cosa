/**
 * @file Cosa.h
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
 * Default for per-application customization of Cosa. This file is
 * intentionally empty as it is the default customization if the
 * application does not provide one.
 *
 * @section Configuration
 * To customize Cosa, copy this file into the application directory
 * and enable define below.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_H
#define COSA_H

/**
 * Software disable low voltage detect. Powersaving is 23 uA at 5 V.
 * In file: main.cpp
 */
#define COSA_BOD_DISABLE

/**
 * Event queue size. Default is 16 entries (8 ATTINY).
 * In file: Cosa/Event.hh
 */
#define COSA_EVENT_QUEUE_MAX 8

/**
 * UART buffer size. Default is 32 characters (16 ATTINY).
 * In file: Cosa/UART.hh
 */
#define COSA_UART_BUFFER_MAX 64

/**
 * Soft::UART buffer size. Default is 32 characters (16 ATTINY).
 * In file: Cosa/Soft/UART.hh
 */
#define COSA_SOFT_UART_BUFFER_MAX 8

#endif
