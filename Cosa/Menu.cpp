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
  Menu::item_list_P menu = walker.m_stack[walker.m_top];
  Menu::item_vec_P list = (Menu::item_vec_P) pgm_read_word(&menu->list);
  Menu::item_P item = &menu->item;
  Menu::type_t type;

  outs << (const char*) pgm_read_word(&item->name) << ':';
  item = (Menu::item_P) pgm_read_word(&list[walker.m_ix]);
  outs << (const char*) pgm_read_word(&item->name) << endl;
  type = (Menu::type_t) pgm_read_byte(&item->type);
  switch (type) {
  case Menu::ITEM_LIST:
    break;
  case Menu::ENUM:
    {
      Menu::enum_P evar = (Menu::enum_P) item;
      uint16_t ix = *((uint16_t*) pgm_read_word(&evar->value));
      list = (Menu::item_vec_P) pgm_read_word(&evar->list);
      item = (Menu::item_P) pgm_read_word(&list[ix]);
      outs << (const char*) pgm_read_word(&item->name);
    }
    break;
  case Menu::RANGE:
    {
      Menu::range_P range = (Menu::range_P) item;
      int16_t* rvp = (int*) pgm_read_word(&range->value);
      outs << *rvp << PSTR(" [")
	   << (int16_t) pgm_read_word(&range->low) << PSTR("..")
	   << (int16_t) pgm_read_word(&range->high) << PSTR("]");
    }
    break;
  case Menu::ACTION:
    {
      Menu::action_P action = (Menu::action_P) item;
      outs << PSTR("action = ") << (void*) pgm_read_word(&action->obj);
    }
    break;
  }
}

void 
Menu::Walker::on_key_down(uint8_t nr)
{
  Menu::item_list_P menu = m_stack[m_top];
  Menu::item_vec_P list = (Menu::item_vec_P) pgm_read_word(&menu->list);
  Menu::item_P item = (Menu::item_P) pgm_read_word(&list[m_ix]);
  Menu::type_t type = (Menu::type_t) pgm_read_byte(&item->type);

  switch (nr) {
  case NO_KEY: 
    break;
  case SELECT_KEY: 
  case RIGHT_KEY:
    switch (type) {
    case Menu::ITEM_LIST:
      {
	m_stack[++m_top] = (Menu::item_list_P) item;
	m_ix = 0;
      }
      break;
    case Menu::ACTION:
      {
	Menu::action_P action = (Menu::action_P) item;
	Menu::Action* obj = (Menu::Action*) pgm_read_word(&action->obj);
	obj->run(item);
	m_top = 0;
	m_ix = 0;
      }
      break;
    default:
      m_selected = true;
    }
    break;
  case LEFT_KEY: 
    if (m_selected) {
      m_selected = false;
    } 
    else if (m_top > 0) {
      m_top -= 1;
      m_ix = 0;
    }
    break;
  case DOWN_KEY: 
    if (!m_selected) {
      m_ix += 1;
      item = (Menu::item_P) pgm_read_word(&list[m_ix]);
      if (item == 0) m_ix -= 1;
    }
    else {
      switch (type) {
      case Menu::ENUM:
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
      case Menu::RANGE:
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
    if (!m_selected) {
      if (m_ix > 0) m_ix -= 1;
    }
    else {
      switch (type) {
      case Menu::ENUM:
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
      case Menu::RANGE:
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
  m_out << clear;
  if (m_selected) m_out << '*';
  m_out << *this;
}
