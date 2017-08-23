# @file Cosa.mk
# @version 1.0
#
# @section License
# Copyright (C) 2014-2017, Mikael Patel
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# @section Description
# Arduino-Makefile configuration for Cosa.

ifdef GCC_AVR_VER
  ARDUINO_VER = $(GCC_AVR_VER)
else
  ifndef ARDUINO_VER
    ARDUINO_VER = 1.8.4
  endif
  ifndef ARDUINO_DIR
    ARDUINO_DIR = $(HOME)/opt/arduino-$(ARDUINO_VER)
  endif
endif

ARDMK_DIR = $(COSA_DIR)/build/Arduino-Makefile
ARDUINO_CORE_PATH = $(COSA_DIR)/cores/cosa
ARDUINO_VAR_PATH = $(COSA_DIR)/variants
ARDUINO_LIB_PATH = $(COSA_DIR)/libraries
BOARDS_TXT = $(COSA_DIR)/build/boards.txt

MONITOR_CMD = $(COSA_DIR)/build/miniterm.py -q --lf

include $(ARDMK_DIR)/Arduino.mk
