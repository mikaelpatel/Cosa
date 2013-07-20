/**
 * @file CosaLCDmenu.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * @section Footprint
 * Baseline(Watchdog, LCD) 4006 bytes, +Menu::Walker 6732 bytes (+2726)
 * and +Demo menu code 7046 bytes (+314).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/LCD/Driver/HD44780.hh"
#include "Cosa/Menu.hh"

// Use the HD44780 LCD driver with 4-bit parallel port and keypad shield
HD44780::Port port;
HD44780 lcd(&port);

// Menu Action ---------------------------------------------------------------
// Menu item with binding to an action function and state (object)
// 1. Create an action handler by sub-classing Menu::Action and implement
// the virtual member function run().

class FileOpenAction : public Menu::Action {
public:
  virtual void run(Menu::item_P item) 
  {
    lcd.display_clear();
    lcd.puts_P(PSTR("Opening File..."));
    SLEEP(2);
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
MENU_ENUM(onoff_enum_t,tracing_enum,"Tracing",tracing)

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
MENU_ENUM(state_enum_t,state_enum,"State",state)

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
MENU_BITSET(debug_enum_t,debug_enum,"Debug",debug)

// Menu Integer Range Variable -----------------------------------------------
// Menu item with integer range(low..high) to change program state change
// 1. Create menu item with integer range and reference to variable
int16_t limit = 42;
MENU_RANGE(limit_range,"Limit",-10,100,limit)

// Sub-Menu ------------------------------------------------------------------
// Menu item list may contain other menu item lists (sub-menues)

MENU_BEGIN(options_menu,"Options")
  MENU_ITEM(tracing_enum)
  MENU_ITEM(state_enum)
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
// The watchdog issues timeout events which periodically activate the
// keypad handler. It polls the keys and issues key events to the
// menu walker.
Menu::Walker walker(&lcd, &root_menu);

void setup()
{
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
  lcd.begin();
  lcd.puts_P(PSTR("CosaLCDmenu: started"));
  SLEEP(2);
  walker.begin();
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
