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

REGISTRY_BLOB(PRODUCT, "product name", PROGMEM)
REGISTRY_BLOB(VERSION, "version string", PROGMEM)

REGISTRY_BEGIN(PIM, "product information management")
REGISTRY_BLOB_ITEM(PRODUCT)	// 0.0
REGISTRY_BLOB_ITEM(VERSION)	// 0.1
REGISTRY_END(PIM)

static int16_t NETWORK EEMEM = 0xC05A;
static uint8_t DEVICE EEMEM = 0x42;
static uint16_t TIMEOUT = 2000;

REGISTRY_BLOB(NETWORK, "network address (16b)", EEMEM)
REGISTRY_BLOB(DEVICE, "device address (8b)", EEMEM)
REGISTRY_BLOB(TIMEOUT, "sleep period (ms)", EEMEM)

REGISTRY_BEGIN(CM, "configuration")
REGISTRY_BLOB_ITEM(NETWORK)	// 1.0
REGISTRY_BLOB_ITEM(DEVICE)	// 1.1
REGISTRY_BLOB_ITEM(TIMEOUT)	// 1.2
REGISTRY_END(CM)

static uint16_t vcc = 4943;
static uint8_t load = 4;

REGISTRY_BLOB(vcc, "battery status (mV)", SRAM)
REGISTRY_BLOB(load, "processor load (%)", SRAM)

REGISTRY_BEGIN(STATUS, "status")
REGISTRY_BLOB_ITEM(vcc)		// 2.0
REGISTRY_BLOB_ITEM(load)	// 2.1
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
REGISTRY_LIST_ITEM(PIM)		// 0
REGISTRY_LIST_ITEM(CM)		// 1
REGISTRY_LIST_ITEM(STATUS)	// 2
REGISTRY_LIST_ITEM(ACTION)	// 3
REGISTRY_END(ROOT)

Registry root(&ROOT);

IOStream& operator<<(IOStream& outs, Registry::item_P item)
{
  outs << PSTR("item@") << (void*) item;
  if (item != NULL)
    outs << PSTR("(type = ") << Registry::get_type(item)
	 << PSTR(", name = ") << Registry::get_name(item)
	 << PSTR(", storage = ") << Registry::get_storage(item)
	 << PSTR(")");
  return (outs);
}

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaRegistry: started"));
  Watchdog::begin();
}

void loop()
{
  uint8_t path[] = { 0, 0 };
  Registry::action_P action;
  Registry::item_P item;
  Registry::blob_P blob;
  char buf[64];
  int size;

  // Access root
  item = root.lookup(NULL);
  trace << item << endl;

  // Access 0 blob product information management
  item = root.lookup(path, 1);
  trace << item << endl;
  
  // Access 0.0 blob product name
  item = root.lookup(path, sizeof(path));
  trace << item << endl;
  blob = Registry::to_blob(item);
  ASSERT(blob != NULL);
  ASSERT(Registry::get_value(blob, buf, sizeof(buf)) == sizeof(PRODUCT));
  trace << buf << endl;

  // Access 0.1 blob version string
  path[1] = 1; 
  item = root.lookup(path, sizeof(path));
  trace << item << endl;
  blob = Registry::to_blob(item);
  ASSERT(blob != NULL);
  ASSERT(Registry::get_value(blob, buf, sizeof(buf)) == sizeof(VERSION));
  trace << buf << endl;

  // Access 2.1 blob processor load %
  path[0] = 2; 
  item = root.lookup(path, sizeof(path));
  trace << item << endl;
  blob = Registry::to_blob(item);
  ASSERT(blob != NULL);
  uint8_t load;
  ASSERT(Registry::get_value(blob, &load, sizeof(load)) == sizeof(load));
  trace << load << PSTR(" %") << endl;

  // Access 2.0 blob battery status (mV)
  path[1] = 0; 
  item = root.lookup(path, sizeof(path));
  trace << item << endl;
  blob = Registry::to_blob(item);
  ASSERT(blob != NULL);
  uint16_t vcc;
  ASSERT(Registry::get_value(blob, &vcc, sizeof(vcc)) == sizeof(vcc));
  trace << vcc << PSTR(" mV") << endl;

  // Access 3.0 action restart
  path[0] = 3; 
  item = root.lookup(path, sizeof(path));
  trace << item << endl;
  action = Registry::to_action(item);
  ASSERT(action != NULL);
  trace << Registry::run(action, (void*) PSTR("rebooting..."), 0) << endl;
  ASSERT(true == false);
}

