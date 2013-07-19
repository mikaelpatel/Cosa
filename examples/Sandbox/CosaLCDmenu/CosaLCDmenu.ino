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
 * Baseline(Watchdog, LCD) 4006 bytes, +Menu::Walker 6640 bytes 
 * and +Demo menu code 6888 bytes.
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
// Menu item with binding to action function and state (object)
// 1. Create an action handler by sub-classing Menu::Action

class FileOpenAction : public Menu::Action {
public:
  virtual void run(Menu::item_P item) 
  {
    lcd.display_clear();
    lcd.puts_P(PSTR("opening file..."));
    SLEEP(2);
  }
};

// 2. Create an instance and bind to a menu item.
FileOpenAction do_open;
MENU_ACTION(open_action,"open",do_open)

// 3. Add to a menu item list.
MENU_BEGIN(file_menu,"file")
MENU_ITEM(open_action)
MENU_END(file_menu)

// Menu Enumeration Variable -------------------------------------------------
// Menu item with enumeration to change program state change
// 1. Define the symbols needed for the enumeration type
MENU_SYMB(on_symb,"On")
MENU_SYMB(off_symb,"Off")

// 2. Define the enumeration type
MENU_ENUM_BEGIN(onoff_enum_t)
  MENU_ENUM_ITEM(off_symb)
  MENU_ENUM_ITEM(on_symb)
MENU_ENUM_END(onoff_enum_t);

// 3. Create a menu item with reference to program state to control
uint16_t tracing = 1;
MENU_ENUM(onoff_enum_t,tracing_enum,"tracing",tracing)

MENU_SYMB(idle_symb,"Idle")
MENU_SYMB(running_symb,"Running")
MENU_SYMB(waiting_symb,"Waiting")

MENU_ENUM_BEGIN(state_enum_t)
  MENU_ENUM_ITEM(idle_symb)
  MENU_ENUM_ITEM(running_symb)
  MENU_ENUM_ITEM(waiting_symb)
MENU_ENUM_END(state_enum_t);

uint16_t state = 0;
MENU_ENUM(state_enum_t,state_enum,"state",state)

// Menu Integer Range Variable -----------------------------------------------
// Menu item with integer range(low..high) to change program state change
// 1. Create menu item with integer range and reference to variable
int16_t count = 42;
MENU_RANGE(count_range,"count",-100,100,count)

// 2. Add menu item to menu item list
MENU_BEGIN(options_menu,"options")
  MENU_ITEM(tracing_enum)
  MENU_ITEM(state_enum)
  MENU_ITEM(count_range)
MENU_END(options_menu)

MENU_BEGIN(edit_menu,"edit")
  MENU_ITEM(options_menu)
MENU_END(edit_menu)

// Sub-Menu ------------------------------------------------------------------
// Menu item list may contain other menu item lists (sub-menues)
MENU_BEGIN(root_menu,"demo")
  MENU_ITEM(file_menu)
  MENU_ITEM(edit_menu)
MENU_END(root_menu)

// The menu handler ----------------------------------------------------------
// The walker will recieve key events from the keypad
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
