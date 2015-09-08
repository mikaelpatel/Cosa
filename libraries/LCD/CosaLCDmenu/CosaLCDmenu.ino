/**
 * @file CosaLCDmenu.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Cosa LCD menu system demo.
 *
 * @section Circuit
 * See HD44780.hh for description of LCD adapter circuits. Designed
 * for LCD Keypad shield with 16x2 HD44780 LCD and six buttons, or LCD
 * and a rotary encoder.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Rotary.h>
#include <Menu.h>

#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"

#define USE_KEYPAD
// #define USE_ROTARY_ENCODER

// Select port type to use with the LCD device driver.
// LCD and communication port
#include <HD44780.h>

// HD44780 driver built-in adapters
HD44780::Port4b port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4W port;

// I2C expander io port based adapters
// #include <PCF8574.h>
// #include <MJKDZ_LCD_Module.h>
// MJKDZ_LCD_Module port;
// MJKDZ_LCD_Module port(0);
// #include <GY_IICLCD.h>
// GY_IICLCD port;
// #include <DFRobot_IIC_LCD_Module.h>
// DFRobot_IIC_LCD_Module port;
// #include <SainSmart_LCD2004.h>
// SainSmart_LCD2004 port;
// #include <MCP23008.h>
// #include <Adafruit_I2C_LCD_Backpack.h>
// Adafruit_I2C_LCD_Backpack port;

// HD44780 based LCD with support for serial communication
// #include <ERM1602_5.h>
// ERM1602_5 port;

// HD44780 variants; 16X1, 16X2, 16X4, 20X4, default 16X2
// HD44780 lcd(&port, 20, 4);
// HD44780 lcd(&port, 16, 4);
HD44780 lcd(&port);

// Menu Action ---------------------------------------------------------------
// Menu item with binding to an action function and state (object)
// 1. Create an action handler by sub-classing Menu::Action and implement
// the virtual member function run().
class FileOpenAction : public Menu::Action {
public:
  virtual bool run(Menu::item_P item)
  {
    UNUSED(item);
    lcd.display_clear();
    lcd.puts(PSTR("Opening File..."));
    sleep(2);
    return (true);
  }
};

// 2. Create an instance and bind to a menu item.
FileOpenAction do_open;
MENU_ACTION(open_action,"Open",do_open)

// 3. Add to a menu item list.
MENU_BEGIN(file_menu,"File")
MENU_ITEM(open_action)
MENU_END(file_menu)

// Menu Enumeration Variable -------------------------------------------------
// Menu item with enumeration to change program state
// 1. Define the symbols needed for the enumeration type
MENU_SYMB(on_symb,"On")
MENU_SYMB(off_symb,"Off")

// 2. Define the enumeration type. Symbol order is the value set (0..1).
MENU_ENUM_BEGIN(onoff_enum_t)
  MENU_ENUM_ITEM(off_symb)
  MENU_ENUM_ITEM(on_symb)
MENU_ENUM_END(onoff_enum_t);

// 3. Create a menu item with reference to the program state to control
uint16_t tracing = 1;
MENU_ONE_OF(onoff_enum_t,tracing_one_of,"Tracing",tracing)

// Menu Enumeration Variable -------------------------------------------------
// Menu item with enumeration to change program state
// 1. Define the symbols needed for the enumeration type
MENU_SYMB(idle_symb,"Idle")
MENU_SYMB(running_symb,"Running")
MENU_SYMB(waiting_symb,"Waiting")

// 2. Define the enumeration type. Symbol order is the value set (0..2).
MENU_ENUM_BEGIN(state_enum_t)
  MENU_ENUM_ITEM(idle_symb)
  MENU_ENUM_ITEM(running_symb)
  MENU_ENUM_ITEM(waiting_symb)
MENU_ENUM_END(state_enum_t);

// 3. Create a menu item with reference to the program state to control
uint16_t state = 0;
MENU_ONE_OF(state_enum_t,state_one_of,"State",state)

// Menu Bitset Variable ------------------------------------------------------
// Menu item with bitset to change program state
// 1. Define the symbols needed for the enumeration type
MENU_SYMB(break_symb,"Break")
MENU_SYMB(mockup_symb,"Mockup")
MENU_SYMB(profile_symb,"Profile")
MENU_SYMB(trace_symb,"Trace")

// 2. Define the enumeration type. Symbol order is the value set (0.3).
MENU_ENUM_BEGIN(debug_enum_t)
  MENU_ENUM_ITEM(break_symb)
  MENU_ENUM_ITEM(mockup_symb)
  MENU_ENUM_ITEM(profile_symb)
  MENU_ENUM_ITEM(trace_symb)
MENU_ENUM_END(debug_enum_t);

// 3. Create a menu item with reference to program state to control
// The enumeration type values are bit positions. The initial value is
// Break | Mockup.
uint16_t debug = 3;
MENU_ZERO_OR_MANY(debug_enum_t,debug_enum,"Debug",debug)

// Menu Integer Range Variable -----------------------------------------------
// Menu item with integer range(low..high) to change program state change
// 1. Create menu item with integer range and reference to variable
int16_t limit = 42;
MENU_INT_RANGE(limit_range,"Limit",-10,100,limit)

// Sub-Menu ------------------------------------------------------------------
// Menu item list may contain other menu item lists (sub-menues)

MENU_BEGIN(options_menu,"Options")
  MENU_ITEM(tracing_one_of)
  MENU_ITEM(state_one_of)
  MENU_ITEM(limit_range)
MENU_END(options_menu)

MENU_BEGIN(edit_menu,"Edit")
  MENU_ITEM(options_menu)
  MENU_ITEM(debug_enum)
MENU_END(edit_menu)

MENU_BEGIN(root_menu,"Demo")
  MENU_ITEM(file_menu)
  MENU_ITEM(edit_menu)
MENU_END(root_menu)

// The menu handler ----------------------------------------------------------
// Control the menu walker with keypad (analog pin) or rotary encoder with
// push button.
Watchdog::Scheduler scheduler;
Menu::Walker walker(&lcd, &root_menu);

#if defined(USE_KEYPAD)
Menu::KeypadController controller(&walker, &scheduler);
#endif

#if defined(USE_ROTARY_ENCODER)
Menu::RotaryController controller(&walker, &scheduler,
				  Board::PCI6, Board::PCI5,
				  Board::D3);
#endif

void setup()
{
  Watchdog::begin();
  Watchdog::job(&scheduler);
  lcd.begin();
  lcd.puts(PSTR("CosaLCDmenu: started"));
  sleep(2);
  walker.begin();
  controller.start();
}

void loop()
{
  Event::service();
}
