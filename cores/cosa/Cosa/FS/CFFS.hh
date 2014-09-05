/**
 * @file Cosa/FS/CFFS.hh
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

#ifndef COSA_FS_CFFS_HH
#define COSA_FS_CFFS_HH

#include "Cosa/FS.hh"
#include "Cosa/SPI/Driver/S25FL127S.hh"
#include "Cosa/IOStream.hh"

/**
 * Cosa Flash File System for S25FL127S Flash Memory. 
 *
 * @section Warning
 * This version will not allow files larger than 64Kbyte and will not
 * reclaim deleted directory entries.
 */
class CFFS {
public:
  /** Max size of file name */
  static const size_t FILENAME_MAX = 30;

protected:
  /**
   * CFFS directory entry. Total size is 32 bytes per entry to allow 
   * max 128 entries per directory. First entry is used for the parent
   * directory reference. The type is DIR_TYPE, name is ".." and the
   * value is the 4K sector index of the parent directory. The root
   * directory parent is a self reference.
   */
  struct dir_t {
    uint8_t type;		//!< Type of file and entry state.
    char name[FILENAME_MAX];	//!< Printable name of file (zero terminated).
    union {
      uint8_t first_sector;     //!< High byte of sector address.
      uint8_t dir_index;	//!< Directory index.
      uint8_t value;		//!< Entry value.
    };
  };

  /**
   * CFFS sector header entry.
   */
  struct sector_t {
    uint8_t next_sector;	//!< High byte of 64-KB next sector.
    uint8_t magic;		//!< Magic number
  };
  
public:
  /**
   * Flash File access class. Support for directories, text and binary
   * files. Text files may not use the value (0xff). Binary files must
   * pad each entry with zero(0x00). Files may be read/written. Write
   * should always be in append mode as the file cannot be rewritten
   * with any value. Opening a file that already exists in append mode
   * will require locating the end of the file. 
   */
  class File : public IOStream::Device {
  public:
    /**
     * Construct file access instance. Must be use open() before any
     * operation are possible.
     */
    File() : IOStream::Device(), m_flags(0) {}
    
    /**
     * Open a file by path and mode flags. Returns zero(0) if
     * successful otherwise a negative error code. 
     * @param[in] filename of file to open.
     * @param[in] oflag mode of file open.
     * @return zero or negative error code.
     */
    int open(const char* filename, uint8_t oflag = O_READ);
    
    /**
     * Checks the file's open/closed status.
     * @return the value true if a file is open otherwise false;
     */
    bool is_open(void) const { return (m_flags & O_RDWR) != 0; }

    /**
     * Remove a file. The directory entry and all data for the file
     * are deleted. Returns zero(0) if succesful otherwise a negative
     * error code. 
     * @return zero or negative error code.
     */
    int remove();

    /**
     * Close a file. Return zero(0) if successful otherwise a negative
     * error code.
     * @return zero or negative error code.
     */
    int close();

    /**
     * Sets the file's read position relative to mode. Return zero(0)
     * if successful otherwise a negative error code.
     * @param[in] pos new position in bytes from given mode.
     * @param[in] mode absolute, relative and from end.
     * @return zero or negative error code.
     */
    int seek(uint32_t pos, uint8_t whence = SEEK_SET);

    /**
     * Return current position. 
     */
    uint32_t tell()
    { 
      return (m_current_pos); 
    }

    /**
     * Rewind to the start of the file.
     */
    int rewind()
    { 
      return (seek(0L)); 
    }

    /**
     * Return number of bytes in file. 
     */
    uint32_t size()
    { 
      return (m_file_size); 
    }

    /**
     * @override IOStream::Device
     * Write data from buffer with given size to the file. If
     * successful returns number of bytes written or negative error
     * code.
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @return number of bytes written or negative error code.
     */
    virtual int write(const void *buf, size_t size);
  
    /**
     * @override IOStream::Device
     * Write data from buffer in program memory with given size to the
     * file. If successful returns number of bytes written or negative
     * error code.  
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @return number of bytes written or negative error code.
     */
    virtual int write_P(const void* buf, size_t size);

    /**
     * @override IOStream::Device
     * Read character/byte from the file. If successful returns byte
     * read or negative error code.  
     * @return character or negative error code.
     */
    virtual int getchar();

    /**
     * @override IOStream::Device
     * Read data to given buffer with given size from the file. If
     * successful returns number of bytes read or negative error code.  
     * @param[in] buf buffer to read into.
     * @param[in] size number of bytes to read.
     * @return number of bytes read or negative error code.
     */
    virtual int read(void* buf, size_t size);
    
  protected:
    uint8_t m_flags;			//!< File open flags.
    uint8_t m_dir_index;		//!< Directory index.
    uint8_t m_entry_index;		//!< Entry index.
    CFFS::dir_t m_entry;		//!< Directory entry.
    CFFS::sector_t m_sector;		//!< Sector header.
    uint32_t m_file_size;		//!< File size.
    uint32_t m_current_addr;		//!< Current flash address.
    uint32_t m_current_pos;		//!< Current logical position.
  };

  /**
   * Mount a CFFS volume on the given flash device. Return true if
   * successful otherwise false. 
   * @param[in] flash device to mount.
   * @return bool.
   */
  static bool begin(S25FL127S* flash);

  /** 
   * List directory contents to given iostream. Return zero(0) if
   * successful otherwise a negative error code. 
   * @param[in] outs output stream.
   * @param[in] verbose list all files (default false).
   * @return zero or negative error code.
   */
  static int ls(IOStream& outs, bool verbose = false);

  /**
   * Remove a file. The directory entry and all data for the file is 
   * deleted. Return zero(0) if successful otherwise a negative error
   * code. 
   * @param[in] filename to remove. 
   * @return zero or negative error code.
   */
  static int rm(const char* filename);
  
  /**
   * Change current directory to the given filename in the current
   * directory. 
   * Return zero(0) if successful otherwise a negative error code. 
   * @param[in] filename directory to change to.
   * @return zero or negative error code.
   */
  static int cd(const char* filename);
  
  /**
   * Create a directory with the given filename in the current
   * directory. Return zero(0) if successful otherwise a negative
   * error code.  
   * @param[in] filename directory to create.
   * @return zero or negative error code.
   */
  static int mkdir(const char* filename);
  
  /**
   * Remove directory with the given filename in the current
   * directory. Return zero(0) if successful otherwise a negative
   * error code.  
   * @param[in] filename directory to remove.
   * @return zero or negative error code.
   */
  static int rmdir(const char* filename);
  
  /**
   * Format the flash. Create a CFFS volume with root directory. 
   * Returns zero(0) if successful otherwise a negative error code.   
   * @param[in] flash device to mount.
   * @return zero or negative error code.
   */
  static int format(S25FL127S* flash);

  friend class File;

protected:
  /**
   * CFFS directory entry type.
   */
  enum {
    DIR_TYPE = 0x80,		//!< Directory entry.
    FILE_TYPE = 0x81,		//!< File entry.
    FREE_TYPE = 0xff,		//!< Free entry.
    ALLOC_MASK = 0x80,		//!< Allocated directory entry.
    TYPE_MASK = 0x7f		//!< Directory entry type.
  } __attribute__((packed));

  /** Sector header magic number */
  static const uint8_t MAGIC = 0xa5;
  
  /** Null next sector */
  static const uint8_t NULL_NEXT_SECTOR = 0xff;

  /** Max number of directory entries. */
  static const size_t DIR_MAX = S25FL127S::SECTOR4K_MAX / sizeof(dir_t);

  /** Current flash device. */
  static S25FL127S* device;

  /** Current directory index. */
  static uint8_t current_dir_index;

  /**
   * Read flash block with the given size into the buffer from the
   * source address. Return number of bytes read or negative error
   * code. 
   * @param[in] dest buffer to read from flash into.
   * @param[in] src address in flash to read from.
   * @param[in] size number of bytes to read.
   * @return number of bytes or negative error code.
   */
  static int read(void* dest, uint32_t src, size_t size)
    __attribute__((always_inline))
  {
    if (device == NULL) return (-1);
    return (device->read(dest, src, size));
  }

  /**
   * Write flash block at given destination address with the contents
   * of the source buffer. Return number of bytes written or negative
   * error code. 
   * @param[in] dest address in flash to write to.
   * @param[in] src buffer to write to flash.
   * @param[in] size number of bytes to write.
   * @return number of bytes or negative error code.
   */
  static int write(uint32_t dest, const void* src, size_t size)
    __attribute__((always_inline))
  {
    if (device == NULL) return (-1);
    return (device->write(dest, src, size));
  }

  /**
   * Write flash block at given destination address with contents
   * of the source buffer in program memory. Return number of bytes
   * written or negative error code.  
   * @param[in] buf buffer to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes written or EOF(-1).
   */
  static int write_P(uint32_t dest, const void* src, size_t size)
    __attribute__((always_inline))
  {
    if (device == NULL) return (-1);
    return (device->write_P(dest, src, size));
  }
  
  /**
   * Lookup directory entry with the given file name. Return entry
   * index if found and entry setting, otherwise a negative error
   * code. 
   * @param[in] filename to lookup.
   * @param[out] entry setting return.
   * @return entry index or negative error code.
   */
  static int lookup_entry(const char* filename, dir_t &entry);

  /**
   * Create directory entry with given file name and type. The flags (O_EXCL)
   * may be used to fail if the file already exists. The type must be
   * DIR_TYPE or FILE_TYPE. Returns entry index and entry setting if
   * created otherwise a negative error code. 
   * @param[in] filename to create.
   * @param[in] type of entry to create.
   * @param[in] flags open flags to check.
   * @return entry index or negative error code.
   */
  static int create_entry(const char* filename, uint8_t type, uint8_t flags, 
			  dir_t &entry);

  /**
   * Remove directory entry. Returns zero(0) if successful otherwise a
   * negative error code. 
   * @param[in] dir_index directory index.
   * @param[in] entry_index entry index to remove.
   * @return zero or negative error code.
   */
  static int remove_entry(uint8_t dir_index, uint8_t entry_index);

  /**
   * Allocate next free sector. Returns sector number (0..255) or
   * negative error code.
   * @return sector number or negative error code.
   */
  static int next_free_sector();
  
  /**
   * Allocate next free directory. Returns directory index (0..15) or
   * negative error code.
   * @return directory index or negative error code.
   */
  static int next_free_directory();
  
  /**
   * Locate address and size of file that starts with the given
   * sector. Return zero(0) if successful otherwise a negative error
   * code. 
   * @param[in] sector index of first file sector.
   * @param[out] pos address of end of file.
   * @param[out] size of file.
   * @return zero or negative error code.
   */
  static int lookup_end_of_file(uint8_t sector, 
				uint32_t &pos, uint32_t &size);
};

#endif
