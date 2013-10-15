/**
 * @file CosaRegistry.ino
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
 * @section Description
 * Cosa demonstration of Registry.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Registry.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"

static const char PRODUCT[] PROGMEM = "registry-demo";
static const char VERSION[] PROGMEM = "1.0";

REGISTRY_BLOB(PRODUCT, "product name", PROGMEM, true)
REGISTRY_BLOB(VERSION, "version string", PROGMEM, true)

REGISTRY_BEGIN(PIM, "product information management")
  REGISTRY_BLOB_ITEM(PRODUCT)	// 0.0
  REGISTRY_BLOB_ITEM(VERSION)	// 0.1
REGISTRY_END(PIM)

static int16_t NETWORK EEMEM = 0xC05A;
static uint8_t DEVICE EEMEM = 0x42;
static uint16_t TIMEOUT EEMEM = 2000;

REGISTRY_BLOB(NETWORK, "network address (16b)", EEMEM, false)
REGISTRY_BLOB(DEVICE, "device address (8b)", EEMEM, false)
REGISTRY_BLOB(TIMEOUT, "sleep period (ms)", EEMEM, false)

REGISTRY_BEGIN(CM, "configuration")
  REGISTRY_BLOB_ITEM(NETWORK)	// 1.0
  REGISTRY_BLOB_ITEM(DEVICE)	// 1.1
  REGISTRY_BLOB_ITEM(TIMEOUT)	// 1.2
REGISTRY_END(CM)

static uint16_t vcc = 4943;
static uint8_t load = 4;
static uint16_t errors = 0;

REGISTRY_BLOB(vcc, "battery status (mV)", SRAM, true)
REGISTRY_BLOB(load, "processor load (%)", SRAM, true)
REGISTRY_BLOB(errors, "error count", SRAM, false)

REGISTRY_BEGIN(STATUS, "status")
  REGISTRY_BLOB_ITEM(vcc)	// 2.0
  REGISTRY_BLOB_ITEM(load)	// 2.1
  REGISTRY_BLOB_ITEM(errors)	// 2.2
REGISTRY_END(STATUS)

class Restart : public Registry::Action {
public:
  virtual int run(void* buf, size_t size) 
  {
    trace << (const char*) buf << endl;
    return (strlen_P((const char*) buf));
  }
};
Restart restart;
REGISTRY_ACTION(restart, "restart device");

REGISTRY_BEGIN(ACTION, "actions")
  REGISTRY_ACTION_ITEM(restart)	// 3.0
REGISTRY_END(ACTION)

REGISTRY_BEGIN(ROOT, "root")
  REGISTRY_LIST_ITEM(PIM)	// 0
  REGISTRY_LIST_ITEM(CM)	// 1
  REGISTRY_LIST_ITEM(STATUS)	// 2
  REGISTRY_LIST_ITEM(ACTION)	// 3
REGISTRY_END(ROOT)

Registry reg(&ROOT);

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
  if (list != NULL) {
    Registry::Iterator iter(list);
    Registry::item_P item;
    while ((item = iter.next()) != NULL)
      trace << item << endl;
  }
  return (outs);
}

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaRegistry: started"));
  Watchdog::begin();

  // Initiate EEMEM variables (once)
  int16_t network = 0xC05A;
  uint8_t device = 0x42;
  uint16_t timeout = 2000;
  EEPROM::Device::eeprom.write(&NETWORK, &network, sizeof(network));
  EEPROM::Device::eeprom.write(&DEVICE, &device, sizeof(device));
  EEPROM::Device::eeprom.write(&TIMEOUT, &timeout, sizeof(timeout));
}

void loop()
{
  Registry::item_list_P list;
  Registry::action_P action;
  Registry::item_P item;
  Registry::blob_P blob;
  uint8_t path[2];
  char buf[64];

  // Access reg root item (item list)
  item = reg.lookup(NULL);
  trace << item << endl;
  list = Registry::to_list(item);
  ASSERT(list != NULL);
  trace << list << endl;

  // Access 0 blob product information management
  path[0] = 0;
  item = reg.lookup(path, 1);
  trace << item << endl;
  list = Registry::to_list(item);
  ASSERT(list != NULL);
  trace << list << endl;
  
  // Access 0.0 blob product name
  path[0] = 0;
  path[1] = 0;
  item = reg.lookup(path, sizeof(path));
  trace << item << endl;
  blob = Registry::to_blob(item);
  ASSERT(blob != NULL);
  ASSERT(reg.get_value(blob, buf, sizeof(buf)) == sizeof(PRODUCT));
  ASSERT(reg.set_value(blob, buf, sizeof(buf)) < 0);
  trace << buf << endl;

  // Access 0.1 blob version string
  path[0] = 0;
  path[1] = 1; 
  item = reg.lookup(path, sizeof(path));
  trace << item << endl;
  blob = Registry::to_blob(item);
  ASSERT(blob != NULL);
  ASSERT(reg.get_value(blob, buf, sizeof(buf)) == sizeof(VERSION));
  ASSERT(reg.set_value(blob, buf, sizeof(buf)) < 0);
  trace << buf << endl;

  // Access 1.0 network address
  path[0] = 1;
  path[1] = 0; 
  item = reg.lookup(path, sizeof(path));
  trace << item << endl;
  blob = Registry::to_blob(item);
  ASSERT(blob != NULL);
  uint16_t network;
  ASSERT(reg.get_value(blob, &network, sizeof(network)) == sizeof(NETWORK));
  trace << hex << network << endl;
  network = 0xBEEF;
  ASSERT(reg.set_value(blob, &network, sizeof(network)) == sizeof(NETWORK));
  ASSERT(reg.get_value(blob, &network, sizeof(network)) == sizeof(NETWORK));
  trace << hex << network << endl;

  // Access 2.0 blob battery status (mV)
  path[0] = 2; 
  path[1] = 0; 
  item = reg.lookup(path, sizeof(path));
  trace << item << endl;
  blob = Registry::to_blob(item);
  ASSERT(blob != NULL);
  uint16_t vcc;
  ASSERT(reg.get_value(blob, &vcc, sizeof(vcc)) == sizeof(vcc));
  ASSERT(reg.set_value(blob, buf, sizeof(buf)) < 0);
  trace << PSTR("vcc = ") << vcc << PSTR(" mV") << endl;

  // Access 2.1 blob processor load %
  path[0] = 2; 
  path[1] = 1; 
  item = reg.lookup(path, sizeof(path));
  trace << item << endl;
  blob = Registry::to_blob(item);
  ASSERT(blob != NULL);
  uint8_t load;
  ASSERT(reg.get_value(blob, &load, sizeof(load)) == sizeof(load));
  ASSERT(reg.set_value(blob, buf, sizeof(buf)) < 0);
  trace << PSTR("load = ") << load << PSTR(" %") << endl;

  // Access 2.2 blob errors
  path[0] = 2; 
  path[1] = 2; 
  item = reg.lookup(path, sizeof(path));
  trace << item << endl;
  blob = Registry::to_blob(item);
  ASSERT(blob != NULL);
  uint16_t errs;
  ASSERT(reg.get_value(blob, &errs, sizeof(errs)) == sizeof(errors));
  trace << errs << endl;
  errors += 42;
  ASSERT(reg.get_value(blob, &errs, sizeof(errs)) == sizeof(errors));
  trace << PSTR("errors = ") << errs << endl;
  errs = 0;
  ASSERT(reg.set_value(blob, &errs, sizeof(errs)) == sizeof(errors));
  ASSERT(reg.get_value(blob, &errs, sizeof(errs)) == sizeof(errors));
  trace << PSTR("errors = ") << errs << endl;

  // Access 3.0 action restart
  path[0] = 3; 
  path[1] = 0; 
  trace << reg.apply(path, sizeof(path), (void*) PSTR("rebooting..."), 0) << endl;

  // Access 3.1 illegal access path
  path[0] = 3; 
  path[1] = 1; 
  item = reg.lookup(path, sizeof(path));
  trace << item << endl;
  ASSERT(true == false);
}

