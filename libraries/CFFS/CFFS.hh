/**
 * @file CFFS.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#ifndef COSA_CFFS_HH
#define COSA_CFFS_HH

#include "Cosa/FS.hh"
#include "Cosa/Flash.hh"
#include "Cosa/IOStream.hh"

/**
 * Cosa Flash File System for Flash Memory.
 *
 * @section Limitations
 * Directory entries are not reclaimed (directory block is not erased
 * and rewritten when full).
 */
class CFFS {
public:
  /** Max size of file/drive name. */
  static const size_t FILENAME_MAX = 22;

protected:
  /**
   * CFFS object descriptor.
   */
  struct descr_t {
    uint16_t type;		//!< Type of file and entry state.
    uint32_t size;		//!< Number of bytes (including header).
    uint32_t ref;		//!< Reference value (pointer).
    char name[FILENAME_MAX];	//!< Printable name of object(zero terminated).
  };

  /**
   * CFFS object types:
   *
   * CFFS_TYPE is the file system master block on the device. It is
   * the first object on the flash; size is the size of the header,
   * ref is the address of the root directory (the next object), and
   * the name of the device.
   *
   * FILE_ENTRY_TYPE is a file descriptor; size is not used, ref is
   * the  address of the first file block, name is the name of the file.
   *
   * FILE_BLOCK_TYPE is a file block; size is the block size (typically
   * sector size), ref is the address of the next block, name is not
   * used (filled with zero).

   * DIR_ENTRY_TYPE is a directory reference; size is not used, ref is
   * the address of the directory block, name is the name of the
   * directory.
   *
   * DIR_BLOCK_TYPE is directory block header; size is the directory
   * sector, ref is the address to the next directory block (NULL is
   * encoded as 0xffffffffL, NULL_REF)
   */
  enum {
    CFFS_TYPE = 0xf5cf,		//!< File System Master header.
    FILE_ENTRY_TYPE = 0x8001,	//!< File descriptor entry.
    FILE_BLOCK_TYPE = 0x8002,	//!< File data block.
    DIR_ENTRY_TYPE = 0x8003,	//!< Directory reference entry.
    DIR_BLOCK_TYPE = 0x8004,	//!< Directory block.
    FREE_TYPE = 0xffff,		//!< Free descriptor.
    ALLOC_MASK = 0x8000,	//!< Allocated mask.
    TYPE_MASK = 0x7fff		//!< Type mask.
  };

  /**
   * CFFS null address in flash data structures.
   */
  static const uint32_t NULL_REF = 0xffffffffL;

public:
  /**
   * Flash File access class. Support for directories, hard links,
   * text and binary files. The end of the file is not store in the
   * directory entry, instead it is located when the file is
   * opened. This is done by searching for the first non-0xff value
   * from the end of the last file sector. Text files may not use the
   * value (0xff). Binary files must end each entry with non-0xff
   * entry. Write should always be in append mode as the file cannot
   * be rewritten with any value.
   */
  class File : public IOStream::Device {
  public:
    /**
     * Construct file access instance. Call open() before any
     * read/write operations are possible.
     */
    File() : IOStream::Device(), m_flags(0) {}

    /**
     * Open a file by name and mode flags. Returns zero(0) if
     * successful otherwise a negative error code (EBUSY, EPERM,
     * EINVAL, ENAMETOOLONG, EIO, EEXIST, ENFILE, ENOSPC).
     * @param[in] filename of file to open.
     * @param[in] oflag mode of file open.
     * @return zero or negative error code.
     */
    int open(const char* filename, uint8_t oflag = O_READ);

    /**
     * Checks the file's open/closed status. Return true if open
     * otherwise false.
     * @return bool.
     */
    bool is_open(void) const
    {
      return ((m_flags & O_RDWR) != 0);
    }

    /**
     * Remove a file. The directory entry and all data for the file
     * are deleted. Returns zero(0) if succesful otherwise a negative
     * error code (EPREM, EIO, ENOENT).
     * @return zero or negative error code.
     */
    int remove();

    /**
     * Close a file. Return zero(0) if successful otherwise a negative
     * error code (EPREM).
     * @return zero or negative error code.
     */
    int close();

    /**
     * Sets the file's read position relative to mode. Return zero(0)
     * if successful otherwise a negative error code (EPERM, EINVAL,
     * EIO, ENXIO).
     * @param[in] pos new position in bytes from given mode.
     * @param[in] mode absolute, relative and from end.
     * @return zero or negative error code.
     */
    int seek(uint32_t pos, uint8_t whence = SEEK_SET);

    /**
     * Return current position.
     * @return position.
     */
    uint32_t tell()
    {
      return (m_current_pos);
    }

    /**
     * Rewind to the start of the file. Return zero(0) if successful
     * otherwise a negative error code (EPERM, EINVAL,EIO, ENXIO).
     * @return zero or negative error code.
     */
    int rewind()
    {
      return (seek(0L));
    }

    /**
     * Return number of bytes in file.
     * @return size.
     */
    uint32_t size()
    {
      return (m_file_size);
    }

    /** Overloaded virtual member functions. */
    using IOStream::Device::write;

    /**
     * @override{IOStream::Device}
     * Write data from buffer with given size to the file. If
     * successful returns number of bytes written or negative error
     * code (EPREM, EFAULT, ENOSPC, EIO, ENXIO).
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @return number of bytes written or negative error code.
     */
    virtual int write(const void *buf, size_t size);

    /**
     * @override{IOStream::Device}
     * Write data from buffer in program memory with given size to the
     * file. If successful returns number of bytes written or negative
     * error (EPREM, EFAULT, ENOSPC, EIO, ENXIO).
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @return number of bytes written or negative error code.
     */
    virtual int write_P(const void* buf, size_t size);

    /**
     * @override{IOStream::Device}
     * Read character/byte from the file. If successful returns character
     * read or negative error code (EPREM, EFAULT, ENOSPC, EIO, ENXIO).
     * @return character or negative error code.
     */
    virtual int getchar();

    /** Overloaded virtual member functions. */
    using IOStream::Device::read;

    /**
     * @override{IOStream::Device}
     * Read data to given buffer with given size from the file. If
     * successful returns number of bytes read or negative error code
     * (EPERM, EIO, ENXIO).
     * @param[in] buf buffer to read into.
     * @param[in] size number of bytes to read.
     * @return number of bytes read or negative error code.
     */
    virtual int read(void* buf, size_t size);

  protected:
    uint8_t m_flags;			//!< File open flags.
    uint32_t m_entry_addr;		//!< Entry address.
    CFFS::descr_t m_entry;		//!< Cached directory entry.
    CFFS::descr_t m_sector;		//!< Cached sector header.
    uint32_t m_file_size;		//!< File size.
    uint32_t m_current_addr;		//!< Current flash address.
    uint32_t m_current_pos;		//!< Current logical position.

    /**
     * @override{IOStream::Device}
     * Write data from buffer in data or program memory with given
     * size to the file. If successful returns number of bytes written
     * or negative error code.
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @param[in] progmem from data(false) or program memory(true).
     * @return number of bytes written or negative error code.
     */
    int write(const void* buf, size_t size, bool progmem);
  };

  /**
   * Mount a CFFS volume on the given flash device. Return true if
   * successful otherwise false.
   * @param[in] flash device to mount.
   * @return bool.
   */
  static bool begin(Flash::Device* flash);

  /**
   * List directory contents to given iostream. Return zero(0) if
   * successful otherwise a negative error code (EPERM, EIO).
   * @param[in] outs output stream.
   * @return zero or negative error code.
   */
  static int ls(IOStream& outs);

  /**
   * Remove a file. The directory entry and all data for the file is
   * deleted. Return zero(0) if successful otherwise a negative error
   * code (EPREM, EIO, ENOENT, ENFILE, EINVAL).
   * @param[in] filename to remove.
   * @return zero or negative error code.
   */
  static int rm(const char* filename);

  /**
   * Change current directory to the given filename in the current
   * directory. Return zero(0) if successful otherwise a negative
   * error code (EPREM, EIO, ENOENT, ENOTDIR).
   * @param[in] filename directory to change to.
   * @return zero or negative error code.
   */
  static int cd(const char* filename);

  /**
   * Create a directory with the given filename in the current
   * directory. Return zero(0) if successful otherwise a negative
   * error code (EPERM, EIO, EEXIST).
   * @param[in] filename directory to create.
   * @return zero or negative error code.
   */
  static int mkdir(const char* filename);

  /**
   * Remove directory with the given filename in the current
   * directory. Return zero(0) if successful otherwise a negative
   * error code (ENOSYS).
   * @param[in] filename directory to remove.
   * @return zero or negative error code.
   */
  static int rmdir(const char* filename);

  /**
   * Format the flash. Create a CFFS volume with root directory.
   * Returns zero(0) if successful otherwise a negative error code
   * (EPERM, ENAMETOOLONG, EIO).
   * @param[in] flash device to mount.
   * @param[in] name of file system.
   * @return zero or negative error code.
   */
  static int format(Flash::Device* flash, const char* name);

  friend class File;

protected:
  /** Number of directory sectors. */
  static const size_t DIR_MAX = 16;

  /** Current flash device (singleton). */
  static Flash::Device* device;

  /** Current directory address. */
  static uint32_t current_dir_addr;

  /**
   * Read flash block with the given size into the buffer from the
   * source address. Return number of bytes read or negative error
   * code.
   * @param[in] dest buffer to read from flash into.
   * @param[in] src address in flash to read from.
   * @param[in] size number of bytes to read.
   * @return number of bytes or negative error code.
   */
  static int read(void* dest, uint32_t src, size_t size);

  /**
   * Write flash block at given destination address with the contents
   * of the source buffer. Return number of bytes written or negative
   * error code.
   * @param[in] dest address in flash to write to.
   * @param[in] src buffer to write to flash.
   * @param[in] size number of bytes to write.
   * @return number of bytes or negative error code.
   */
  static int write(uint32_t dest, const void* src, size_t size);

  /**
   * Write flash block at given destination address with contents
   * of the source buffer in program memory. Return number of bytes
   * written or negative error code.
   * @param[in] dest address in flash to write to.
   * @param[in] src buffer in program memory to write to flash.
   * @param[in] size number of bytes to write.
   * @return number of bytes written or EOF(-1).
   */
  static int write_P(uint32_t dest, const void* src, size_t size);

  /**
   * Lookup directory entry with the given file name. Return zero and
   * entry if found otherwise negative error code.
   * @param[in] filename to lookup.
   * @param[out] entry setting.
   * @param[out] addr entry address.
   * @return zero and entry setting otherwise negative error code.
   */
  static int lookup(const char* filename, descr_t &entry, uint32_t& addr);

  /**
   * Create directory entry with given file name and type. The flags (O_EXCL)
   * may be used to fail if the file already exists. The type must be
   * DIR_TYPE or FILE_TYPE. Returns zero and entry otherwise negative
   * error code.
   * @param[in] filename to create.
   * @param[in] type of entry to create.
   * @param[in] flags open flags to check.
   * @param[out] entry setting.
   * @param[out] addr address of entry.
   * @return zero and entry otherwise negative error code.
   */
  static int create(const char* filename, uint16_t type, uint8_t flags,
		    descr_t &entry, uint32_t &addr);

  /**
   * Remove directory entry. Returns zero(0) if successful otherwise a
   * negative error code.
   * @param[in] addr entry address.
   * @param[in] type entry type.
   * @return zero or negative error code.
   */
  static int remove(uint32_t addr, uint16_t type);

  /**
   * Allocate next free sector. Returns sector address or zero.
   * @return sector address or zero.
   */
  static uint32_t next_free_sector();

  /**
   * Allocate next free directory. Returns directory address or zero.
   * @return directory address or zero.
   */
  static uint32_t next_free_directory();

  /**
   * Find address and size of file that starts with the given
   * sector. Return zero(0) if successful otherwise a negative error
   * code.
   * @param[in] sector address of sector.
   * @param[out] pos address of end of file.
   * @param[out] size of file.
   * @return zero or negative error code.
   */
  static int find_end_of_file(uint32_t sector, uint32_t &pos, uint32_t &size);
};

#endif
