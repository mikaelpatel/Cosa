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
 * Path Description
 *   0 product information management
 *   0.0 product name
 *   0.1 version string
 *   1 configuration
 *   1.0 network address (16b)
 *   1.1 device address (8b)
 *   1.2 sleep period (ms)
 *   2 status
 *   2.0 battery satus (mV)
 *   2.1 processor load (%)
 *   2.2 error count
 *   3 actions
 *   3.0 restart device
 *   3.1 broadcast sensor data
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Registry.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"

// Product information management registry
REGISTRY_BLOB_PSTR(PRODUCT, "product name", "registry-demo")
REGISTRY_BLOB_PSTR(VERSION, "version string", "1.0")

REGISTRY_BEGIN(PIM, "product information management") // 0
  REGISTRY_BLOB_ITEM(PRODUCT)		// 0.0
  REGISTRY_BLOB_ITEM(VERSION)		// 0.1
REGISTRY_END(PIM)

// Configuration variables and registry
static int16_t NETWORK EEMEM = 0xc05a;
static uint8_t DEVICE EEMEM = 0x42;
static uint16_t TIMEOUT EEMEM = 2000;

REGISTRY_BLOB(NETWORK, "network address (16b)", EEMEM, false)
REGISTRY_BLOB(DEVICE, "device address (8b)", EEMEM, false)
REGISTRY_BLOB(TIMEOUT, "sleep period (ms)", EEMEM, false)

REGISTRY_BEGIN(CM, "configuration") 	// 1
  REGISTRY_BLOB_ITEM(NETWORK)		// 1.0
  REGISTRY_BLOB_ITEM(DEVICE)		// 1.1
  REGISTRY_BLOB_ITEM(TIMEOUT)		// 1.2
REGISTRY_END(CM)

// Status variables and registry
REGISTRY_BLOB_VAR(uint16_t, vcc, "battery status (mV)", 4943, true)
REGISTRY_BLOB_VAR(uint8_t, load, "processor load (%)", 4, true)
REGISTRY_BLOB_VAR(uint16_t, errors, "error count", 0, false)

REGISTRY_BEGIN(STATUS, "status") 	// 2
  REGISTRY_BLOB_ITEM(vcc)		// 2.0
  REGISTRY_BLOB_ITEM(load)		// 2.1
  REGISTRY_BLOB_ITEM(errors)		// 2.2
REGISTRY_END(STATUS)

// Action class and registry
class Restart : public Registry::Action {
public:
  virtual int run(void* buf, size_t size) 
  {
    trace << (const char*) buf << endl;
    return (strlen_P((const char*) buf));
  }
};
Restart do_restart;
REGISTRY_ACTION(do_restart, "restart device");
REGISTRY_BLOB_VAR(bool, do_broadcast, "broadcast sensor data", false, false)

REGISTRY_BEGIN(ACTION, "actions")  	// 3
  REGISTRY_ACTION_ITEM(do_restart)	// 3.0
  REGISTRY_BLOB_ITEM(do_broadcast)	// 3.1
REGISTRY_END(ACTION)

// Root registry
REGISTRY_BEGIN(ROOT, "root")		// -
  REGISTRY_LIST_ITEM(PIM)		// 0
  REGISTRY_LIST_ITEM(CM)		// 1
  REGISTRY_LIST_ITEM(STATUS)		// 2
  REGISTRY_LIST_ITEM(ACTION)		// 3
REGISTRY_END(ROOT)

// Application registry (1218 bytes)
Registry reg(&ROOT);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaRegistry: started"));
  Watchdog::begin();
  
  // Initiate EEMEM variables. Arduino build does not handle .eeprom section
  int16_t network = 0xc05a;
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
  network = 0xbeef;
  ASSERT(reg.set_value(blob, &network, sizeof(network)) == sizeof(NETWORK));
  network = 0;
  ASSERT(reg.get_value(blob, &network, sizeof(network)) == sizeof(NETWORK));
  ASSERT(network == 0xbeef);
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

  // Access 3.1 action broadcast sensor data
  path[0] = 3; 
  path[1] = 1; 
  bool flag;
  ASSERT(reg.get_value(blob, &flag, sizeof(flag)) == sizeof(flag));
  trace << PSTR("flag = ") << flag << endl;
  flag = true;
  ASSERT(reg.set_value(blob, &flag, sizeof(flag)) == sizeof(flag));

  // Access 3.2 illegal path
  path[0] = 3; 
  path[1] = 2; 
  item = reg.lookup(path, sizeof(path));
  trace << item << endl;
  ASSERT(item != NULL);
}

