/**
 * @file DS2482.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2016, Mikael Patel
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

#include "DS2482.hh"

bool
DS2482::device_reset()
{
  status_t status;
  uint8_t cmd;
  int count;

  // Issue device reset command
  cmd = DEVICE_RESET;
  twi.acquire(this);
  count = twi.write(cmd);
  if (count != sizeof(cmd)) goto error;

  // Check status register for device reset
  count = twi.read(&status, sizeof(status));
  twi.release();
  return (count == sizeof(status) && status.RST);

 error:
  twi.release();
  return (false);
}

bool
DS2482::device_config(bool apu, bool spu, bool iws)
{
  config_t config;
  status_t status;
  uint8_t cmd[2];
  int count;

  // Set configuration bit-fields
  config.APU = apu;
  config.SPU = spu;
  config.IWS = iws;
  config.COMP = ~config;

  // Issue write configuration command with given setting
  cmd[0] = WRITE_CONGIFURATION;
  cmd[1] = config;
  twi.acquire(this);
  count = twi.write(cmd, sizeof(cmd));
  if (count != sizeof(cmd)) goto error;

  // Read status and check configuration
  count = twi.read(&status, sizeof(status));
  twi.release();
  return (count == sizeof(status) && !status.RST);

 error:
  twi.release();
  return (false);
}

int
DS2482::set_read_pointer(Register addr)
{
  uint8_t cmd[2];
  uint8_t reg;
  int count;

  // Issue set read pointer command with given pointer
  cmd[0] = SET_READ_POINTER;
  cmd[1] = (uint8_t) addr;
  twi.acquire(this);
  count = twi.write(cmd, sizeof(cmd));
  if (count != sizeof(cmd)) goto error;

  // Read register value
  count = twi.read(&reg, sizeof(reg));
  twi.release();
  return ((count == sizeof(reg)) ? reg : -1);

 error:
  twi.release();
  return (false);
}

bool
DS2482::channel_select(uint8_t chan)
{
  uint8_t cmd[2];
  int count;

  // Check channel number
  if (chan > 7) return (false);

  // Issue channel select command with channel code
  cmd[0] = CHANNEL_SELECT;
  cmd[1] = (~chan << 4) | chan;
  twi.acquire(this);
  count = twi.write(cmd, sizeof(cmd));
  twi.release();
  return (count == sizeof(cmd));
}

bool
DS2482::one_wire_reset()
{
  status_t status;
  uint8_t cmd;
  int count;

  // Issue one wire reset command
  cmd = ONE_WIRE_RESET;
  twi.acquire(this);
  count = twi.write(cmd);
  if (count != sizeof(cmd)) goto error;

  // Wait for one wire operation to complete
  for (int i = 0; i < POLL_MAX; i++) {
    count = twi.read(&status, sizeof(status));
    if (count == sizeof(status) && !status.IWB) break;
  }
  twi.release();
  return (count == sizeof(status) && status.PPD);

  error:
  twi.release();
  return (false);
}

int
DS2482::one_wire_read_bit()
{
  status_t status;
  uint8_t cmd[2];
  int count;

  // Issue one wire single bit command with read data time slot
  cmd[0] = ONE_WIRE_SINGLE_BIT;
  cmd[1] = 0x80;
  twi.acquire(this);
  count = twi.write(cmd, sizeof(cmd));
  if (count != sizeof(cmd)) goto error;

  // Wait for one wire operation to complete
  for (int i = 0; i < POLL_MAX; i++) {
    count = twi.read(&status, sizeof(status));
    if (count == sizeof(status) && !status.IWB) break;
  }
  twi.release();
  if (count != sizeof(status) || status.IWB) return (-1);
  return (status.SBR);

 error:
  twi.release();
  return (-1);
}

bool
DS2482::one_wire_write_bit(bool value)
{
  status_t status;
  uint8_t cmd[2];
  int count;

  // Issue one wire single bit command with given data
  cmd[0] = ONE_WIRE_SINGLE_BIT;
  cmd[1] = (value ? 0x80 : 0x00);
  twi.acquire(this);
  count = twi.write(cmd, sizeof(cmd));
  if (count != sizeof(cmd)) goto error;

  // Wait for one wire operation to complete
  for (int i = 0; i < POLL_MAX; i++) {
    count = twi.read(&status, sizeof(status));
    if (count == sizeof(status) && !status.IWB) break;
  }
  twi.release();
  return (count == sizeof(status) && !status.IWB);

 error:
  twi.release();
  return (false);
}

bool
DS2482::one_wire_write_byte(uint8_t value)
{
  status_t status;
  uint8_t cmd[2];
  int count;

  // Issue one wire write byte command with given data
  cmd[0] = ONE_WIRE_WRITE_BYTE;
  cmd[1] = value;
  twi.acquire(this);
  count = twi.write(cmd, sizeof(cmd));
  if (count != sizeof(cmd)) goto error;

  // Wait for one wire operation to complete
  for (int i = 0; i < POLL_MAX; i++) {
    count = twi.read(&status, sizeof(status));
    if (count == sizeof(status) && !status.IWB) break;
  }
  twi.release();
  return (count == sizeof(status) && !status.IWB);

 error:
  twi.release();
  return (false);
}

int
DS2482::one_wire_read_byte()
{
  status_t status;
  uint8_t cmd;
  int count;

  // Issue one wire read byte command
  cmd = ONE_WIRE_READ_BYTE;
  twi.acquire(this);
  count = twi.write(cmd);
  if (count != sizeof(cmd)) goto error;

  // Wait for one wire operation to complete
  for (int i = 0; i < POLL_MAX; i++) {
    count = twi.read(&status, sizeof(status));
    if (count == sizeof(status) && !status.IWB) break;
  }
  twi.release();
  if (count != sizeof(status) || status.IWB) return (false);

  // Read data register value
  return (set_read_pointer(READ_DATA_REGISTER));

 error:
  twi.release();
  return (false);
}

int
DS2482::one_wire_triplet(bool direction)
{
  status_t status;
  uint8_t cmd[2];
  int count;

  // Issue one wire single bit command with given data
  cmd[0] = ONE_WIRE_TRIPLET;
  cmd[1] = (direction ? 0x80 : 0x00);
  twi.acquire(this);
  count = twi.write(cmd, sizeof(cmd));
  if (count != sizeof(cmd)) goto error;

  // Wait for one wire operation to complete
  for (int i = 0; i < POLL_MAX; i++) {
    count = twi.read(&status, sizeof(status));
    if (count == sizeof(status) && !status.IWB) break;
  }
  twi.release();
  if (count != sizeof(status) && status.IWB) return (-1);

  // Return (DIR, NID, ID)
  return (status >> 5);

 error:
  twi.release();
  return (-1);
}

