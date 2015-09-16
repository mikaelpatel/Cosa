/**
 * @file Registry.hh
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

#ifndef COSA_REGISTRY_HH
#define COSA_REGISTRY_HH

#include "Cosa/Types.h"
#include "Cosa/EEPROM.hh"
#include "Cosa/IOStream.hh"

/**
 * Cosa Configuration Registry. Allow path (x0.x1..xn) access to
 * registry items. Supports actions and binary objects in tree
 * structure. The binary object is a mapping from program memory to
 * data stored in SRAM, PROGMEM or EEMEM. The low level access is
 * type-less. Applications may add run-time data-types by extending
 * the item type system. Any type tag larger than BLOB may be used.
 */
class Registry {
public:
  /**
   * Registry item type tags.
   */
  enum type_t {
    ITEM = 0,			//!< Item descriptor.
    ITEM_LIST = 1,		//!< List of items.
    ACTION = 2,			//!< Action function.
    BLOB = 3,			//!< General binary object.
    APPL = 4			//!< Application binary object.
  } __attribute__((packed));

  /**
   * Storage type tags.
   */
  enum storage_t {
    IN_PROGMEM = 0,		//!< In program memory.
    IN_SRAM = 1,		//!< In data memory.
    IN_EEMEM = 2,		//!< In eeprom.
  } __attribute__((packed));

  /** Mask storage bits of item attributes. */
  static const uint8_t STORAGE_MASK = 0x7f;

  /** Mask readonly bit of item attributes. */
  static const uint8_t READONLY = 0x80;

  /**
   * Registry item structure.
   */
  struct item_t {
    type_t type;		//!< Item type tag(ITEM).
    str_P name;			//!< Name string in program memory.
    uint8_t attr;		//!< Attributes.
  };

  /** Pointer to item in program memory. */
  typedef const PROGMEM item_t* item_P;

  /** Pointer to vector of item pointers in program memory. */
  typedef const PROGMEM item_P* item_vec_P;

  /**
   * Return item type given pointer to item in program memory.
   * @param [in] item pointer to item in program memory.
   * @return item type.
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
    __attribute__((always_inline))
  {
    return ((str_P) pgm_read_word(&item->name));
  }

  /**
   * Return item storage type given pointer to item in program memory.
   * @param [in] item pointer to item in program memory.
   * @return storage type.
   */
  static storage_t get_storage(item_P item)
    __attribute__((always_inline))
  {
    return ((storage_t) (pgm_read_byte(&item->attr) & STORAGE_MASK));
  }

  /**
   * Return true if the item storage is readonly otherwise false.
   * @param [in] item pointer to item in program memory.
   * @return bool.
   */
  static bool is_readonly(item_P item)
    __attribute__((always_inline))
  {
    return ((pgm_read_byte(&item->attr) & READONLY) != 0);
  }

  /**
   * Registry item list structure.
   */
  struct item_list_t {
    item_t item;		//!< Item header(ITEM_LIST).
    uint8_t length;		//!< Item vector length (for boundary checking).
    item_vec_P list;		//!< Item vector in program memory.
  };

  /** Pointer to item list in program memory. */
  typedef const PROGMEM item_list_t* item_list_P;

  /**
   * Type check given item pointer and convert it to an item list.
   * Return pointer to item list in program memory if successful
   * otherwise NULL. This function provides run-time type checking.
   * @param [in] item pointer to item in program memory.
   * @return pointer to item list in program memory.
   */
  static item_list_P to_list(item_P item)
    __attribute__((always_inline))
  {
    return (get_type(item) == ITEM_LIST ? (item_list_P) item : NULL);
  }

  /**
   * Return number of items in item vector or negative error code.
   * @param [in] list pointer to item list in program memory.
   * @return number of items.
   */
  static int get_length(item_list_P list)
    __attribute__((always_inline))
  {
    if (UNLIKELY(get_type(&list->item) != ITEM_LIST)) return (EINVAL);
    return ((int) pgm_read_byte(&list->length));
  }

  /**
   * Registry item list iterator.
   */
  class Iterator {
  private:
    item_vec_P m_vec;		//!< Item vector from item list.
    uint8_t m_length;		//!< Length of vector.
    uint8_t m_next;		//!< Current index in vector.
  public:
    /**
     * Construct iterator on given item list in program memory.
     * @param[in] list of items.
     */
    Iterator(item_list_P list) :
      m_vec((item_vec_P) pgm_read_word(&list->list)),
      m_length((uint8_t) pgm_read_byte(&list->length)),
      m_next(0)
    {}

    /**
     * Return the next item in the item list otherwise NULL.
     */
    item_P next()
      __attribute__((always_inline))
    {
      if (UNLIKELY(m_next == m_length)) return (NULL);
      return ((item_P) pgm_read_word(&m_vec[m_next++]));
    }

    /**
     * Reset iterator to start position.
     */
    void reset()
      __attribute__((always_inline))
    {
      m_next = 0;
    }
  };

  /**
   * Registry Action handler. Must be sub-classed and the virtual member
   * function run() must be implemented. Called after lookup by apply().
   */
  class Action {
  public:
    /**
     * @override{Registry::Action}
     * Registry action function for given registry item. Should return
     * number of bytes in buffer as return value or negative error
     * code (EINVAL if illegal parameter, ENXIO if no object defined).
     * @param[in,out] buf pointer to parameter block.
     * @param[in] size of parameter block.
     * @return number of bytes or negative error code.
     */
    virtual int run(void* buf, size_t size) = 0;
  };

  /**
   * Registry action item.
   */
  struct action_t {
    item_t item;		//!< Item header(ACTION).
    Action* obj;		//!< Pointer to action handler instance.
  };

  /** Pointer to action item in program memory. */
  typedef const PROGMEM action_t* action_P;

  /**
   * Type check given item pointer and convert to an action pointer.
   * Returns a pointer to action item in program memory if successful
   * otherwise NULL. This function provides run-time type checking.
   * @param [in] item pointer to item in program memory.
   * @return pointer to action in program memory.
   */
  static action_P to_action(item_P item)
    __attribute__((always_inline))
  {
    return (get_type(item) == ACTION ? (action_P) item : NULL);
  }

  /**
   * Run the action item with the given argument block and given
   * number of bytes. Return number of bytes in buffer (return value)
   * or negative error code (EINVAL if illegal parameter, ENXIO if no
   * object defined).
   * @param [in] action item.
   * @param [in,out] buf argument/result buffer.
   * @param [in] size number of bytes argument.
   * @return number of bytes result or negative error code.
   */
  static int run(action_P action, void* buf, size_t size);

  /**
   * Registry binary object variable. Mappning between registry and
   * application data.
   */
  struct blob_t {
    item_t item;		//!< Item header(>= BLOB).
    void* value;		//!< Pointer to value.
    size_t size;		//!< Size of object.
  };

  /** Pointer to blob registry item in program memory. */
  typedef const PROGMEM blob_t* blob_P;

  /**
   * Type check given item pointer and convert to a blob pointer.
   * Returns pointer to blob registry item in program memory if
   * successful otherwise NULL. This function provides run-time type
   * checking.
   * @param [in] item pointer to item in program memory.
   * @return pointer to blob in program memory.
   */
  static blob_P to_blob(item_P item)
  {
    return (get_type(item) >= BLOB ? (blob_P) item : NULL);
  }

  /**
   * Copy blob value into given buffer with given maximum size
   * (len). Return number of bytes copied into buffer or negative
   * error code (EINVAL if illegal parameter, ENOBUFS if the buffer is
   * too small for the stored data, ENXIO if storage is not recognized).
   * @param [in] blob pointer to blob in program memory.
   * @param [out] buf pointer to buffer for value.
   * @param [in] len number of bytes maximum in buffer.
   * @return number of bytes or negative error code.
   */
  int get_value(blob_P blob, void* buf, size_t len);

  /**
   * Template function to copy blob value into given value storage
   * of given type. Returns true(1) if successful otherwise false(0).
   * @param [in] T type of variable to copy.
   * @param [in] blob pointer to blob in program memory.
   * @param [out] value buffer.
   * @return boolean.
   */
  template<class T> bool get_value(blob_P blob, T* value)
  {
    return (get_value(blob, value, sizeof(T)) == sizeof(T));
  }

  /**
   * Copy value in given buffer of given maximum size (len) to blob.
   * Return number of bytes copied from buffer or negative error
   * code (EINVAL if illegal parameter, EACCES if the blob is
   * readonly, ENOBUFS if the buffer is too small for the stored data,
   * ENXIO if storage is not recognized). The storage type must be
   * SRAM or EEMEM.
   * @param [in] blob pointer to blob in program memory.
   * @param [in] buf pointer to buffer with new value.
   * @param [in] len number of bytes maximum in buffer.
   * @return number of bytes or negative error code.
   */
  int set_value(blob_P blob, const void* buf, size_t len);

  /**
   * Template function to copy given value of given type to
   * blob. Return true(1) if successful otherwise false(0).
   * The storage type must be SRAM or EEMEM.
   * @param [in] T value type.
   * @param [in] blob pointer to blob in program memory.
   * @param [in] value value to set.
   * @return boolean.
   */
  template<class T> bool set_value(blob_P blob, const T* value)
  {
    return (set_value(blob, value, sizeof(T)) == sizeof(T));
  }

  /** Max length of a path. */
  static const size_t PATH_MAX = 8;

  /**
   * Construct registery root object.
   * @param[in] root item list.
   * @param[in] eeprom device driver (default internal EEPROM).
   */
  Registry(item_list_P root, EEPROM::Device* eeprom = NULL) :
    m_root(root),
    m_eeprom(eeprom == NULL ? &EEPROM::Device::eeprom : eeprom)
  {}

  /**
   * Lookup registry item for given path. Returns pointer to item if
   * found otherwise NULL. Default parameters will give root item.
   * @param[in] path registry index sequence (Default NULL).
   * @param[in] count number of bytes in path (Default 0).
   * @return item pointer or NULL.
   */
  item_P lookup(const uint8_t* path = NULL, size_t count = 0);

  void print(IOStream& outs, const uint8_t* path, size_t count);

  /**
   * Lookup registry item for given path and if the item is an action
   * call the run() member function with the given argument block.
   * Return error code(-1) if the path is not valid otherwise return
   * value from run().
   * @param[in] path registry index sequence.
   * @param[in] count number of bytes in path.
   * @param[in,out] buf pointer to argument/result block.
   * @param[in] len of argument block.
   * @return negative error code or return value from action.
   */
  int apply(const uint8_t* path, size_t count, void* buf, size_t len)
  {
    return (run(to_action(lookup(path, count)), buf, len));
  }

private:
  /** Root item list. */
  item_list_P m_root;

  /** EEPROM device driver. */
  EEPROM::Device* m_eeprom;
};

/**
 * Print item to given output stream.
 * @param[in] outs output stream.
 * @param[in] item registry item.
 * @return output stream.
 */
IOStream& operator<<(IOStream& outs, Registry::item_P item);

/**
 * Print item list to given output stream.
 * @param[in] outs output stream.
 * @param[in] list registry item list.
 * @return output stream.
 */
IOStream& operator<<(IOStream& outs, Registry::item_list_P list);

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
  const char var ## _name[] __PROGMEM = name;		\
  const Registry::item_P var ## _list[] __PROGMEM = {

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
  const Registry::item_list_t var __PROGMEM = {		\
    {							\
      Registry::ITEM_LIST,				\
      (str_P) var ## _name,				\
      Registry::IN_PROGMEM | Registry::READONLY		\
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
  const char var ## _name[] __PROGMEM = name;		\
  const Registry::action_t var ## _action __PROGMEM = {	\
    {							\
      Registry::ACTION,					\
      (str_P) var ## _name,				\
      Registry::IN_SRAM | Registry::READONLY		\
    },							\
    &var						\
  };

/**
 * Support macro to define a registry binary object item in program
 * memory.
 * @param[in] var registry blob to create.
 * @param[in] name string of registry item.
 * @param[in] mem storage type (SRAM, PROGMEM or EEMEM).
 * @param[in] readonly access.
 */
#define REGISTRY_BLOB(var,name,mem,readonly)		\
  const char var ## _blob_name[] __PROGMEM = name;	\
  const Registry::blob_t var ## _blob __PROGMEM = {	\
    {							\
      Registry::BLOB,					\
      (str_P) var ## _blob_name,			\
      Registry::IN_ ## mem | (readonly << 7)		\
    },							\
    (void*) &var,					\
    sizeof(var)						\
  };

/**
 * Support macro to define a registry binary object item in program
 * memory and variable in SRAM as reference.
 * @param[in] type variable data type.
 * @param[in] var registry blob to create.
 * @param[in] name string of registry item.
 * @param[in] value of variable (initial).
 * @param[in] readonly access.
 */
#define REGISTRY_BLOB_VAR(type,var,name,value,readonly)	\
  static type var = value;				\
  REGISTRY_BLOB(var,name,SRAM,readonly)

/**
 * Support macro to define a registry binary object item in program
 * memory and struct variable in SRAM as reference.
 * @param[in] type variable data type.
 * @param[in] var registry blob to create.
 * @param[in] name string of registry item.
 * @param[in] readonly access.
 */
#define REGISTRY_BLOB_STRUCT(type,var,name,readonly)	\
  static type var;					\
  REGISTRY_BLOB(var,name,SRAM,readonly)

/**
 * Support macro to define a registry binary object item in program
 * memory and string variable in program memory as reference. Values
 * is always READONLY.
 * @param[in] var registry blob to create.
 * @param[in] name string of registry item.
 * @param[in] value string for variable.
 */
#define REGISTRY_BLOB_PSTR(var,name,value)		\
  static const char var[] __PROGMEM = value;		\
  REGISTRY_BLOB(var,name,PROGMEM,true)

#endif

