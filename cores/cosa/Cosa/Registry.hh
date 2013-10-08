/**
 * @file Cosa/Registry.hh
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

#ifndef __COSA_REGISTRY_HH__
#define __COSA_REGISTRY_HH__

#include "Cosa/Types.h"

/**
 * Cosa Configuration Registry. Allow path (x0.x1..xn) access to
 * registry items. Supports actions and binary objects in tree
 * structure. The binary object is a mapping from program memory to
 * data. The low level access is type-less. Applications may add
 * run-time data-types by extending the item type system. Any type tag
 * larger than BLOB may be used. 
 */
class Registry {
public:
  /**
   * Type tags
   */
  enum type_t {
    ITEM,			// Item descriptor
    ITEM_LIST,			// List of items
    ACTION,			// Action function
    BLOB,			// General Binary Object
    APPL			// Application Binary Object
  };

  // Pointer type for character string in program memory
  typedef const PROGMEM char* str_P;

  // Registry item header
  struct item_t {
    type_t type;		// Item type tag(ITEM)
    str_P name;			// Item string in program memory
  };
  typedef const PROGMEM item_t* item_P;
  typedef const PROGMEM item_P* item_vec_P;

  // Registry item lists
  struct item_list_t {
    item_t item;		// Item header(ITEM_LIST)
    uint8_t length;		// Item list length (for boundary checking)
    item_vec_P list;		// Item list in program memory
  };
  typedef const PROGMEM item_list_t* item_list_P;

  /**
   * Registry Action handler. Must be sub-classed and the virtual member
   * function run() must be implemented. Called after lookup by apply().
   */
  class Action {
  public:
    /**
     * @override
     * Registry action function for given registry item. Should return
     * true(1) if successful otherwise false(0).
     * @param[in] item registry item reference.
     * @param[in] buf pointer to parameter block.
     * @param[in] size of parameter block.
     * @return bool
     */
    virtual bool run(Registry::item_P item, const void* buf, size_t size) = 0;
  };

  // Registry action item
  struct action_t {
    item_t item;		// Item header(ACTION)
    Action* obj;		// Pointer to action handler
  };
  typedef const PROGMEM action_t* action_P;

  // Binary object variable 
  struct blob_t {
    item_t item;		// Item header(>= BLOB)
    void* value;		// Pointer to value
    size_t size;		// Size of object
  };
  typedef const PROGMEM blob_t* blob_P;

  /** End Of Path value */
  static const uint8_t EOP= 0xff;

  /** Max length of a path */
  static const size_t PATH_MAX = 8;
  
  /**
   * Construct registery root object.
   */
  Registry(item_list_P root) : m_root(root) {}
  
  /**
   * Lookup registry item for given path. Returns pointer to item if
   * found otherwise NULL(0).
   * @param[in] path registry index sequence.
   */
  item_P lookup(const uint8_t* path);

  /**
   * Lookup registry item for given path. If the item is an action
   * call the run() member function with the given argument block.
   * Return error code(-1) if the path is not valid otherwise return
   * value from run().
   * @param[in] path registry index sequence.
   * @param[in] arg pointer to argument block.
   * @param[in] size of argument block.
   * @return negative error code or return value from action.
   */
  int apply(const uint8_t* path, const void* arg, size_t size);

private:
  item_list_P m_root;
};

/**
 * Support macro to start the definition of a registery item list in
 * program memory. 
 * Used in the form:
 *   REGISTRY_BEGIN(var,name)
 *     REGISTRY_ITEM(item-1)
 *     ...
 *     REGISTRY_ITEM(item-n)
 *   REGISTRY_END(var,n)
 * @param[in] var registry item list to create.
 * @param[in] name string for registry item.
 */
#define REGISTRY_BEGIN(var,name)			\
  const char var ## _name[] PROGMEM = name;		\
  const Registry::item_P var ## _list[] PROGMEM = {  

/**
 * Support macro to add a registry item to an item list in program
 * memory. The item can be any of the registry item types; ITEM,
 * ITEM_LIST, ACTION and BLOB.
 * @param[in] var item reference to add.
 */
#define REGISTRY_ITEM(var)				\
  &var.item,

/**
 * Support macro to complete a registry in program memory.
 * @param[in] var registry to create.
 */
#define REGISTRY_END(var,length)			\
  };							\
  const Registry::item_list_t var PROGMEM = {		\
    {							\
      Registry::ITEM_LIST,				\
      var ## _name					\
    },							\
    length,						\
    var ## _list					\
  };

/**
 * Support macro to define a registry action in program memory.
 * @param[in] var registry action item to create.
 * @param[in] name string of registry item.
 * @param[in] obj pointer to registry action handler.
 */
#define REGISTRY_ACTION(var,name,obj)			\
  const char var ## _name[] PROGMEM = name;		\
  const Registry::action_t var PROGMEM = {		\
    {							\
      Registry::ACTION,					\
      var ## _name					\
    },							\
  &obj							\
};

/**
 * Support macro to define a registry binary object item in program
 * memory. 
 * @param[in] var registry range item to create.
 * @param[in] name string of registry item.
 * @param[in] value name.
 * @param[in] size of value.
 */
#define REGISTRY_BLOB(var,name,value,size)		\
  const char var ## _name[] PROGMEM = name;		\
  const Registry::blob_t var PROGMEM = {		\
  {							\
    Registry::BLOB,					\
    var ## _name					\
  },							\
  &value,						\
  size							\
};

#endif

