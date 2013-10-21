/**
 * @file Cosa/Registry.cpp
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

#include "Cosa/Registry.hh"

Registry::item_P 
Registry::lookup(const uint8_t* path, size_t count)
{
  // Check for root path
  item_P item = (item_P) m_root;
  if (path == NULL) return (item);

  // Paths should not exceed the maximum length
  if (count > PATH_MAX) return (NULL);
  for (uint8_t i = 0; i < count; i++) {
    // Check that the current item is a list
    uint8_t ix = path[i]; 
    uint8_t nx = i + 1;
    type_t type = (type_t) pgm_read_byte(&item->type);
    if ((nx < count) && (type != ITEM_LIST)) return (NULL);

    // Check that the current index is within the list length
    item_list_P items = (item_list_P) item;
    uint8_t len = pgm_read_byte(&items->length);
    if (ix >= len) return (NULL);

    // Read the item and step to the next path index
    item_vec_P vec = (item_vec_P) pgm_read_word(&items->list);
    item = (item_P) pgm_read_word(&vec[ix]);
  }

  // Return path indexed item
  return (item);
}

int 
Registry::run(action_P action, void* buf, size_t size)
{
  // Sanity check the parameters
  if (action == NULL) return (-1);
  if (pgm_read_byte(&action->item.type) != ACTION) return (-2);

  // Access the action object 
  Action* obj = (Action*) pgm_read_word(&action->obj);
  if (obj == NULL) return (-3);

  // And run the member function
  return (obj->run(buf, size));
}
  
int 
Registry::get_value(blob_P blob, void* buf, size_t len)
{
  // Sanity check the parameters
  if (len == 0) return (0);
  if ((blob == NULL) || (buf == NULL)) return (-1);
  if (pgm_read_byte(&blob->item.type) < BLOB) return (-2);

  // Check size of blob
  size_t size = (size_t) pgm_read_word(&blob->size);
  if (size == 0) return (0);
  if (size > len) return (-4);

  // Check where the value is stored and copy into buffer
  storage_t storage = get_storage(&blob->item);
  if (storage == IN_PROGMEM) 
    memcpy_P(buf, (const void*) pgm_read_word(&blob->value), size);
  else if (storage == IN_SRAM) 
    memcpy(buf, (const void*) pgm_read_word(&blob->value), size);
  else if (storage == IN_EEMEM && m_eeprom != NULL)
    m_eeprom->read(buf, (const void*) pgm_read_word(&blob->value), size);
  else return (-5);

  // Return the number of bytes read
  return (size);
}

int 
Registry::set_value(blob_P blob, const void* buf, size_t len)
{
  // Sanity check the parameters
  if (len == 0) return (0);
  if ((blob == NULL) || (buf == NULL)) return (-1);
  if (pgm_read_byte(&blob->item.type) < BLOB) return (-2);
  if (is_readonly(&blob->item)) return (-3);

  // Check size of blob
  size_t size = (size_t) pgm_read_word(&blob->size);
  if (size == 0) return (0);
  if (size != len) return (-4);

  // Check where the value is stored and copy into buffer
  storage_t storage = get_storage(&blob->item);
  if (storage == IN_SRAM) 
    memcpy((void*) pgm_read_word(&blob->value), buf, size);
  else if (storage == IN_EEMEM && m_eeprom != NULL)
    m_eeprom->write((void*) pgm_read_word(&blob->value), buf, size);
  else return (-5);

  // Return the number of bytes written
  return (size);
}

IOStream& operator<<(IOStream& outs, Registry::item_P item)
{
  outs << PSTR("item@") << (void*) item;
  if (item != NULL)
    outs << PSTR("(type = ") << Registry::get_type(item)
	 << PSTR(", name = ") << Registry::get_name(item)
	 << PSTR(", storage = ") << Registry::get_storage(item)
	 << PSTR(", readonly = ") << Registry::is_readonly(item)
	 << PSTR(")");
  else
    outs << PSTR("(NULL)");
  return (outs);
}

IOStream& operator<<(IOStream& outs, Registry::item_list_P list)
{
  if (list == NULL) return (outs);
  Registry::Iterator iter(list);
  Registry::item_P item;
  while ((item = iter.next()) != NULL)
    outs << item << endl;
  return (outs);
}

