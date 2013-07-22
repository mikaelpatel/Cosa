/**
 * @file Cosa/Menu.hh
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Menu.hh"

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
  outs << (const char*) pgm_read_word(&item->name) << ':';
  item = (Menu::item_P) pgm_read_word(&list[walker.m_ix]);
  outs << (const char*) pgm_read_word(&item->name) << endl;
  type = (Menu::type_t) pgm_read_byte(&item->type);

  // Print possible value of current menu item
  switch (type) {
  case Menu::ENUM:
    // Print the enumeration variable value string
    {
      Menu::enum_P var = (Menu::enum_P) item;
      uint16_t ix = *((uint16_t*) pgm_read_word(&var->value));
      list = (Menu::item_vec_P) pgm_read_word(&var->list);
      item = (Menu::item_P) pgm_read_word(&list[ix]);
      outs << (const char*) pgm_read_word(&item->name);
    }
    break;
    // Print the bitset variable when selected
  case Menu::BITSET:
    {
      if (!walker.m_selected) break;
      Menu::bitset_P var = (Menu::bitset_P) item;
      uint16_t value = *((uint16_t*) pgm_read_word(&var->value));
      list = (Menu::item_vec_P) pgm_read_word(&var->list);
      item = (Menu::item_P) pgm_read_word(&list[walker.m_bv]);
      if (value & _BV(walker.m_bv)) 
	outs << PSTR("[x] ");
      else outs << PSTR("[ ] ");
      outs << (const char*) pgm_read_word(&item->name);
    }
    break;
    // Print the range variable and limits when selected
  case Menu::RANGE:
    {
      Menu::range_P var = (Menu::range_P) item;
      int16_t* vp = (int*) pgm_read_word(&var->value);
      int16_t value = *vp;
      outs << value;
      if (!walker.m_selected) break;
      outs << PSTR(" [") 
	   << (int16_t) pgm_read_word(&var->low) 
	   << PSTR("..")
	   << (int16_t) pgm_read_word(&var->high) 
	   << PSTR("]");
    }
    break;
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
    case Menu::BITSET:
      // Select bitset or toggle current bitset item
      {
	if (!m_selected) {
	  m_selected = true;
	  m_bv = 0;
	  break;
	}
	Menu::bitset_P var = (Menu::bitset_P) item;
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
      m_selected = true;
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
      if (item == 0) m_ix -= 1;
    }
    else {
      switch (type) {
      case Menu::ENUM:
	// Step to the next enumeration value
	{
	  Menu::enum_P evar = (Menu::enum_P) item;
	  uint16_t* vp = (uint16_t*) pgm_read_word(&evar->value);
	  uint16_t value = *vp + 1;
	  list = (Menu::item_vec_P) pgm_read_word(&evar->list);	  
	  item = (Menu::item_P) pgm_read_word(&list[value]);
	  if (item == 0) break;
	  *vp = value;
	}
	break;
      case Menu::BITSET:
	// Step to the next bitset value
	{
	  Menu::enum_P bitset = (Menu::enum_P) item;
	  list = (Menu::item_vec_P) pgm_read_word(&bitset->list);	  
	  item = (Menu::item_P) pgm_read_word(&list[m_bv + 1]);
	  if (item == 0) break;
	  m_bv += 1;
	}
	break;
      case Menu::RANGE:
	// Decrement the integer variable if within the range
	{
	  Menu::range_P range = (Menu::range_P) item;
	  int16_t* vp = (int16_t*) pgm_read_word(&range->value);
	  int value = *vp;
	  int low = (int) pgm_read_word(&range->low);
	  if (value == low) break;
	  *vp = value - 1;
	}
	break;
      }
    }
    break;
  case UP_KEY: 
    // Step to the previous menu item or value in item modification mode
    if (!m_selected) {
      if (m_ix > 0) m_ix -= 1;
    }
    else {
      switch (type) {
      case Menu::ENUM:
	// Step to the previous enumeration value
	{
	  Menu::enum_P evar = (Menu::enum_P) item;
	  uint16_t* vp = (uint16_t*) pgm_read_word(&evar->value);
	  uint16_t value = *vp;
	  if (value == 0) break;
	  value -= 1;
	  list = (Menu::item_vec_P) pgm_read_word(&evar->list);	  
	  item = (Menu::item_P) pgm_read_word(&list[value]);
	  *vp = value;
	}
	break;
      case Menu::BITSET:
	// Step to the previous bitset value
	{
	  if (m_bv == 0) break;
	  m_bv -= 1;
	}
      case Menu::RANGE:
	// Increment the integer variable in within range
	{
	  Menu::range_P range = (Menu::range_P) item;
	  int16_t* vp = (int16_t*) pgm_read_word(&range->value);
	  int value = *vp;
	  int high = (int) pgm_read_word(&range->high);
	  if (value == high) break;
	  *vp = value + 1;
	}
	break;
      }
    }
    break;
  }

  // Display the new walker state
  m_out << clear << *this;
}
