/**
 * @file CosaLCDverify.ino
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
 * Verify build of all implementations of LCD and adapters.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <HD44780.h>

#include <PCF8574.h>
#include <MJKDZ_LCD_Module.h>
#include <GY_IICLCD.h>
#include <DFRobot_IIC_LCD_Module.h>
#include <SainSmart_LCD2004.h>

#include <MCP23008.h>
#include <Adafruit_I2C_LCD_Backpack.h>
#include <ERM1602_5.h>

#include <Canvas.h>
#include <PCD8544.h>
#include <ST7565.h>

#include <VLCD.h>

