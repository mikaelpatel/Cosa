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
Registry::lookup(const uint8_t* path)
{
  // Check for root path
  item_P item = (item_P) m_root;
  if (path == NULL) return (item);
  // Paths should not exceed the maximum length
  for (uint8_t i = 0; i < PATH_MAX; i++) {
    uint8_t ix = path[i]; 
    // Check if we have reached the end of the path
    if (ix == EOP) return (item);
    // Check that the current item is a list
    type_t type = (type_t) pgm_read_byte(&item->type);
    if ((path[i + 1] != EOP) && (type != ITEM_LIST)) return (NULL);
    // Check that the current index is within the list length
    item_list_P items = (item_list_P) item;
    uint8_t len = pgm_read_byte(&items->length);
    if (ix >= len) return (NULL);
    // Read the item and step to the next path index
    item_vec_P vec = (item_vec_P) pgm_read_word(&items->list);
    item = (item_P) pgm_read_word(&vec[ix]);
  }
  return (NULL);
}

int 
Registry::apply(const uint8_t* path, const void* arg, size_t size)
{
  // Lookup the item given the path
  item_P item = lookup(path);
  // Check that it is valid and an action
  if (item == NULL) return (-1);
  type_t type = (type_t) pgm_read_byte(&item->type);
  if (type != ACTION) return (-1);
  // Apply the action with the given argument block
  action_P action = (action_P) item;
  Action* obj = (Action*) pgm_read_word(&action->obj);
  return (obj->run(item, arg, size));
}
