/**
 * @file Cosa/FS/CFFS.cpp
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

#include "Cosa/FS/CFFS.hh"

Flash::Device* CFFS::device = NULL;
uint32_t CFFS::current_dir_addr = 0L;

int 
CFFS::File::open(const char* filename, uint8_t oflag)
{
  // Check that the file is not open and the open mode
  if (is_open()) return (-1);

  // Check if the file should be created
  uint32_t addr;
  if (oflag & O_CREAT) {
    oflag |= O_WRITE;
    addr = CFFS::create(filename, CFFS::FILE_ENTRY_TYPE, oflag, m_entry);
    if (addr == 0L) return (-1);
    m_entry_addr = addr;
    m_file_size = 0L;
    m_current_addr = m_entry.ref;
    m_current_addr += sizeof(CFFS::descr_t);
    m_current_pos = 0L;
  } 

  // Check that the file exists; open file 
  else {
    if ((oflag & O_WRITE) == 0) oflag |= O_READ;
    addr = CFFS::lookup(filename, m_entry);
    if (addr == 0L) return (-2);
    m_entry_addr = addr;
    int res = lookup_end_of_file(m_entry.ref, m_current_addr, m_file_size);
    if (res < 0) return (res);
    m_current_pos = m_file_size;
  }

  // Check if the position should be from the start of the file
  if (((oflag & O_RDWR) == O_READ) || (oflag & O_CREAT)) {
    m_current_addr = m_entry.ref;
    m_current_addr += sizeof(CFFS::descr_t);
    m_current_pos = 0L;
  }

  // Save flags
  m_flags = oflag;
  return (0);
}

int 
CFFS::File::remove()
{
  if (m_flags == 0) return (-1);
  return (CFFS::remove(m_entry_addr));
}

int 
CFFS::File::close()
{
  if (m_flags == 0) return (-1);
  m_flags = 0;
  return (0);
}

int 
CFFS::File::seek(uint32_t pos, uint8_t whence)
{
  if ((m_flags & O_READ) == 0) return (-1);
  // Fix: Should implement all seek variants
  if (whence != SEEK_SET) return (-1);
  // Fix: Should allow multiple sectors
  m_current_addr = m_entry.ref;
  m_current_addr += sizeof(CFFS::descr_t) + pos;
  m_current_pos = pos;
  return (0);
}

int
CFFS::File::write(const void *buf, size_t size)
{
  if ((m_flags & O_WRITE) == 0) return (-1);
  if (m_current_pos != m_file_size) return (-1);
  // Fix: Check if new sector is required
  int res = CFFS::write(m_current_addr, buf, size);
  if (res < 0) return (res);
  m_current_addr += res;
  m_current_pos += res;
  m_file_size += res;
  return (res);
}

int 
CFFS::File::write_P(const void* buf, size_t size)
{
  if ((m_flags & O_WRITE) == 0) return (-1);
  if (m_current_pos != m_file_size) return (-1);
  // Fix: Check if new sector is required
  int res = CFFS::write_P(m_current_addr, buf, size);
  if (res < 0) return (res);
  m_current_addr += res;
  m_current_pos += res;
  m_file_size += res;
  return (res);
}

int 
CFFS::File::getchar()
{
  char c;
  if (read(&c, sizeof(c)) != sizeof(c)) return (-1);
  return (c & 0xff);
}

int
CFFS::File::read(void* buf, size_t size)
{
  if ((m_flags & O_READ) == 0) return (-1);
  uint32_t remains = m_file_size - m_current_pos;
  // Fix: Check if new sector is required
  if (size > remains) size = remains;
  if (size == 0) return (0);
  int res = CFFS::read(buf, m_current_addr, size);
  if (res < 0) return (res);
  m_current_addr += res;
  m_current_pos += res;
  return (res);
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
      || strcmp_P(entry.name, PSTR("..")))
    return (false);

  // A file system and root directory exists
  device = flash;
  current_dir_addr = addr;
  return (true);
}

#define CFFS_DEBUG

#if defined(CFFS_DEBUG)
int
CFFS::ls(IOStream& outs)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (-1);
  uint32_t addr = current_dir_addr;
  descr_t entry;
  if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry)) 
    return (-2);
  const uint16_t ENTRY_MAX = entry.size / sizeof(entry);
  outs << PSTR("current_dir_addr = ") << hex << addr 
       << '[' << ENTRY_MAX << ']' << endl;
  for (uint16_t i = 0; i < ENTRY_MAX; i++, addr += sizeof(entry)) {
    if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry)) 
      return (-2);
    if (entry.type == FREE_TYPE) continue;
    outs << hex << addr << PSTR(":name=\"") << entry.name << '\"';
    if ((entry.type & 0x8000) == 0) outs << PSTR("deleted,");
    outs << PSTR(",type=");
    switch (entry.type | 0x8000) {
    case DIR_BLOCK_TYPE: outs << PSTR("DIR_BLOCK"); break;
    case DIR_ENTRY_TYPE: outs << PSTR("DIR_ENTRY"); break;
    case FILE_ENTRY_TYPE: outs << PSTR("FILE_ENTRY"); break;
    }
    outs << PSTR(",size=") << entry.size;
    outs << PSTR(",ref=") << hex << entry.ref;
    if (entry.type == FILE_ENTRY_TYPE) {
      uint32_t ref = entry.ref;
      if (ref == NULL_REF) return (-3);
      while (1) {
	if (device->read(&entry, ref, sizeof(entry)) != sizeof(entry)) 
	  return (-3);
	if (entry.type != FILE_BLOCK_TYPE) return (-4);
	ref = entry.ref;
	if (ref == NULL_REF) break;
	outs << PSTR(",ref=") << hex << entry.ref;
      }
    }
    outs << endl;
  }
  return (0);
}
#else
int
CFFS::ls(IOStream& outs)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (-1);
  uint32_t addr = current_dir_addr;
  descr_t entry;
  uint8_t j = 0;
  if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry)) 
    return (-2);
  const uint16_t ENTRY_MAX = entry.size / sizeof(entry);
  for (uint16_t i = 0; i < ENTRY_MAX; i++, addr += sizeof(entry)) {
    if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry)) 
      return (-3);
    if ((entry.type == DIR_BLOCK_TYPE) 
	|| (entry.type == DIR_ENTRY_TYPE)
	|| (entry.type == FILE_ENTRY_TYPE)) {
      outs << entry.name << tab;
      j += 1;
      if ((j & 0x7) == 0) outs << endl;
    }
  }
  if (j & 0x7) outs << endl;
  return (0);
}
#endif

int
CFFS::rm(const char* filename)
{
  File file;
  int res;
  res = file.open(filename, O_READ);
  if (res < 0) return (res);
  return (file.remove());
}

int 
CFFS::cd(const char* filename)
{
  descr_t entry;
  uint32_t addr = CFFS::lookup(filename, entry);
  if (addr == 0L) return (-1);
  if ((entry.type != DIR_BLOCK_TYPE) && 
      (entry.type != DIR_ENTRY_TYPE)) 
    return (-1);
  current_dir_addr = entry.ref;
  return (0);
}

int 
CFFS::mkdir(const char* filename)
{
  descr_t entry;
  int res = CFFS::create(filename, CFFS::DIR_ENTRY_TYPE, O_EXCL, entry);
  if (res < 0) return (res);
  return (0);
}

int 
CFFS::rmdir(const char* filename)
{
  UNUSED(filename);
  return (-1);
}
  
int 
CFFS::format(Flash::Device* flash, const char* name)
{
  // Check that the file system driver is initiated
  if (device != NULL) return (-1);

  // Check that the drive name is not too long
  if (strlen(name) >= FILENAME_MAX) return (-2);

  // Erase sectors
  descr_t header;
  uint32_t addr = 0L; 
  const uint8_t SIZE = flash->SECTOR_BYTES / 1024;
  for (uint16_t i = 0; i < flash->SECTOR_MAX; i++) {
    if (flash->read(&header, addr, sizeof(header)) != sizeof(header)) 
      return (-3);
    if (header.type != FREE_TYPE) {
      if (flash->erase(addr, SIZE) != 0) return (-4);
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
    return (-5);

  // Write root directory
  addr += sizeof(header);
  memset(&header, 0, sizeof(header));
  header.type = DIR_BLOCK_TYPE;
  header.size = flash->DEFAULT_SECTOR_BYTES - sizeof(header);
  header.ref = addr;
  strcpy_P(header.name, PSTR(".."));
  if (flash->write(addr, &header, sizeof(header)) != sizeof(header)) 
    return (-6);
  return (0);
}

uint32_t
CFFS::lookup(const char* filename, descr_t &entry)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (-1);

  // Read current director and lookup entry with given filename
  uint32_t addr = current_dir_addr;
  if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry)) 
    return (-2);
  const uint16_t ENTRY_MAX = entry.size / sizeof(descr_t);
  addr = entry.ref;
  for (uint16_t i = 0; i < ENTRY_MAX; i++, addr += sizeof(descr_t)) {
    if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry)) 
      return (-2);
    if (entry.type == FREE_TYPE) break;
    if ((entry.type & ALLOC_MASK) == 0) continue;
    if (strcmp(filename, entry.name)) continue;
    return (addr);
  }
  return (0L);
}

uint32_t
CFFS::create(const char* filename, uint16_t type, uint8_t flags, descr_t &entry)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (-1);

  // Check parameters
  if ((type != DIR_ENTRY_TYPE) && (type != FILE_ENTRY_TYPE)) return (0L);
  if (strlen(filename) >= FILENAME_MAX) return (0L);

  // Search through the current directory
  uint32_t addr = current_dir_addr;
  if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry)) 
    return (-2);
  uint16_t ENTRY_MAX = entry.size / sizeof(descr_t);
  addr = entry.ref;
  for (uint16_t i = 0; i < ENTRY_MAX; i++, addr += sizeof(descr_t)) {
    if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry)) 
      return (0L);
    // Skip deleted entries
    if ((entry.type & ALLOC_MASK) == 0) continue;
    // Check if file name is already used; error or remove
    if (!strcmp(filename, entry.name)) {
      if ((flags & O_EXCL) || (type == DIR_ENTRY_TYPE)) return (-3);
      entry.type &= TYPE_MASK;
      if (device->write(addr, &entry, sizeof(entry)) != sizeof(entry)) 
	return (0L);
    }
    else if (entry.type == FREE_TYPE) {
      // Creating a directory or file entry
      if (type == DIR_ENTRY_TYPE) {
	uint32_t addr = next_free_directory();
	if (addr == 0L) return (0L);
	entry.ref = addr;
      }
      else {
	uint32_t sector = next_free_sector();
	if (sector == 0L) return (0L);
	entry.ref = sector;
      }
      strcpy(entry.name, filename);
      entry.type = type;
      entry.size = sizeof(entry);
      // Write the entry and return the address
      if (device->write(addr, &entry, sizeof(entry)) != sizeof(entry)) 
	return (0L);
      return (addr);
    }
  }
  
  // Directory is full
  return (0L);
}

int
CFFS::remove(uint32_t addr)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (-1);

  // Read directory entry to remove and check type (dir/file)
  descr_t entry;
  if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry)) 
    return (-2);
  if ((entry.type != DIR_ENTRY_TYPE) || (entry.type != FILE_ENTRY_TYPE)) return (-3);

  // Fix: Check directory files are removed
  // Fix: Mark file sectors as removed
  
  // Mark the entry as removed but maintain rest of information for cleanup
  entry.type &= TYPE_MASK;
  if (device->write(addr, &entry, sizeof(entry)) != sizeof(entry)) 
    return (-4);
  return (0);
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
CFFS::lookup_end_of_file(uint32_t addr, uint32_t &pos, uint32_t &size)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (-1);

  // Locate last sector
  descr_t header;
  size = 0L;
  while (1) {
    if (device->read(&header, addr, sizeof(header)) != sizeof(header)) 
      return (0L);
    if (header.type != FILE_BLOCK_TYPE) return (-1);
    if (header.size != device->SECTOR_BYTES) return (-1);
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
      return (-1);
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
