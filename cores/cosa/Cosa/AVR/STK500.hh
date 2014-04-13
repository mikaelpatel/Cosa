/**
 * @file Cosa/AVR/STK500.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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

#ifndef __COSA_AVR_STK500_HH__
#define __COSA_AVR_STK500_HH__

#include "Cosa/AVR/Programmer.hh"
#include "Cosa/IOStream.hh"

/**
 * An implementation of the STK500 Communication Protocol (V1.X) to allow
 * Arduino to act as an ISP. Uses the Cosa Programmer class to perform
 * the actual programming of devices (e.g. ATtiny). The overall structure
 * of the protocol is:
 * 
 * Command: 	Cmnd {param} CRC_EOP
 *
 * Response: 	INSYNC {res} OK
 *		INSYNC {param} FAILED
 *		INSYNC NODEVICE
 *		NOSYNC
 *
 * @References
 * 1. AVR061: STK500 Communication Protocol, Application Note, 
 * Rev. 2525B-AVR-04/03, http://www.atmel.com/Images/doc2525.pdf
 */
class STK500 {
public:
  /** 
   * Construct command parser with given io-stream device and
   * programmer.
   * @param[in] dev iostream device for commands.
   * @param[in] prog programmer.
   */
  STK500(IOStream::Device* dev, Programmer* prog) : 
    m_dev(dev), 
    m_prog(prog),
    m_state(IDLE_STATE),
    m_addr(0)
  {
  }

  /** 
   * Run the command parser. Returns after parsing and executing 
   * a single command  
   */  
  void run();

protected:
  /** Response definitions */
  enum Resp {
    OK = 0x10,			// Sent after a valid command has been executed
    FAILED = 0x11,		// if a command execution fails
    UNKNOWN = 0x12,		// if a command is unknown
    NODEVICE = 0x13,		// if command is ENTER_PROGMODE and
				// SET_DEVICE has not been set
    INSYNC = 0x14,		// Send after CRC_EOP has been received
    NOSYNC = 0x15,		// if CRC_EOP is not received after a command
  } __attribute__((packed));

  /** Synchronization symbol */
  enum Sync { 
    CRC_EOP = 0x20		// End of command
  } __attribute__((packed));
    
  /** Commands */
  enum Cmnd {
    GET_SYNC = 0x30,		// Regain synchronization
    GET_SIGN_ON = 0x31,		// Check if the startkit is present
    SET_PARAMETER = 0x40,	// Set value of a valid parameter (see Parm)
    GET_PARAMETER = 0x41,	// Get value of a valid parameter
    SET_DEVICE = 0x42,		// Set the device programming parameters
    SET_DEVICE_EXT = 0x45,	// Set extended programming parameters
    ENTER_PROGMODE = 0x50,	// Enter programming mode
    LEAVE_PROGMODE = 0x51,	// Leave programming mode
    CHIP_ERASE = 0x52,		// Erase device
    CHECK_AUTOINC = 0x53,	// Check auto-increment of address
    LOAD_ADDRESS = 0x55,	// Load address (eeprom/progmem)
    UNIVERSAL = 0x56,		// Send instruction to device
    UNIVERSAL_MULTI = 0x57,	// Send memory block to device
    PROG_FLASH = 0x60,		// Write word to program memory 
    PROG_DATA = 0x61,		// Write data to eeprom
    PROG_FUSE = 0x62,		// Write fuse bits (low/high)
    PROG_LOCK = 0x63,		// Write lock bits
    PROG_PAGE = 0x64,		// Write block to program memory or eeprom
    PROG_FUSE_EXT = 0x65,	// Write extended fuse (low/high/ext)
    READ_FLASH = 0x70,		// Read word from program memory
    READ_DATA = 0x71,		// Read byte from eeprom
    READ_FUSE = 0x72,		// Read fuse bits (low/high)
    READ_LOCK = 0x73,		// Read lock bits
    READ_PAGE = 0x74,		// Read block from program memory or eeprom
    READ_SIGN = 0x75,		// Read device signature bytes
    READ_OSCCAL = 0x76,		// Read oscilator calibration byte
    READ_FUSE_EXT = 0x77	// Read extended fuse bites (low/high/ext)
  } __attribute__((packed));

  /** Parameter keys for SET/GET_PARAMETER */
  enum Parm {
    HW_VER = 0x80,		// Hardware version, read-only
    SW_MAJOR = 0x81,		// Software version major, read-only
    SW_MINOR = 0x82,		// Software version minor, read-only
    LEDS = 0x83,		// Status LEDs
    VTARGET = 0x84,		// Target voltage X 10.0
    VADJUST = 0x85,		// Adjustable voltage X 10.0
    OSC_PSCALE = 0x86,		// Oscillator Timer Prescale Value
    OSC_CMATCH = 0x87,		// Oscillator Timer Compare Match Value
    RESET_DURATION = 0x88,	// ISP Reset Duration
    SCK_DURATION = 0x89,	// ISP SCK half-period duration
    BUFSIZEL = 0x90,		// Communication buffer size (low)
    BUFSIZEH = 0x91,		// Communication buffer size (high)
    DEVICE = 0x92,		// Device
    PROGMODE = 0x93,		// Programming mode
    PARAMODE = 0x94,		// Parameter mode
    POLLING = 0x95,		// Polling period
    SELFTIMED = 0x96,		// Timing of device
    TOPCARD_DETECT = 0x98	// Detect top-card
  } __attribute__((packed));

  /** Parameter block for SET_DEVICE */
  struct param_t {
    uint8_t devicecode;		// Device code
    uint8_t revision;		// Device revision
    uint8_t progtype;		// Parallel/High voltage(1), Serial(0) mode
    uint8_t parmode;		// Full(1), Pseudo(0) parallel 
    uint8_t polling;		// Polling may be used
    uint8_t selftimed;		// Programming self timed
    uint8_t lockbytes;		// Number of lock bytes
    uint8_t fusebytes;		// Number of fuse bytes
    uint8_t flashpoll;		// Program memory polling value
    uint8_t dummy;		// Dummy
    uint16_t eeprompoll;	// EEPROM memory polling value
    uint16_t pagesize;		// Program memory page size in bytes
    uint16_t eepromsize;	// EEPROM size in bytes
    uint32_t flashsize;		// Program memory size
  }; 

  /** Parameter block for SET_DEVICE_EXT */
  struct extparam_t {
    uint8_t commandsize;	// Number of additional parameters(4)
    uint8_t eeprompagesize;	// EEPROM page size in bytes
    uint8_t signalpagel;	// Port pin for PAGEL signal
    uint8_t singalbs2;		// Port pin for BS2 signal
    uint8_t resetdisable;	// RSTDSBL available
  };

  /** Internal state-machine */
  enum state_t {
    IDLE_STATE,
    READY_STATE,
    PROG_STATE
  };

  /** Serial device access functions */
  uint8_t getchar();
  void putchar(uint8_t c);
  void read(void* buf, size_t count);
  void write(void* buf, size_t count);

  /** Command state checking and response functions */
  bool is_insync();
  void response();
  void response(uint8_t param);
  void response(uint8_t param1, uint8_t param2);
  void response(uint8_t param1, uint8_t param2, uint8_t param3);
  void response(const char* param, size_t count);
  void response_P(const char* param);
  void failed(uint8_t param);
  void illegal(uint8_t resp);

  /** Command handlers; general commands */
  void set_parameter();
  void get_parameter();
  void set_device();
  void set_device_ext();
  void enter_progmode();
  void leave_progmode();

  /** Command handlers; general programmer commands */
  void chip_erase();
  void load_address();
  void universal();
  void universal_multi();

  /** Command handlers; device write commands */
  void prog_flash();
  void prog_data(); 
  void prog_fuse();
  void prog_lock(); 
  void prog_page();
  void prog_fuse_ext();

  /** Command handlers; device read commands */
  void read_flash();
  void read_data();
  void read_fuse();
  void read_lock(); 
  void read_page();
  void read_sign();
  void read_osccal();
  void read_fuse_ext();

private:
  /** IOStream device used for communication */
  IOStream::Device* m_dev;

  /** Programmer for issuing instructions and read/write data */
  Programmer* m_prog;

  /** Current state */
  state_t m_state;

  /** Current address; program memory or eeprom word address */
  uint16_t m_addr;
};

#endif
