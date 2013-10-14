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
    ITEM = 0,			// Item descriptor
    ITEM_LIST,			// List of items
    ACTION,			// Action function
    BLOB,			// General Binary Object
    APPL,			// Application Binary Object
  };
  
  /**
   * Storage tags
   */
  enum storage_t {
    IN_SRAM,			// In data memory
    IN_PROGMEM,			// In program memory
    IN_EEMEM			// In eeprom 
  };

  // Pointer type for character string in program memory
  typedef const PROGMEM char* str_P;

  // Registry item header
  struct item_t {
    type_t type;		// Item type tag(ITEM)
    str_P name;			// Item string in program memory
    storage_t storage;		// Value storage

  };
  typedef const PROGMEM item_t* item_P;
  typedef const PROGMEM item_P* item_vec_P;
  
  /**
   * Return item type given pointer to item in program memory.
   * @param [in] item pointer to item in program memory.
   * @return type_t
   */
  static type_t get_type(item_P item)
  {
    return ((type_t) pgm_read_byte(&item->type));
  }

  /**
   * Return pointer to name string given pointer to item in program
   * memory.  
   * @param [in] item pointer to item in program memory.
   * @return pointer to string in program memory.
   */
  static str_P get_name(item_P item)
  {
    return ((str_P) pgm_read_word(&item->name));
  }

  /**
   * Return item storage type given pointer to item in program memory.
   * @param [in] item pointer to item in program memory.
   * @return storage_t
   */
  static storage_t get_storage(item_P item)
  {
    return ((storage_t) pgm_read_byte(&item->storage));
  }

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
     * @override Registry::Action
     * Registry action function for given registry item. Should return
     * number of bytes in buffer as return value or negative error
     * code. 
     * @param[inout] buf pointer to parameter block.
     * @param[in] size of parameter block.
     * @return number of bytes or negative error code.
     */
    virtual int run(void* buf, size_t size) 
    {
      return (0);
    }
  };

  // Registry action item
  struct action_t {
    item_t item;		// Item header(ACTION)
    Action* obj;		// Pointer to action handler
  };
  typedef const PROGMEM action_t* action_P;

  /**
   * Type check given item pointer and convert to an action pointer. 
   * Return NULL if the item is not tagged as an ACTION.
   * @param [in] item pointer to item in program memory.
   * @return pointer to action in program memory.
   */
  static action_P to_action(item_P item)
  {
    type_t type = get_type(item);
    return (type == ACTION ? (action_P) item : NULL);
  }

  /**
   * Run the action item with the given argument block and given
   * number of bytes. Return number of return bytes in buffer or
   * negative error code.
   * @param [in] action item.
   * @param [inout] buf argument/result buffer.
   * @param [in] size number of bytes argument.
   * @return number of bytes result or negative error code.
   */
  static int run(action_P action, void* buf, size_t size);
  
  // Binary object variable 
  struct blob_t {
    item_t item;		// Item header(>= BLOB)
    void* value;		// Pointer to value
    size_t size;		// Size of object
  };
  typedef const PROGMEM blob_t* blob_P;

  /**
   * Type check given item pointer and convert to a blob pointer. 
   * Return NULL if the item is not tagged as a BLOB.
   * @param [in] item pointer to item in program memory.
   * @return pointer to blob in program memory.
   */
  static blob_P to_blob(item_P item)
  {
    type_t type = get_type(item);
    return (type == BLOB ? (blob_P) item : NULL);
  }

  /**
   * Copy blob value into given buffer with given maximum size
   * (len). Return number of bytes copied into buffer or negative
   * error code.
   * @param [in] blob pointer to blob in program memory.
   * @param [out] buf pointer to buffer for value.
   * @param [in] len number of bytes maximum in buffer. 
   * @return number of bytes or negative error code. 
   */
  static int get_value(blob_P blob, void* buf, size_t len);
  
  /** Max length of a path */
  static const size_t PATH_MAX = 8;
  
  /**
   * Construct registery root object.
   * @param[in] root item list.
   */
  Registry(item_list_P root) : m_root(root) {}
  
  /**
   * Lookup registry item for given path. Returns pointer to item if
   * found otherwise NULL(0).
   * @param[in] path registry index sequence.
   * @param[in] count number of bytes in path.
   * @return item pointer or NULL.
   */
  item_P lookup(const uint8_t* path = NULL, size_t count = 0);

  /**
   * Lookup registry item for given path. If the item is an action
   * call the run() member function with the given argument block.
   * Return error code(-1) if the path is not valid otherwise return
   * value from run().
   * @param[in] path registry index sequence.
   * @param[in] count number of bytes in path.
   * @param[inout] buf pointer to argument/result block.
   * @param[in] len of argument block.
   * @return negative error code or return value from action.
   */
  int apply(const uint8_t* path, size_t count, void* buf, size_t len)
  {
    return (run(to_action(lookup(path, count)), buf, len));
  }

private:
  item_list_P m_root;
};

/**
 * Support macro to start the definition of a registery item list in
 * program memory. 
 * Used in the form:
 *   REGISTRY_BEGIN(var,name)
 *     REGISTRY_XXX_ITEM(item-1)
 *     ...
 *     REGISTRY_XXX_ITEM(item-n)
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
#define REGISTRY_LIST_ITEM(var) &var.item,
#define REGISTRY_BLOB_ITEM(var) &var ## _blob.item,
#define REGISTRY_ACTION_ITEM(var) &var ## _action.item,

/**
 * Support macro to complete a registry in program memory.
 * @param[in] var registry to create.
 */
#define REGISTRY_END(var)				\
  };							\
  const Registry::item_list_t var PROGMEM = {		\
    {							\
      Registry::ITEM_LIST,				\
      var ## _name,					\
      Registry::IN_PROGMEM				\
    },							\
    membersof(var ## _list),				\
    var ## _list					\
  };

/**
 * Support macro to define a registry action in program memory.
 * @param[in] var registry action item to create.
 * @param[in] name string of registry item.
 */
#define REGISTRY_ACTION(var,name)			\
  const char var ## _name[] PROGMEM = name;		\
  const Registry::action_t var ## _action PROGMEM = {	\
    {							\
      Registry::ACTION,					\
      var ## _name,					\
      Registry::IN_SRAM					\
    },							\
    &var						\
  };

/**
 * Support macro to define a registry binary object item in program
 * memory. 
 * @param[in] var registry range item to create.
 * @param[in] name string of registry item.
 * @param[in] value name.
 * @param[in] size of value.
 */
#define REGISTRY_BLOB(var,name,mem)			\
  const char var ## _blob_name[] PROGMEM = name;	\
  const Registry::blob_t var ## _blob PROGMEM = {	\
    {							\
      Registry::BLOB,					\
      var ## _blob_name,				\
      Registry::IN_ ## mem				\
    },							\
    (void*) &var,					\
    sizeof(var)						\
  };

#endif

