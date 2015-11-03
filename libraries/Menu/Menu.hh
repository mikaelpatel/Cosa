/**
 * @file Menu.hh
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_MENU_HH
#define COSA_MENU_HH

#include "Cosa/LCD.hh"
#include "Cosa/IOStream.hh"

#include <Rotary.h>
#include "Cosa/Button.hh"
#include "Cosa/Keypad.hh"

/**
 * LCD Menu abstraction. Allows definition of menus with sub-menus,
 * items, enumerations, bitsets, range values and actions.
 */
class Menu {
public:
  /** Menu type tag code. */
  enum type_t {
    ITEM,			//!< Menu item/symbol.
    ITEM_LIST,			//!< Menu item/enum list.
    ONE_OF,			//!< Menu enumeration variable (one-of).
    ZERO_OR_MANY,		//!< Menu bitset variable (zero-or-many).
    BCD_RANGE,			//!< Menu bcd(2) range variable.
    INT_RANGE,			//!< Menu integer range variable.
    ACTION			//!< Menu action.
  } __attribute__((packed));

  /** Menu item header. Also used for enumeration symbols. */
  struct item_t {
    type_t type;		//!< Item type tag(ITEM).
    str_P name;			//!< Item string in program memory.
  };
  typedef const PROGMEM item_t* item_P;
  typedef const PROGMEM item_P* item_vec_P;

  /** Menu item lists. */
  struct item_list_t {
    item_t item;		//!< Item header(ITEM_LIST).
    item_vec_P list;		//!< Item list in program memory.
  };
  typedef const PROGMEM item_list_t* item_list_P;

  /** Enumeration variable symbols list (one-of). */
  struct one_of_t {
    item_t item;		//!< Item header(ONE_OF).
    item_vec_P list;		//!< Item list in program memory.
    uint16_t* value;		//!< Pointer to value.
  };
  typedef const PROGMEM one_of_t* one_of_P;
  static void print(IOStream& outs, one_of_P var);

  /** Zero-or-many variable symbols list, Item header(ZERO_OR_MANY). */
  typedef one_of_t zero_or_many_t;
  typedef const PROGMEM zero_or_many_t* zero_or_many_P;
  static void print(IOStream& outs, zero_or_many_P var, bool selected, uint8_t bv);

  /** Bcd(2) range variable. */
  struct bcd_range_t {
    item_t item;		//!< Item header(BCD_RANGE).
    uint8_t low;		//!< Range low value.
    uint8_t high;		//!< Range high value.
    uint8_t* value;		//!< Pointer to value.
  };
  typedef const PROGMEM bcd_range_t* bcd_range_P;

  /** Integer range variable. */
  struct int_range_t {
    item_t item;		//!< Item header(INT_RANGE).
    int16_t low;		//!< Range low value.
    int16_t high;		//!< Range high value.
    int16_t* value;		//!< Pointer to value.
  };
  typedef const PROGMEM int_range_t* int_range_P;
  static void print(IOStream& outs, int_range_P var, bool selected);

  /**
   * Menu Action handler. Must be sub-classed and the virtual member
   * function run() must be implemented. Holds the state for the
   * menu action.
   */
  class Action {
  public:
    /**
     * @override{Menu::Action}
     * Menu action function for given menu item. Return true(1) if the
     * menu walker should render the display otherwise if the false(0).
     * @param[in] item menu item reference.
     * @return bool
     */
    virtual bool run(Menu::item_P item) = 0;
  };

  /** Menu action item. */
  struct action_t {
    item_t item;		//!< Item header(ACTION).
    Action* obj;		//!< Pointer to action handler.
  };
  typedef const PROGMEM action_t* action_P;

  /**
   * The Menu Walker reacts to key events from the key pad. It maintains
   * a stack with the path to the current position in the menu three.
   */
  class Walker {
  private:
    /** Stack for menu walker path. */
    static const uint8_t STACK_MAX = 8;
    Menu::item_list_P m_stack[STACK_MAX];
    uint8_t m_top;

    /** Current menu list item index. */
    uint8_t m_ix;

    /** Current menu bitset index. */
    uint8_t m_bv;

    /** Item selection state. */
    bool m_selected;

    /** Output stream for menu printout. */
    IOStream m_out;

    void on_key_down(uint8_t nr, zero_or_many_P var);

  public:
    /** Menu walker key index (same as LCDkeypad map for simplicity). */
    enum {
      NO_KEY = 0,
      SELECT_KEY,
      LEFT_KEY,
      DOWN_KEY,
      UP_KEY,
      RIGHT_KEY
    } __attribute__((packed));

    /**
     * Construct a menu walker for the given menu.
     * @param[in] lcd device.
     * @param[in] root menu item list.
     */
    Walker(LCD::Device* lcd, item_list_P root) :
      m_top(0),
      m_ix(0),
      m_bv(0),
      m_selected(false),
      m_out(lcd)
    {
      m_stack[m_top] = root;
    }

    /**
     * The menu walker key interpretor. Should be called by a menu
     * controller, i.e. an adapter of controller events to the menu
     * walker key.
     * @param[in] nr key number (index in map).
     */
    void on_key_down(uint8_t nr);

    /**
     * Print menu walker state, current menu position to given
     * output stream.
     * @param[in] outs output stream.
     * @param[in] walker menu walker.
     * @return output stream
     */
    friend IOStream& operator<<(IOStream& outs, Walker& walker);

    /**
     * Initiate the menu. If the given flag is true the menu state
     * is printed.
     */
    void begin(bool flag = true)
    {
      if (flag) m_out << clear << *this;
    }

    /**
     * Get current menu item type.
     */
    Menu::type_t type();
  };

  /**
   * Menu walker controller for the LCD keypad. Adapt the keypad
   * key down events to the Menu walker. For simplicity the key
   * map for the walker and the LCD keypad are the same.
   */
  class KeypadController : public LCD::Keypad {
  public:
    Walker* m_walker;

  public:
    /**
     * Construct keypad event adapter for menu walker.
     * @param[in] walker to control.
     */
    KeypadController(Walker* walker, Job::Scheduler* scheduler) :
      LCD::Keypad(scheduler),
      m_walker(walker)
    {}

    /**
     * @override{Keypad}
     * The menu walker key interpretor.
     * @param[in] nr key number (index in map).
     */
    virtual void on_key_down(uint8_t nr)
    {
      m_walker->on_key_down(nr);
    }
  };

  /**
   * Menu walker controller for the Rotary encoder with push button.
   * Rotary encoder CW is mapped to DOWN_KEY, CCW to UP_KEY. The
   * push button is mapped to SELECT_KEY.
   */
  class RotaryController : public Rotary::Encoder {
  private:
    class RotaryButton : public Button {
    private:
      Menu::Walker* m_walker;

    public:
      /**
       * Create rotary encoder push button handler for given pin.
       * @param[in] walker to control.
       * @param[in] scheduler for button sampling.
       * @param[in] pin rotary encoder push button.
       */
      RotaryButton(Menu::Walker* walker,
		   Job::Scheduler* scheduler,
		   Board::DigitalPin pin = Board::D2) :
	Button(scheduler, pin, Button::ON_FALLING_MODE),
	m_walker(walker)
      {}

      /**
       * @override{Button}
       * The rotary encoder button generates a SELECT_KEY.
       * @param[in] type of change.
       */
      virtual void on_change(uint8_t type)
      {
	UNUSED(type);
	m_walker->on_key_down(Menu::Walker::SELECT_KEY);
      }
    };

    /** The menu walker to control. */
    Menu::Walker* m_walker;

    /** The rotary encoder push button handler. */
    RotaryButton m_sw;

  public:
    /**
     * Construct rotary encoder event adapter for menu walker.
     * @param[in] walker to control.
     * @param[in] scheduler for button sampling.
     * @param[in] clk rotary encoder clock pin (Default PCI4).
     * @param[in] dt rotary encoder data pin (Default PCI3).
     * @param[in] sw rotary encoder switch pin (Default D2).
     */
    RotaryController(Menu::Walker* walker,
		     Job::Scheduler* scheduler,
		     Board::InterruptPin clk = Board::PCI4,
		     Board::InterruptPin dt = Board::PCI3,
		     Board::DigitalPin sw = Board::D2) :
      Rotary::Encoder(clk, dt),
      m_walker(walker),
      m_sw(walker, scheduler, sw)
    {}

    /**
     * @override{Event::Handler}
     * Rotary change event handler. Forward change as a key; CW is mapped
     * to DOWN_KEY and CCW to UP_KEY (and reverse for RANGE).
     * @param[in] type the event type.
     * @param[in] direction the event value.
     */
    virtual void on_event(uint8_t type, uint16_t direction);

    /**
     * Start the rotary encoder change detector.
     */
    void begin()
    {
      PinChangeInterrupt::begin();
      m_sw.start();
    }
  };
};

/**
 * Support macro to start the definition of a menu in program memory.
 * Used in the form:
 *   MENU_BEGIN(var,name)
 *     MENU_ITEM(item-1)
 *     ...
 *     MENU_ITEM(item-n)
 *   MENU_END(var)
 * @param[in] var menu to create.
 * @param[in] name string for menu.
 */
#define MENU_BEGIN(var,name)				\
  const char var ## _name[] __PROGMEM = name;		\
  const Menu::item_P var ## _list[] __PROGMEM = {

/**
 * Support macro to add a menu item in program memory.
 * The item can be any of the menu item types; ITEM, ITEM_LIST,
 * ENUM, RANGE and ACTION.
 * @param[in] var item reference to add.
 */
#define MENU_ITEM(var)					\
  &var.item,

/**
 * Support macro to complete a menu in program memory.
 * @param[in] var menu to create.
 */
#define MENU_END(var)					\
    0							\
  };							\
  const Menu::item_list_t var __PROGMEM = {		\
  {							\
    Menu::ITEM_LIST,					\
    (str_P) var ## _name				\
  },							\
  var ## _list						\
};

/**
 * Support macro to define a menu symbol in program memory.
 * @param[in] var menu symbol to create.
 * @param[in] name string for menu symbol.
 */
#define MENU_SYMB(var,name)				\
  const char var ## _name[] __PROGMEM = name;		\
  const Menu::item_t var __PROGMEM = {			\
    Menu::ITEM,						\
    (str_P) var ## _name				\
  };

/**
 * Support macro to start the definition of an enumeration type
 * in program memory.
 * Used in the form:
 *   MENU_SYMB(symb-1)
 *   ...
 *   MENU_SYMB(symb-n)
 *   MENU_ENUM_BEGIN(var,name)
 *     MENU_ENUM_ITEM(symb-1)
 *     ...
 *     MENU_ENUM_ITEM(symb-n)
 *   MENU_ENUM_END(var)
 * @param[in] var menu enumeration variable to create.
 */
#define MENU_ENUM_BEGIN(var)				\
  const Menu::item_P var ## _list[] __PROGMEM = {

/**
 * Support macro to add an menu item/symbol to an enumeration type
 * in program memory.
 * @param[in] var menu item/symbol.
 */
#define MENU_ENUM_ITEM(var)				\
  &var,

/**
 * Support macro to complete an enumeration type in program memory.
 * @param[in] var menu to create.
 */
#define MENU_ENUM_END(var)				\
  0							\
  };

/**
 * Support macro to define a menu one-of variable.
 * @param[in] type enumeration list.
 * @param[in] var menu one-of variable to create.
 * @param[in] name string for menu one-of variable.
 * @param[in] value variable with runtime value.
 */
#define MENU_ONE_OF(type,var,name,value)		\
  const char var ## _name[] __PROGMEM = name;		\
  const Menu::one_of_t var __PROGMEM = {		\
  {							\
    Menu::ONE_OF,					\
    (str_P) var ## _name				\
  },							\
  type ## _list,					\
  &value						\
};

/**
 * Support macro to define a menu zero-or-many variable.
 * @param[in] type enumeration list.
 * @param[in] var menu zero-or-many variable to create.
 * @param[in] name string for menu zero-or-many variable.
 * @param[in] value variable with runtime value.
 */
#define MENU_ZERO_OR_MANY(type,var,name,value)		\
  const char var ## _name[] __PROGMEM = name;		\
  const Menu::zero_or_many_t var __PROGMEM = {		\
  {							\
    Menu::ZERO_OR_MANY,					\
    (str_P) var ## _name				\
  },							\
  type ## _list,					\
  &value						\
};

/**
 * Support macro to define a menu integer range in program memory.
 * @param[in] var menu range item to create.
 * @param[in] name string of menu item.
 * @param[in] low range value.
 * @param[in] high range value.
 * @param[in] value variable name.
 */
#define MENU_INT_RANGE(var,name,low,high,value)		\
  const char var ## _name[] __PROGMEM = name;		\
  const Menu::int_range_t var __PROGMEM = {		\
  {							\
    Menu::INT_RANGE,					\
    (str_P) var ## _name				\
  },							\
  low,							\
  high,							\
  &value						\
};

/**
 * Support macro to define a menu action in program memory.
 * @param[in] var menu action item to create.
 * @param[in] name string of menu item.
 * @param[in] obj pointer to menu action handler.
 */
#define MENU_ACTION(var,name,obj)			\
  const char var ## _name[] __PROGMEM = name;		\
  const Menu::action_t var __PROGMEM = {		\
  {							\
    Menu::ACTION,					\
    (str_P) var ## _name				\
  },							\
  &obj							\
  };
#endif
