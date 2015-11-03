/**
 * @file Menu.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#include "Menu.hh"

void
Menu::print(IOStream& outs, Menu::one_of_P var)
{
  uint16_t ix = *((uint16_t*) pgm_read_word(&var->value));
  item_vec_P list = (item_vec_P) pgm_read_word(&var->list);
  item_P item = (item_P) pgm_read_word(&list[ix]);
  outs << (str_P) pgm_read_word(&item->name);
}

void
Menu::print(IOStream& outs, zero_or_many_P var, bool selected, uint8_t bv)
{
  if (UNLIKELY(!selected)) return;
  uint16_t value = *((uint16_t*) pgm_read_word(&var->value));
  item_vec_P list = (item_vec_P) pgm_read_word(&var->list);
  item_P item = (item_P) pgm_read_word(&list[bv]);
  if (value & _BV(bv))
    outs << PSTR("[x] ");
  else outs << PSTR("[ ] ");
  outs << (str_P) pgm_read_word(&item->name);
}

void
Menu::print(IOStream& outs, int_range_P var, bool selected)
{
  int16_t* vp = (int16_t*) pgm_read_word(&var->value);
  int16_t value = *vp;
  outs << value;
  if (UNLIKELY(!selected)) return;
  outs << PSTR(" [")
       << (int16_t) pgm_read_word(&var->low)
       << PSTR("..")
       << (int16_t) pgm_read_word(&var->high)
       << PSTR("]");
}

IOStream&
operator<<(IOStream& outs, Menu::Walker& walker)
{
  // Access the current state of the menu walker
  Menu::item_list_P menu = walker.m_stack[walker.m_top];
  Menu::item_vec_P list = (Menu::item_vec_P) pgm_read_word(&menu->list);
  Menu::item_P item = &menu->item;
  Menu::type_t type;

  // Print asterics to mark selection
  if (walker.m_selected) outs << '*';

  // Print the name of the current menu item with parent
  outs << (str_P) pgm_read_word(&item->name) << ':';
  item = (Menu::item_P) pgm_read_word(&list[walker.m_ix]);
  outs << (str_P) pgm_read_word(&item->name) << endl;
  type = (Menu::type_t) pgm_read_byte(&item->type);

  // Print possible value of current menu item
  switch (type) {
  case Menu::ONE_OF:
    // Print the one-of variable value string
    Menu::print(outs, (Menu::one_of_P) item);
    break;
  case Menu::ZERO_OR_MANY:
    // Print the zero-or-many variable when selected
    Menu::print(outs, (Menu::zero_or_many_P) item,
		walker.m_selected, walker.m_bv);
    break;
  case Menu::INT_RANGE:
    // Print the range variable and limits when selected
    Menu::print(outs, (Menu::int_range_P) item, walker.m_selected);
    break;
  default:
    ;
  }
  return (outs);
}

void
Menu::Walker::on_key_down(uint8_t nr)
{
  // Access the current menu item
  Menu::item_list_P menu = m_stack[m_top];
  Menu::item_vec_P list = (Menu::item_vec_P) pgm_read_word(&menu->list);
  Menu::item_P item = (Menu::item_P) pgm_read_word(&list[m_ix]);
  Menu::type_t type = (Menu::type_t) pgm_read_byte(&item->type);

  // React to key event
  switch (nr) {
  case NO_KEY:
    break;
  case SELECT_KEY:
  case RIGHT_KEY:
    switch (type) {
    case Menu::ZERO_OR_MANY:
      // Select zero-or-many variable or toggle current item
      {
	if (!m_selected) {
	  m_selected = true;
	  m_bv = 0;
	  break;
	}
	Menu::zero_or_many_P var = (Menu::zero_or_many_P) item;
	uint16_t* vp = (uint16_t*) pgm_read_word(&var->value);
	list = (Menu::item_vec_P) pgm_read_word(&var->list);
	item = (Menu::item_P) pgm_read_word(&list[m_bv]);
	uint16_t value = *vp;
	if ((value & _BV(m_bv)) == 0)
	  *vp = (value | _BV(m_bv));
	else
	  *vp = (value & ~_BV(m_bv));
      }
      break;
    case Menu::ITEM_LIST:
      // Walk into sub-menu
      {
	m_stack[++m_top] = (Menu::item_list_P) item;
	m_ix = 0;
      }
      break;
    case Menu::ACTION:
      // Execute action and fall back to menu root
      {
	Menu::action_P action = (Menu::action_P) item;
	Menu::Action* obj = (Menu::Action*) pgm_read_word(&action->obj);
	bool res = obj->run(item);
	m_top = 0;
	m_ix = 0;
	if (!res) return;
      }
      break;
    default:
      // Enter item modification mode
      m_selected = !m_selected;
      m_bv = 0;
    }
    break;
  case LEFT_KEY:
    // Exit item modification mode or walk back
    if (m_selected) {
      m_selected = false;
    }
    else if (m_top > 0) {
      m_top -= 1;
      m_ix = 0;
    }
    break;
  case DOWN_KEY:
    // Step to the next menu item or value in item modification mode
    if (!m_selected) {
      m_ix += 1;
      item = (Menu::item_P) pgm_read_word(&list[m_ix]);
      if (item == NULL) m_ix -= 1;
    }
    else {
      switch (type) {
      case Menu::ONE_OF:
	// Step to the next enumeration value
	{
	  Menu::one_of_P evar = (Menu::one_of_P) item;
	  uint16_t* vp = (uint16_t*) pgm_read_word(&evar->value);
	  uint16_t value = *vp + 1;
	  list = (Menu::item_vec_P) pgm_read_word(&evar->list);
	  item = (Menu::item_P) pgm_read_word(&list[value]);
	  if (item == NULL) break;
	  *vp = value;
	}
	break;
      case Menu::ZERO_OR_MANY:
	// Step to the next item
	{
	  Menu::zero_or_many_P bitset = (Menu::zero_or_many_P) item;
	  list = (Menu::item_vec_P) pgm_read_word(&bitset->list);
	  item = (Menu::item_P) pgm_read_word(&list[m_bv + 1]);
	  if (item == NULL) break;
	  m_bv += 1;
	}
	break;
      case Menu::INT_RANGE:
	// Decrement the integer variable if within the range
	{
	  Menu::int_range_P range = (Menu::int_range_P) item;
	  int16_t* vp = (int16_t*) pgm_read_word(&range->value);
	  int value = *vp;
	  int low = (int) pgm_read_word(&range->low);
	  if (value == low) break;
	  *vp = value - 1;
	}
	break;
      default:
	;
      }
    }
    break;
  case UP_KEY:
    // Step to the previous menu item or value in item modification mode
    if (!m_selected) {
      if (m_ix > 0)
	m_ix -= 1;
      else if (m_top > 0) {
	m_top -= 1;
      }
    }
    else {
      switch (type) {
      case Menu::ONE_OF:
	// Step to the previous enumeration value
	{
	  Menu::one_of_P evar = (Menu::one_of_P) item;
	  uint16_t* vp = (uint16_t*) pgm_read_word(&evar->value);
	  uint16_t value = *vp;
	  if (value == 0) break;
	  value -= 1;
	  list = (Menu::item_vec_P) pgm_read_word(&evar->list);
	  item = (Menu::item_P) pgm_read_word(&list[value]);
	  *vp = value;
	}
	break;
      case Menu::ZERO_OR_MANY:
	// Step to the previous bitset value
	{
	  if (m_bv == 0) {
	    m_selected = 0;
	    break;
	  }
	  m_bv -= 1;
	}
	break;
      case Menu::INT_RANGE:
	// Increment the integer variable in within range
	{
	  Menu::int_range_P range = (Menu::int_range_P) item;
	  int16_t* vp = (int16_t*) pgm_read_word(&range->value);
	  int value = *vp;
	  int high = (int) pgm_read_word(&range->high);
	  if (value == high) break;
	  *vp = value + 1;
	}
	break;
      default:
	;
      }
    }
    break;
  }

  // Display the new walker state
  m_out << clear << *this;
}

Menu::type_t
Menu::Walker::type()
{
  if (!m_selected) return (ITEM_LIST);
  Menu::item_list_P menu = m_stack[m_top];
  Menu::item_vec_P list = (Menu::item_vec_P) pgm_read_word(&menu->list);
  Menu::item_P item = (Menu::item_P) pgm_read_word(&list[m_ix]);
  Menu::type_t type = (Menu::type_t) pgm_read_byte(&item->type);
  return (type);
}

void
Menu::RotaryController::on_event(uint8_t type, uint16_t direction)
{
  UNUSED(type);
  if (m_walker->type() == Menu::INT_RANGE)
    m_walker->on_key_down(direction == CW ?
			  Menu::Walker::UP_KEY :
			  Menu::Walker::DOWN_KEY);
  else
    m_walker->on_key_down(direction == CW ?
			  Menu::Walker::DOWN_KEY :
			  Menu::Walker::UP_KEY);
}
