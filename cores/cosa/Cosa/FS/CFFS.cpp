/**
 * @file Cosa/FS/CFFS.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel.
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

S25FL127S* CFFS::device = NULL;
uint8_t CFFS::current_dir_index = 0;

int 
CFFS::File::open(const char* filename, uint8_t oflag)
{
  // Check that the file is not open and the open mode
  if (is_open()) return (-1);

  // Check if the file should be created
  int res;
  if (oflag & O_CREAT) {
    oflag |= O_WRITE;
    res = CFFS::create_entry(filename, CFFS::FILE_TYPE, oflag, m_entry);
    if (res < 0) return (res);
    m_entry_index = res;
    m_file_size = 0L;
    m_current_addr = (m_entry.first_sector * S25FL127S::SECTOR_MAX);
    m_current_addr += sizeof(CFFS::sector_t);
    m_current_pos = 0L;
  } 

  // Check that the file exists; open file 
  else {
    if ((oflag & O_WRITE) == 0) oflag |= O_READ;
    res = CFFS::lookup_entry(filename, m_entry);
    if (res < 0) return (res);
    m_entry_index = res;
    res = lookup_end_of_file(m_entry.first_sector, m_current_addr, m_file_size);
    if (res < 0) return (res);
    m_current_pos = m_file_size;
  }

  // Check if the position should be from the start of the file
  if (((oflag & O_RDWR) == O_READ) || (oflag & O_CREAT)) {
    m_current_addr = (m_entry.first_sector * S25FL127S::SECTOR_MAX);
    m_current_addr += sizeof(CFFS::sector_t);
    m_current_pos = 0L;
  }

  // Save flags and the directory index 
  m_flags = oflag;
  m_dir_index = CFFS::current_dir_index;
  return (0);
}

int 
CFFS::File::remove()
{
  if (m_flags == 0) return (-1);
  return (CFFS::remove_entry(m_dir_index, m_entry_index));
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
  m_current_addr = (m_entry.first_sector * S25FL127S::SECTOR_MAX);
  m_current_addr += sizeof(CFFS::sector_t) + pos;
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
CFFS::begin(S25FL127S* flash)
{
  // Check that the file system is not already initiated
  if (device != NULL) return (false);
  device = flash;

  // Check that the root directory is defined
  dir_t entry;
  if ((device->read(&entry, 0L, sizeof(entry)) == sizeof(entry))
      && (entry.type == DIR_TYPE)
      && (entry.dir_index == 0)
      && !strcmp_P(entry.name, PSTR("..")))
    return (true);

  // Failed: not formated device
  device = NULL;
  return (false);
}

int
CFFS::ls(IOStream& outs, bool verbose)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (-1);
  uint32_t addr = (current_dir_index << 12);
  dir_t entry;
  uint8_t j = 0;
  if (verbose)
    outs << PSTR("current_dir_index = ") << current_dir_index << endl;
  for (uint8_t i = 0; i < DIR_MAX; i++, addr += sizeof(entry)) {
    if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry)) 
      return (-2);
    if (entry.type != FREE_TYPE) {
      if (verbose) {
	outs << i << PSTR(":name = ") << '"' << entry.name << '"';
	outs << PSTR(":type = ");
	switch (entry.type) {
	case DIR_TYPE:
	  outs << PSTR("dir(") << entry.dir_index << PSTR(")") << endl;
	  break;
	case FILE_TYPE:
	  outs << PSTR("file(") << entry.first_sector << PSTR(")") << endl;
	  break;
	default:
	  outs << PSTR("deleted(") << entry.type 
	       << PSTR(",") << entry.value << PSTR(")")
	       << endl;
	}
      }
      else if ((entry.type == DIR_TYPE) || (entry.type == FILE_TYPE)) {
	outs << entry.name << tab;
	j += 1;
	if ((j & 0x7) == 0) outs << endl;
      }
    }
  }
  if (j & 0x7) outs << endl;
  return (0);
}

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
  dir_t entry;
  int res = CFFS::lookup_entry(filename, entry);
  if (res < 0) return (res);
  if (entry.type != DIR_TYPE) return (-1);
  current_dir_index = entry.dir_index;
  return (0);
}

int 
CFFS::mkdir(const char* filename)
{
  dir_t entry;
  int res = CFFS::create_entry(filename, CFFS::DIR_TYPE, O_EXCL, entry);
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
CFFS::format(S25FL127S* flash)
{
  // Check that the file system driver is initiated
  if (device != NULL) return (-1);

  // Erase directories (first major sector contains the minor sectors)
  dir_t entry;
  uint32_t addr = 0L; 
  for (uint8_t i = 0; i < S25FL127S::SECTOR4K_COUNT; i++) {
    if (flash->read(&entry, addr, sizeof(dir_t)) != sizeof(dir_t)) 
      return (-2);
    if (entry.type != FREE_TYPE) {
      if (flash->erase(addr) != 0) return (-1);
    }
    addr += S25FL127S::SECTOR4K_MAX;
  }

  // Erase sectors
  sector_t header;
  addr = S25FL127S::SECTOR_MAX;
  for (uint8_t i = 1; i < S25FL127S::SECTOR_COUNT - 1; i++) {
    if (flash->read(&header, addr, sizeof(header)) != sizeof(header)) 
      return (-2);
    if (header.magic == MAGIC) {
      if (flash->erase(addr) != 0) return (-1);
    }
    addr += S25FL127S::SECTOR_MAX;
  }
  
  // Write root directory
  addr = 0L;
  entry.type = DIR_TYPE;
  strcpy_P(entry.name, PSTR(".."));
  entry.dir_index = 0;
  if (flash->write(addr, &entry, sizeof(entry)) != sizeof(entry)) 
    return (-2);
  return (0);
}

int 
CFFS::lookup_entry(const char* filename, dir_t &entry)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (-1);
  
  // Read current director and lookup entry with given filename
  uint32_t addr = (current_dir_index * S25FL127S::SECTOR4K_MAX);
  for (uint8_t i = 0; i < DIR_MAX; i++, addr += sizeof(dir_t)) {
    if (device->read(&entry, addr, sizeof(dir_t)) != sizeof(dir_t)) 
      return (-2);
    if (entry.type == FREE_TYPE) break;
    if ((entry.type & ALLOC_MASK) == 0) continue;
    if (strcmp(filename, entry.name)) continue;
    return (i);
  }
  return (-1);
}

int 
CFFS::create_entry(const char* filename, uint8_t type, uint8_t flags,
		   dir_t &entry)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (-1);

  // Check parameters
  if (type < DIR_TYPE || type > FILE_TYPE) return (-1);
  if (strlen(filename) >= FILENAME_MAX) return (-1);

  // Search through the current directory
  uint32_t addr = (current_dir_index * S25FL127S::SECTOR4K_MAX);
  for (uint16_t i = 0; i < DIR_MAX; i++, addr += sizeof(dir_t)) {
    if (device->read(&entry, addr, sizeof(entry)) != sizeof(entry)) 
      return (-2);
    // Skip deleted entries
    if ((entry.type & ALLOC_MASK) == 0) continue;
    // Check if file name is already used; error or remove
    if (!strcmp(filename, entry.name)) {
      if ((flags & O_EXCL) || (type == DIR_TYPE)) return (-3);
      entry.type &= TYPE_MASK;
      if (device->write(addr, &entry, sizeof(entry)) != sizeof(entry)) 
	return (-2);
    }
    else if (entry.type == FREE_TYPE) {
      // Creating a directory or file entry
      if (type == DIR_TYPE) {
	int index = next_free_directory();
	if (index < 0) return (-4);
	entry.dir_index = index;
      }
      else {
	int sector = next_free_sector();
	if (sector < 0) return (-4);
	entry.first_sector = sector;
      }
      strcpy(entry.name, filename);
      entry.type = type;
      // Write the entry and return the index
      if (device->write(addr, &entry, sizeof(entry)) != sizeof(entry)) 
	return (-2);
      return (i);
    }
  }
  
  // Directory is full
  return (-5);
}

int 
CFFS::remove_entry(uint8_t dir_index, uint8_t entry_index)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (-1);

  // Check parameters
  if (dir_index >= S25FL127S::SECTOR4K_COUNT) return (-1);
  if (entry_index >= DIR_MAX) return (-1);

  // Read directory entry to remove and check type (dir/file)
  uint32_t entry_addr;
  dir_t entry;
  entry_addr = S25FL127S::SECTOR4K_MAX * dir_index;
  entry_addr += sizeof(entry) * entry_index;
  if (device->read(&entry, entry_addr, sizeof(entry)) != sizeof(entry)) 
    return (-2);
  if (entry.type < DIR_TYPE || entry.type > FILE_TYPE) return (-4);

  // Fix: Check directory files are removed
  // Fix: Mark file sectors as removed
  
  // Mark the entry as removed
  entry.type &= TYPE_MASK;
  if (device->write(entry_addr, &entry, sizeof(entry)) != sizeof(entry)) 
    return (-3);
  return (0);
}

int 
CFFS::next_free_sector()
{
  // Check that the file system driver is initiated
  if (device == NULL) return (-1);

  // Iterate through the sectors (1..254); ignore directory and boot sectors
  sector_t header;
  uint32_t addr = 0L;
  for (uint8_t i = 1; i < (S25FL127S::SECTOR_COUNT - 1); i++) {
    addr += S25FL127S::SECTOR_MAX;
    if (device->read(&header, addr, sizeof(header)) != sizeof(header)) 
      return (-2);
    if (header.magic != 0xff) continue;
    header.magic = MAGIC;
    if (device->write(addr, &header, sizeof(header)) != sizeof(header)) 
      return (-3);
    return (i);
  }
  return (-1);
}

int 
CFFS::next_free_directory()
{
  // Check that the file system driver is initiated
  if (device == NULL) return (-1);

  // Search for a free directory
  uint32_t addr = 0L; 
  dir_t entry;
  for (uint8_t i = 0; i < S25FL127S::SECTOR4K_COUNT; i++) {
    if (device->read(&entry, addr, sizeof(dir_t)) != sizeof(dir_t)) 
      return (-2);
    if (entry.type != FREE_TYPE) {
      addr += S25FL127S::SECTOR4K_MAX;
      continue;
    }
    // Initiate the parent directory reference
    entry.type = DIR_TYPE;
    strcpy_P(entry.name, PSTR(".."));
    entry.dir_index = current_dir_index;
    if (device->write(addr, &entry, sizeof(entry)) != sizeof(entry)) 
      return (-2);
    // Return the directory index
    return (i);
  }
  return (-3);
}

int 
CFFS::lookup_end_of_file(uint8_t sector, uint32_t &pos, uint32_t &size)
{
  // Check that the file system driver is initiated
  if (device == NULL) return (-1);

  // Locate last sector
  sector_t header;
  uint32_t addr;
  int res;
  header.next_sector = sector;
  size = 0L;
  while (1) {
    addr = header.next_sector * S25FL127S::SECTOR_MAX;
    if (device->read(&header, addr, sizeof(header)) != sizeof(header)) 
      return (-2);
    if (header.magic != MAGIC) return (-4);
    if (header.next_sector == NULL_NEXT_SECTOR) break;
    size += (S25FL127S::SECTOR_MAX - sizeof(header));
  }

  // Locate end of sector
  uint8_t buf[256];
  addr += S25FL127S::SECTOR_MAX;
  for (uint16_t i = 0; i < S25FL127S::SECTOR_MAX / sizeof(buf); i++) {
    addr -= sizeof(buf);
    res = device->read(buf, addr, sizeof(buf));
    if (res != sizeof(buf)) return (-3);
    uint16_t j = sizeof(buf) - 1;
    while (buf[j] == 0xff) if (j == 0) break; else j--;
    if (j == 0) continue;
    addr += j + 1;
    break;
  }

  // And return position and size
  pos = addr;
  size += (addr & 0xffffL) - sizeof(header);
  return (0);
}

