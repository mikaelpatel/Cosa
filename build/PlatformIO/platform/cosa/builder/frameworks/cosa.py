"""
PlatformIO Framework for Cosa
-----------------------------
This PlaformIO framework for Cosa is developed and maintained by Sinisha Djukic as part of the Meshwork project.

NOTE: This framework provides UNOFFICIAL support for Cosa and is not in any way associated with the Cosa project or its author Mikael Patel. In case of questions, ideas, feature and change requests please visit:
https://github.com/SinishaDjukic/Meshwork

Cosa
----
Cosa is an object-oriented platform for Arduino. It replaces the Arduino and Wiring library with a large set of integrated classes that support the full range of AVR/ATmega/ATtiny internal hardware modules; all pin modes, Digital, and Analog Pins, External and Pin Change Interrupts, Analog Comparator, PWM, Watchdog, Timer0/Timer2 (RTT), Timer1 (Servo/Tone/VWI), Input Capture, UART, USI, SPI, TWI and EEPROM. Cosa supports several programming paradigms including Multi-Tasking, Event Driven Programming and UML Capsules/Actors. Cosa contains over 200 classes and nearly as many example sketches to get started.

For more information:
https://github.com/mikaelpatel/Cosa/

"""

import os
import string

from os import listdir, walk
from os.path import isdir, isfile, join

from SCons.Script import DefaultEnvironment

from platformio import util

env = DefaultEnvironment()
platform = env.PioPlatform()

BOARD_OPTS = env.BoardConfig()
BOARD_BUILDOPTS = BOARD_OPTS.get("build", {})
BOARD_CORELIBDIRNAME = "cosa"
BOARD_VARIANTLIBDIRNAME = string.replace(str(BOARD_BUILDOPTS.get("variant")), "/", os.sep)

# IMPORTANT: PlatformIO packages dir is expected to have the following folder:
# - framework-cosa (symlink to <Cosa project root>)
FRAMEWORK_DIR = platform.get_package_dir("framework-cosa")
TOOLCHAIN_DIR = platform.get_package_dir("toolchain-atmelavr")

PLATFORMFW_DIR = FRAMEWORK_DIR

lib_dirs = env.get("LIBSOURCE_DIRS")

project_lib_dir = util.get_projectlib_dir()
for _, subdirs, _ in walk(project_lib_dir):
#  print "Adding project libraries:"
  for dir in subdirs:
    lib_dirs.append(join(project_lib_dir, dir))
#    print join(project_lib_dir, dir)
  break

# Cosa
PLATFORMFW_LIBRARIES_DIR = join(PLATFORMFW_DIR, "libraries")
lib_dirs.append(PLATFORMFW_LIBRARIES_DIR)
lib_dirs.append(join(PLATFORMFW_DIR, "cores", BOARD_CORELIBDIRNAME))
lib_dirs.append(join(PLATFORMFW_DIR, "variants", BOARD_VARIANTLIBDIRNAME))

for _, subdirs, _ in walk(PLATFORMFW_LIBRARIES_DIR):
#  print "Adding Cosa libraries:"
  for dir in subdirs:
    lib_dirs.append(join(PLATFORMFW_LIBRARIES_DIR, dir))
#    print join(PLATFORMFW_LIBRARIES_DIR, dir)
  break

# AVR
lib_dirs.append(join(TOOLCHAIN_DIR, "avr", "include"))

env.Replace(PLATFORMFW_DIR=PLATFORMFW_DIR)
env.Replace(LIBSOURCE_DIRS=lib_dirs)

print "LIBSOURCE_DIRS"
print lib_dirs


#
# Base
#

#ARDUINO_VERSION = int(
#    open(join(env.subst("$PLATFORMFW_DIR"),
#              "platform.txt")).read().replace(".", "").strip())
#TODO: Temporarily hardcoded
ARDUINO_VERSION = 1610

# usb flags
ARDUINO_USBDEFINES = []
if "usb_product" in BOARD_BUILDOPTS:
    ARDUINO_USBDEFINES = [
        "USB_VID=${BOARD_OPTIONS['build']['vid']}",
        "USB_PID=${BOARD_OPTIONS['build']['pid']}",
        "USB_MANUFACTURER=${BOARD_OPTIONS['build']['manufacturer']}",
        'USB_PRODUCT=\\"%s\\"' % (env.subst(
            "${BOARD_OPTIONS['build']['usb_product']}").replace('"', ""))
    ]

ARDUINO_USBDEFINES += ["ARDUINO=%d" % ARDUINO_VERSION]

#
# Compiler flags
#

env.Append(
    CPPDEFINES=ARDUINO_USBDEFINES
)

env.Replace(
	CFLAGS=[
  #"-Wall", "-Wextra", 
		"-std=gnu11", "-c", "-g", "-Os", "-ffunction-sections", "-fdata-sections", "-MMD", "-flto"
	]
)

env.Replace(
	CCFLAGS=[
  #"-Wall", "-Wextra", 
#		"-c", "-g", "-Os", "-ffunction-sections", "-fdata-sections", "-MMD", "-flto"

        "-g",  # include debugging info (so errors include line numbers)
        "-Os",  # optimize for size
        #"-Wall",  # show warnings
        "-ffunction-sections",  # place each function in its own section
        "-fdata-sections",
        "-mmcu=$BOARD_MCU"
	]
)

#compiler.c.flags=-c -g -Os -Wall -ffunction-sections -fdata-sections -MMD -mmcu={build.mcu} -DF_CPU={build.f_cpu} -DARDUINO={runtime.ide.version}
#compiler.cpp.flags=-c -g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -MMD -mmcu={build.mcu} -DF_CPU={build.f_cpu} -DARDUINO={runtime.ide.version}

env.Replace(
	CPPFLAGS=[
  #"-Wall", "-Wextra", 
        "-c", "-g", "-Os", "-fno-exceptions", "-ffunction-sections", "-fdata-sections", "-MMD", "-Woverloaded-virtual", "-flto", "-std=gnu++11", "-felide-constructors", "-fno-implement-inlines", "-fno-rtti", "-fno-threadsafe-statics", "-mcall-prologues"
	]
)

#
# Target: Build Core Library
#

libs = []

env.Append(
	CPPPATH=[lib_dirs]
)
libs.append(env.BuildLibrary(
    join("$BUILD_DIR", "FrameworkArduinoVariant"),
    join(PLATFORMFW_DIR, "variants", BOARD_VARIANTLIBDIRNAME)
    )
)

envsafe = env.Clone()
libs.append(envsafe.BuildLibrary(
    join("$BUILD_DIR", "FrameworkArduino"),
    join("$PLATFORMFW_DIR", "cores", BOARD_CORELIBDIRNAME)
	)
)

env.Append(LIBS=libs)

