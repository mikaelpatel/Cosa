#!/usr/bin/env python

# Generalized Control of Arduino-compatible boards connected via serial or
# native USB
#
# author: jeditekunum
# version: 2016-12-01-1

# Usage:
#
# --port PORT           : PORT is full device path
# --verbose|-v          : verbose output 
# --timeout TIMEOUT     : timeout for device disappear/appear in seconds
# --period PERIOD       : for serial only, seconds to pulse DTR
# --caterina            : device is native USB with caterina bootloader
# --ifcaterina          : nothing is executed if --caterina not specified
# --ifncaterina         : nothing is executed if --caterina is specified
# --gettag              : prints tag of current device
# --disappear [TAG]     : wait for device to disappear, optionally specific TAG
#                         cannot be used with --reset
# --appear              : wait for device to appear
# --reset               : reset device
#                         cannot be used with --disappear
#                         does NOT wait; use --appear if desired
#
# NOTES:
# For reset the desired behavior likely requires --reset and --appear.
#
# --disappear doesn't make a lot of sense with serial devices
# use with --ifcaterina allows calling code to be the same and only
# need to supply --caterina if appropriate

import serial
import os
import sys
import argparse
from time import sleep

tickchar='.'

######################################################################
# Parse command line
######################################################################

parser = argparse.ArgumentParser(
    description = 'Arduino Device Utility'
)

parser.add_argument(
    '--port',
    dest = 'port',
    help = 'serial device e.g. /dev/ttyACM0',
    default = None
)

parser.add_argument(
    '--verbose', '-v',
    dest = 'verbose',
    action = 'store_true',
    help = 'display activity',
    default = False
)

parser.add_argument(
    '--timeout',
    dest = 'timeout',
    type = int,
    help = 'timeout in seconds',
    default = 30
)

parser.add_argument(
    '--period',
    dest = 'period',
    type = float,
    help = 'for serial, DTR pulse width in seconds',
    default = 0.1
)

parser.add_argument(
    '--caterina',
    dest = 'caterina',
    action = 'store_true',
    help = 'device has native USB',
    default = False
)

parser.add_argument(
    '--ifcaterina',
    dest = 'ifcaterina',
    action = 'store_true',
    help = 'operations performed only if --caterina supplied',
    default = False
)

parser.add_argument(
    '--ifncaterina',
    dest = 'ifncaterina',
    action = 'store_true',
    help = 'operations performed only if --caterina not supplied',
    default = False
)

parser.add_argument(
    '--gettag',
    dest = 'gettag',
    action = 'store_true',
    help = 'get current tag of device',
    default = False
)

parser.add_argument(
    '--disappear',
    dest = 'disappear',
    metavar = 'TAG',
    nargs = '?',
    type = int,
    help = 'wait for device to disappear, optionally device TAG',
    default = -1,
    const = 0
)

parser.add_argument(
    '--appear',
    dest = 'appear',
    action = 'store_true',
    help = 'wait for device to appear',
    default = False
)

parser.add_argument(
    '--reset',
    dest = 'reset',
    action = 'store_true',
    help = 'reset device',
    default = False
)

args = parser.parse_args()

######################################################################
# Validate command line
######################################################################

if args.ifcaterina and args.ifncaterina:
    parser.error('cannot supply both --ifcaterina and --ifncaterina')

if args.reset and args.disappear >= 0:
    parser.error('cannot supply --disappear with --reset')

######################################################################
# Make sure caterina matches what is requested
######################################################################

if (args.caterina and args.ifncaterina) or (not args.caterina and args.ifcaterina):
    sys.exit(0)

######################################################################
# --gettag
######################################################################

if args.gettag:
    if os.path.exists(args.port):
        sys.stdout.write('%d\n' % os.stat(args.port).st_ino)
    else:
        sys.stdout.write('0\n')

######################################################################
# wait_for_disappear
######################################################################

def wait_for_disappear(tag):
    # if tag > 0 then only waits for specific tag to disappear
    # if tag <= 0 then waits for current device to disappear
    if os.path.exists(args.port):
        if tag > 0 and os.stat(args.port).st_ino != tag:
            return # already disappeared and reappeared
        if tag <= 0:
            tag = os.stat(args.port).st_ino
        countdown = args.timeout
        while 1:
            if countdown == 0:
                if args.verbose: sys.stdout.write('%s (%d) didn\'t disappear before timeout\n' % (args.port, tag))
                sys.exit(1)
            if not os.path.exists(args.port) or os.stat(args.port).st_ino != tag:
                if args.verbose: sys.stdout.write('%s (%d) disappeared\n' % (args.port, tag))
                break
            if countdown == args.timeout:
                if args.verbose: sys.stdout.write('Waiting for %s (%d) to disappear\n' % (args.port, tag))
            if args.verbose:
                for tick in range(0, countdown):
                    sys.stdout.write(tickchar)
                    sys.stdout.flush()
                sys.stdout.write('\n')
            countdown = countdown - 1
            sleep(1)

######################################################################
# wait_for_appear
######################################################################

def wait_for_appear():
    countdown = args.timeout
    while 1:
        if countdown == 0:
            if args.verbose: sys.stdout.write('%s didn\'t appear before timeout\n' % args.port)
            sys.exit(1)
        if os.path.exists(args.port):
            if countdown < args.timeout:
                if args.verbose: sys.stdout.write('%s (%d) appeared\n' % (args.port, os.stat(args.port).st_ino))
            break
        if countdown == args.timeout:
            if args.verbose: sys.stdout.write('Waiting for %s to appear\n' % args.port)
        if args.verbose:
            for tick in range(0, countdown):
                sys.stdout.write(tickchar)
                sys.stdout.flush()
            sys.stdout.write('\n')
        countdown = countdown - 1
        sleep(1)

######################################################################
# --disappear
######################################################################

if args.disappear >= 0:
    wait_for_disappear(args.disappear)

######################################################################
# --reset
######################################################################

# See discussion at the end regarding serial reset method

if args.reset:
    wait_for_appear()
    sleep(0.25)
    if args.verbose: sys.stdout.write('Resetting %s ' % args.port)
    if not args.caterina:
        if args.verbose: sys.stdout.write('by setting DTR low for %ss\n' % args.period)
        # DTR (and RTS) pin is False/high when closed
        # DTR (and RTS) pin is True/low when opened
        ser = serial.Serial(args.port, 115200)
        # !RTS triggers the enhanced reset discussed below when used with the hardware
        # assist logic.  Normal implementations do not do anything with the RTS signal
        # so this is a no-op for them.
        ser.setRTS(False)
        # Sleep long enough to trigger desired behavior
        sleep(args.period)
        # DTR pin returns to False/high when closed
        ser.close()
    else:
        if args.verbose: sys.stdout.write('by setting 1200bps for 0.25s\n')
        ser = serial.Serial(args.port, 57600)
        ser.close()
        ser.open()
        ser.close()
        previous_inode = os.stat(args.port).st_ino
        try:
            ser.setBaudrate(1200)
        except AttributeError:
            ser.baudrate = 1200
        ser.open()
        ser.setDTR(False)
        sleep(0.25)
        ser.close()
        wait_for_disappear(previous_inode)

######################################################################
# --appear
######################################################################

if args.appear:
    wait_for_appear()

######################################################################
# end
######################################################################


# Special handling of serial method (see POSIX specs):
#
# - When a serial port is closed, neither RTS or DTR are asserted. Since the
#   hardware pin is inverted, this means both RTS and DTR pins are HIGH.
# - When a serial port is open, both RTS and DTR are asserted and the hardware
#   pins are LOW.
#
# Simple off-the-shelf USB to Serial converters need nothing more than
#   open; sleep for period; close
# to have the DTR pin go from HIGH->LOW->HIGH. Unfortunately this also means
# that the device will reset every time the port is opened.
#
# A more advanced solution implemented with hardware assist will not
# reset the device every time the port is opened. It will only reset
# the device if the port is open and RTS is toggled. The circuit to
# accomplish this is roughly (74xx132 is good NAND):
#
#       ___
#       DTR >---NOT----|
#       ___            NAND---> RESET/"DTR" to device
#       RTS >----------|
#
# Logic is:
#                                 _____
# action    | RTS/pin | DTR/pin | RESET
#-----------|---------|---------|--------
# closed    |   F/H   |   F/H   |   H
# opened    |   T/L   |   T/L   |   H
# (unused)  |   T/L   |   F/H   |   H
# open+!RTS |   F/H   |   T/L   |   L
#
# The code above supports both simple or advanced method. The pins
# will behave as follows:
#
#                         open !RTS       close
#                            v v          v
#                            v v          v
#
# ___         ---------------+ +----------~---------------
# RTS pin:                   | |
#                            +-+
#
#
# ___         ---------------+            +---------------
# DTR pin:                   |   period+  |
#                            +------------+
#
#
# _____       ---------------~-+          +---------------
# RESET pin:                   |  period  |
#                              +----------+
#
#                            ^ ^          ^
#                            ^ ^          ^
#                         open !RTS       close
#
# Note that this strange behavior is a workaround for device drivers
# that are not capable of leaving the DTR (or RTS) signals alone during
# open/close operations.
