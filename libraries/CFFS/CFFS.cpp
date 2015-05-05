/**
 * @file CFFS.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel.
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

#include "CFFS.hh"

Flash::Device* CFFS::device = NULL;
uint32_t CFFS::current_dir_addr = 0L;

int
CFFS::File::open(const char* filename, uint8_t oflag)
{
  // Check that the file is not open and the open mode
  if (is_open()) return (EBUSY);

  // Check if the file should be created
  if (oflag & O_CREAT) {
    oflag |= O_WRITE;
    int res = CFFS::create(filename, FILE_ENTRY_TYPE, oflag, m_entry, m_entry_addr);
    if (res < 0) return (res);
    m_current_addr = m_entry.ref + sizeof(CFFS::descr_t);
    m_current_pos = 0L;
    m_file_size = 0L;
  }

  // Check that the file exists; open file
  else {
    if ((oflag & O_WRITE) == 0) oflag |= O_READ;
    int res = lookup(filename, m_entry, m_entry_addr);
    if (res < 0) return (res);
    res = find_end_of_file(m_entry.ref, m_current_addr, m_file_size);
    if (res < 0) return (res);
    m_current_pos = m_file_size;
  }

  // Check if the position should be from the start of the file
  if (((oflag & O_RDWR) == O_READ) || (oflag & O_CREAT)) {
    m_current_addr = m_entry.ref + sizeof(CFFS::descr_t);
    m_current_pos = 0L;
  }

  // Save flags
  m_flags = oflag;
  return (0);
}

int
CFFS::File::remove()
{
  if (m_flags == 0) return (ENXIO);
  m_flags = 0;
  return (CFFS::remove(m_entry_addr, FILE_ENTRY_TYPE));
}

int
CFFS::File::close()
{
  if (m_flags == 0) return (ENXIO);
  m_flags = 0;
  return (0);
}

int
CFFS::File::seek(uint32_t pos, uint8_t whence)
{
  // Check mode and parameters
  if ((m_flags & O_READ) == 0) return (EPERM);
  if (pos > m_file_size) return (EINVAL);

  // Fix: Should implement all seek variants
  if (whence != SEEK_SET) return (EINVAL);

  // Find sector and position in sector
  descr_t entry;
  uint32_t addr = m_entry.ref;
  m_current_pos = pos;
  while (pos != 0) {
    // Read sector header and check that it is a file block
    if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry))
      return (EIO);
    if (entry.type != FILE_BLOCK_TYPE)
      return (ENXIO);
    // Check if additional sector skip is needed
    uint32_t size = entry.size - sizeof(descr_t);
    if (pos >= size) {
      addr = entry.ref;
      if (addr == NULL_REF)
	return (ENXIO);
      pos -= size;
      // First byte in next sector
      if (pos == 0)
	m_current_addr = addr + sizeof(descr_t);
    }
    else {
      // Byte in current sector
      m_current_addr = addr + sizeof(descr_t) + pos;
      pos = 0;
    }
  }

  // Found the position
  return (0);
}

int
CFFS::File::write(const void *buf, size_t size)
{
  return (write(buf, size, false));
}

int
CFFS::File::write_P(const void* buf, size_t size)
{
  return (write(buf, size, true));
}

int
CFFS::File::getchar()
{
  char c;
  if (read(&c, sizeof(c)) != sizeof(c)) return (IOStream::EOF);
  return (c & 0xff);
}

int
CFFS::File::read(void* buf, size_t size)
{
  // Check file access mode
  if ((m_flags & O_READ) == 0) return (EPERM);

  // Adjust requested size if needed
  uint32_t remains = m_file_size - m_current_pos;
  if (size > remains) size = remains;
  int count = size;

  // Read sectors until buffer is filled
  while (size != 0) {
    int res = CFFS::read(buf, m_current_addr, size);
    if (res < 0) return (EIO);
    size -= res;
    m_current_pos += res;
    m_current_addr += res;

    // Read next sector header if needed
    if ((m_current_addr & device->SECTOR_MASK) == 0) {
      descr_t header;
      uint32_t addr = m_current_addr - device->SECTOR_BYTES;
      if (device->read(&header, addr, sizeof(header)) != sizeof(header))
	return (EIO);
      if (header.type != FILE_BLOCK_TYPE) return (ENXIO);
      if (header.size != device->SECTOR_BYTES) return (ENXIO);
      if (header.ref == NULL_REF) return (ENXIO);
      m_current_addr = header.ref + sizeof(header);
    }
  }

  // Return number of bytes read
  return (count);
}

int
CFFS::File::write(const void* buf, size_t size, bool progmem)
{
  // Check access mode
  if ((m_flags & O_WRITE) == 0) return (EPERM);

  // Check write position; must be end of file
  if (m_current_pos != m_file_size) return (EINVAL);
  int count = size;

  // Write sectors with buffer data
  while (size != 0) {
    int res;
    if (progmem)
      res = CFFS::write_P(m_current_addr, buf, size);
    else
      res = CFFS::write(m_current_addr, buf, size);
    if (res < 0) return (res);
    m_current_addr += res;
    m_current_pos += res;
    m_file_size += res;
    size -= res;

    // Check if sector is exhaused
    if ((m_current_addr & device->SECTOR_MASK) == 0) {
      // Allocate a new sector
      uint32_t sector = next_free_sector();
      if (sector == 0L) return (ENOSPC);

      // Update current sector header
      descr_t header;
      uint32_t addr = m_current_addr - device->SECTOR_BYTES;
      if (device->read(&header, addr, sizeof(header)) != sizeof(header))
	return (EIO);
      if (header.type != FILE_BLOCK_TYPE) return (ENXIO);
      if (header.size != device->SECTOR_BYTES) return (ENXIO);
      if (header.ref != NULL_REF) return (ENXIO);

      // Append new sector
      header.ref = sector;
      if (device->write(addr, &header, sizeof(header)) != sizeof(header))
	return (EIO);

      // Continue write in new sector
      m_current_addr = sector + sizeof(header);
    }
  }
  return (count);
}

bool
CFFS::begin(Flash::Device* flash)
{
  // Check that the file system access is not already initiated
  if (device != NULL) return (false);

  // Check that the device is formatted and contains a file system
  descr_t entry;
  uint32_t addr = 0L;
  if ((flash->read(&entry, addr, sizeof(entry)) != sizeof(entry))
      || (entry.type != CFFS_TYPE))
    return (false);
  addr += sizeof(entry);
  if ((flash->read(&entry, addr, sizeof(entry)) != sizeof(entry))
      || (entry.type != DIR_BLOCK_TYPE)
      || (entry.ref != addr)
      || strcmp_P(entry.name, PSTR("..")))
    return (false);

  // A file system and root directory exists
  device = flash;
  current_dir_addr = addr;
  return (true);
}

int
CFFS::ls(IOStream& outs)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (ENXIO);

  // Read directory header for number of entries
  uint32_t addr = current_dir_addr;
  descr_t entry;
  uint8_t j = 0;
  if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry))
    return (EIO);
  const uint16_t ENTRY_MAX = entry.size / sizeof(entry);

  // Print file names to given output stream
  for (uint16_t i = 0; i < ENTRY_MAX; i++, addr += sizeof(entry)) {
    if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry))
      return (EIO);
    if ((entry.type == DIR_BLOCK_TYPE)
	|| (entry.type == DIR_ENTRY_TYPE)
	|| (entry.type == FILE_ENTRY_TYPE)) {
      outs << entry.name << tab;
      j += 1;
      if ((j & 0x7) == 0) outs << endl;
    }
  }
  if (j & 0x7) outs << endl;

  // Return successful
  return (0);
}

int
CFFS::rm(const char* filename)
{
  File file;
  int res = file.open(filename, O_READ);
  return (res < 0 ? res : file.remove());
}

int
CFFS::cd(const char* filename)
{
  descr_t entry;
  uint32_t addr;
  int res = CFFS::lookup(filename, entry, addr);
  if (res < 0)
    return (res);
  if ((entry.type != DIR_BLOCK_TYPE) && (entry.type != DIR_ENTRY_TYPE))
    return (ENOTDIR);
  current_dir_addr = entry.ref;
  return (0);
}

int
CFFS::mkdir(const char* filename)
{
  descr_t entry;
  uint32_t addr;
  return (CFFS::create(filename, CFFS::DIR_ENTRY_TYPE, O_EXCL, entry, addr));
}

int
CFFS::rmdir(const char* filename)
{
  UNUSED(filename);
  return (ENOSYS);
}

int
CFFS::format(Flash::Device* flash, const char* name)
{
  // Check that the file system driver is initiated
  if (device != NULL) return (EPERM);

  // Check that the drive name is not too long
  if (strlen(name) >= FILENAME_MAX) return (ENAMETOOLONG);

  // Erase sectors
  descr_t header;
  uint32_t addr = 0L;
  const uint8_t SIZE = flash->SECTOR_BYTES / 1024;
  for (uint16_t i = 0; i < flash->SECTOR_MAX; i++) {
    if (flash->read(&header, addr, sizeof(header)) != sizeof(header))
      return (EIO);
    if (header.type != FREE_TYPE) {
      if (flash->erase(addr, SIZE) != 0) return (EIO);
    }
    addr += flash->SECTOR_BYTES;
  }

  // Write file system header with drive name
  addr = 0L;
  memset(&header, 0, sizeof(header));
  header.type = CFFS_TYPE;
  header.size = sizeof(header);
  header.ref = sizeof(header);
  strcpy(header.name, name);
  if (flash->write(addr, &header, sizeof(header)) != sizeof(header))
    return (EIO);

  // Write root directory
  addr += sizeof(header);
  memset(&header, 0, sizeof(header));
  header.type = DIR_BLOCK_TYPE;
  header.size = flash->DEFAULT_SECTOR_BYTES - sizeof(header);
  header.ref = addr;
  strcpy_P(header.name, PSTR(".."));
  if (flash->write(addr, &header, sizeof(header)) != sizeof(header))
    return (EIO);
  return (0);
}

int
CFFS::lookup(const char* filename, descr_t &entry, uint32_t& addr)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (ENXIO);

  // Read current director and lookup entry with given filename
  addr = current_dir_addr;
  if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry))
    return (EIO);
  const uint16_t ENTRY_MAX = entry.size / sizeof(descr_t);
  addr = entry.ref;
  for (uint16_t i = 0; i < ENTRY_MAX; i++, addr += sizeof(descr_t)) {
    if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry))
      return (EIO);
    if (entry.type == FREE_TYPE) break;
    if ((entry.type & ALLOC_MASK) == 0) continue;
    if (strcmp(filename, entry.name)) continue;
    return (0);
  }

  // Not found
  return (ENOENT);
}

int
CFFS::create(const char* filename, uint16_t type, uint8_t flags,
	     descr_t &entry, uint32_t &addr)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (ENXIO);

  // Check parameters
  if ((type != DIR_ENTRY_TYPE) && (type != FILE_ENTRY_TYPE)) return (EINVAL);
  if (strlen(filename) >= FILENAME_MAX) return (ENAMETOOLONG);

  // Search through the current directory
  addr = current_dir_addr;
  if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry))
    return (EIO);
  uint16_t ENTRY_MAX = entry.size / sizeof(descr_t);
  for (uint16_t i = 0; i < ENTRY_MAX; i++, addr += sizeof(descr_t)) {
    if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry))
      return (EIO);
    // Skip deleted entries
    if ((entry.type & ALLOC_MASK) == 0) continue;
    // Check if file name is already used; error or remove
    if (!strcmp(filename, entry.name)) {
      if ((flags & O_EXCL) || (type == DIR_ENTRY_TYPE)) return (EEXIST);
      int res = remove(addr, entry.type);
      if (res < 0) return (res);
    }
    else if (entry.type == FREE_TYPE) {
      // Creating a directory or file entry
      if (type == DIR_ENTRY_TYPE) {
	uint32_t dir = next_free_directory();
	if (dir == 0L) return (ENOSPC);
	entry.ref = dir;
      }
      else {
	uint32_t sector = next_free_sector();
	if (sector == 0L) return (ENOSPC);
	entry.ref = sector;
      }
      strcpy(entry.name, filename);
      entry.type = type;
      entry.size = sizeof(entry);
      // Write the entry and return the address
      if (device->write(addr, &entry, sizeof(entry)) != sizeof(entry))
	return (EIO);
      return (0);
    }
  }

  // Directory is full
  return (ENOSPC);
}

int
CFFS::remove(uint32_t addr, uint16_t type)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (ENXIO);

  // Read directory entry and check type
  descr_t entry;
  if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry))
    return (EIO);
  if ((entry.type != type))
    return (EINVAL);

  // Save reference to sector to erase
  uint32_t ref = entry.ref;

  // Mark the entry as removed in the directory block
  memset(&entry, 0, sizeof(entry));
  if (device->write(addr, &entry, sizeof(entry)) != sizeof(entry))
    return (EIO);

  // Erase sectors
  while (ref != NULL_REF) {
    if (device->read(&entry, ref, sizeof(entry)) != sizeof(entry))
      return (EIO);
    if (device->erase(ref, entry.size / 1024) != 0) return (EIO);
    ref = entry.ref;
  }
  return (0);
}

int
CFFS::read(void* dest, uint32_t src, size_t size)
{
  if (device == NULL) return (ENXIO);
  size_t avail = device->SECTOR_BYTES - (src & device->SECTOR_MASK);
  if (size > avail) size = avail;
  return (device->read(dest, src, size));
}

int
CFFS::write(uint32_t dest, const void* src, size_t size)
{
  if (device == NULL) return (ENXIO);
  size_t avail = device->SECTOR_BYTES - (dest & device->SECTOR_MASK);
  if (size > avail) size = avail;
  return (device->write(dest, src, size));
}

int
CFFS::write_P(uint32_t dest, const void* src, size_t size)
{
  if (device == NULL) return (ENXIO);
  size_t avail = device->SECTOR_BYTES - (dest & device->SECTOR_MASK);
  if (size > avail) size = avail;
  return (device->write_P(dest, src, size));
}

uint32_t
CFFS::next_free_sector()
{
  // Check that the file system driver is initiated
  if (device == NULL) return (0L);

  // Search for a free sector
  descr_t header;
  uint32_t addr = device->SECTOR_BYTES;
  for (uint16_t i = 1; i < device->SECTOR_MAX; i++) {
    if (device->read(&header, addr, sizeof(header)) != sizeof(header))
      return (0L);
    if (header.type != FREE_TYPE) {
      addr += device->SECTOR_BYTES;
      continue;
    }
    // Initiate the sector header
    header.type = FILE_BLOCK_TYPE;
    header.size = device->SECTOR_BYTES;
    header.ref = NULL_REF;
    memset(header.name, 0, sizeof(header.name));
    if (device->write(addr, &header, sizeof(header)) != sizeof(header))
      return (0L);
    // Return address of sector
    return (addr);
  }
  return (0L);
}

uint32_t
CFFS::next_free_directory()
{
  // Check that the file system driver is initiated
  if (device == NULL) return (0L);

  // Search for a free directory
  descr_t header;
  uint32_t addr;
  if (device->SECTOR_BYTES == device->DEFAULT_SECTOR_BYTES) {
    addr = device->SECTOR_BYTES;
    for (uint16_t i = 1; i < device->SECTOR_MAX; i++, addr += device->SECTOR_BYTES) {
      if (device->read(&header, addr, sizeof(header)) != sizeof(header))
	return (0L);
      if (header.type == FREE_TYPE) break;
    }
  }
  else {
    addr = device->DEFAULT_SECTOR_BYTES;
    for (uint16_t i = 1; i < DIR_MAX; i++, addr += device->DEFAULT_SECTOR_BYTES) {
      if (device->read(&header, addr, sizeof(header)) != sizeof(header))
	return (0L);
      if (header.type == FREE_TYPE) break;
    }
  }
  if (header.type != FREE_TYPE) return (0L);

  // Initiate the parent directory reference
  memset(&header, 0, sizeof(header));
  header.type = DIR_BLOCK_TYPE;
  header.size = device->DEFAULT_SECTOR_BYTES;
  header.ref = current_dir_addr;
  strcpy_P(header.name, PSTR(".."));
  if (device->write(addr, &header, sizeof(header)) != sizeof(header))
    return (0L);
  // Return the directory address
  return (addr);
}

int
CFFS::find_end_of_file(uint32_t addr, uint32_t &pos, uint32_t &size)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (ENXIO);

  // Locate last sector
  descr_t header;
  size = 0L;
  while (1) {
    if (device->read(&header, addr, sizeof(header)) != sizeof(header))
      return (EIO);
    if (header.type != FILE_BLOCK_TYPE) return (ENXIO);
    if (header.size != device->SECTOR_BYTES) return (ENXIO);
    if (header.ref == NULL_REF) break;
    addr = header.ref;
    size += (header.size - sizeof(header));
  }

  // Locate end of sector
  uint8_t buf[256];
  addr += device->SECTOR_BYTES;
  const uint16_t BUF_MAX = device->SECTOR_BYTES / sizeof(buf);
  for (uint16_t i = 0; i < BUF_MAX; i++) {
    addr -= sizeof(buf);
    if (device->read(buf, addr, sizeof(buf)) != sizeof(buf))
      return (EIO);
    uint16_t j = sizeof(buf) - 1;
    while (buf[j] == 0xff) if (j == 0) break; else j--;
    if (j == 0) continue;
    addr += j + 1;
    break;
  }

  // And return position and size
  pos = addr;
  size += (addr & device->SECTOR_MASK) - sizeof(header);
  return (0);
}
