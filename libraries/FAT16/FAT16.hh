/**
 * @file FAT16.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2009, William Greiman (Arduino Fat16 Library)
 * Copyright (C) 2013-2015, Mikael Patel (Refactoring)
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

#ifndef COSA_FAT16_HH
#define COSA_FAT16_HH

#include <SD.h>

#include "Cosa/IOStream.hh"
#include "Cosa/FS.hh"

/*
 * FAT16 file structures on SD card. Note: may only access files on the
 * root directory.
 *
 * @section Acknowledgement
 * Refactoring of Arduino Fat16 Library, Copyright (C) 2009 by William Greiman
 *
 * @section References
 * 1. http://www.microsoft.com/whdc/system/platform/firmware/fatgen.mspx
 * 2. http://code.google.com/p/fat16lib/
 */
class FAT16 {
protected:
  /** Value for byte 510 of boot block or MBR */
  static uint8_t const BOOTSIG0 = 0X55;

  /** Value for byte 511 of boot block or MBR */
  static uint8_t const BOOTSIG1 = 0XAA;

  /**
   * MBR partition table entry. A partition table entry for a MBR
   * formatted storage device. The MBR partition table has four entries.
   */
  struct part_t {
    /**
     * Boot Indicator. Indicates whether the volume is the active
     * partition.  Legal values include: 0X00. Do not use for booting.
     * 0X80 Active partition.
     */
    uint8_t boot;
    /**
     * Head part of Cylinder-head-sector address of the first block in
     * the partition. Legal values are 0-255. Only used in old PC BIOS.
     */
    uint8_t beginHead;
    /**
     * Sector part of Cylinder-head-sector address of the first block in
     * the partition. Legal values are 1-63. Only used in old PC BIOS.
     */
    unsigned beginSector : 6;
    /**
     * High bits cylinder for first block in partition.
     */
    unsigned beginCylinderHigh : 2;
    /**
     * Combine beginCylinderLow with beginCylinderHigh. Legal values
     * are 0-1023.  Only used in old PC BIOS.
     */
    uint8_t beginCylinderLow;
    /**
     * Partition type. See defines that begin with PART_TYPE_ for
     * some Microsoft partition types.
     */
    uint8_t type;
    /**
     * head part of cylinder-head-sector address of the last sector in the
     * partition.  Legal values are 0-255. Only used in old PC BIOS.
     */
    uint8_t endHead;
    /**
     * Sector part of cylinder-head-sector address of the last sector in
     * the partition.  Legal values are 1-63. Only used in old PC BIOS.
     */
    unsigned endSector : 6;
    /**
     * High bits of end cylinder
     */
    unsigned endCylinderHigh : 2;
    /**
     * Combine endCylinderLow with endCylinderHigh. Legal values
     * are 0-1023.  Only used in old PC BIOS.
     */
    uint8_t endCylinderLow;
    /**
     * Logical block address of the first block in the partition.
     */
    uint32_t firstSector;
    /**
     * Length of the partition, in blocks.
     */
    uint32_t totalSectors;
  };

  /**
   * Master Boot Record. The first block of a storage device that is
   * formatted with a MBR.
   */
  struct mbr_t {
    /**
     * Code Area for master boot program.
     */
    uint8_t codeArea[440];
    /**
     * Optional WindowsNT disk signature. May contain more boot code.
     */
    uint32_t diskSignature;
    /**
     * Usually zero but may be more boot code.
     */
    uint16_t usuallyZero;
    /**
     * Partition tables.
     */
    part_t part[4];
    /**
     * First MBR signature byte. Must be 0X55
     */
    uint8_t mbrSig0;
    /**
     * Second MBR signature byte. Must be 0XAA
     */
    uint8_t mbrSig1;
  };

  /**
   * BIOS parameter block; The BIOS parameter block describes the
   * physical layout of a FAT volume.
   */
  struct bpb_t {
    /**
     * Count of bytes per sector. This value may take on only the
     * following values: 512, 1024, 2048 or 4096
     */
    uint16_t bytesPerSector;
    /**
     * Number of sectors per allocation unit. This value must be a
     * power of 2 that is greater than 0. The legal values are
     * 1, 2, 4, 8, 16, 32, 64, and 128.
     */
    uint8_t sectorsPerCluster;
    /**
     * Number of sectors before the first FAT.
     * This value must not be zero.
     */
    uint16_t reservedSectorCount;
    /** The count of FAT data structures on the volume. This field should
     *  always contain the value 2 for any FAT volume of any type.
     */
    uint8_t fatCount;
    /**
     * For FAT12 and FAT16 volumes, this field contains the count of
     * 32-byte directory entries in the root directory. For FAT32 volumes,
     * this field must be set to 0. For FAT12 and FAT16 volumes, this
     * value should always specify a count that when multiplied by 32
     * results in a multiple of bytesPerSector.  FAT16 volumes should
     * use the value 512.
     */
    uint16_t rootDirEntryCount;
    /**
     * This field is the old 16-bit total count of sectors on the volume.
     * This count includes the count of all sectors in all four regions
     * of the volume. This field can be 0; if it is 0, then totalSectors32
     * must be non-zero.  For FAT32 volumes, this field must be 0. For
     * FAT12 and FAT16 volumes, this field contains the sector count, and
     * totalSectors32 is 0 if the total sector count fits
     * (is less than 0x10000).
     */
    uint16_t totalSectors16;
    /**
     * This dates back to the old MS-DOS 1.x media determination and is
     * no longer usually used for anything.  0xF8 is the standard value
     * for fixed (non-removable) media. For removable media, 0xF0 is
     * frequently used. Legal values are 0xF0 or 0xF8-0xFF.
     */
    uint8_t mediaType;
    /**
     * Count of sectors occupied by one FAT on FAT12/FAT16 volumes.
     * On FAT32 volumes this field must be 0, and sectorsPerFat32
     * contains the FAT size count.
     */
    uint16_t sectorsPerFat16;
    /**
     * Sectors per track for interrupt 0x13. Not used otherwise.
     */
    uint16_t sectorsPerTrtack;
    /**
     * Number of heads for interrupt 0x13.  Not used otherwise.
     */
    uint16_t headCount;
    /**
     * Count of hidden sectors preceding the partition that contains this
     * FAT volume. This field is generally only relevant for media
     *  visible on interrupt 0x13.
     */
    uint32_t hidddenSectors;
    /**
     * This field is the new 32-bit total count of sectors on the volume.
     * This count includes the count of all sectors in all four regions
     * of the volume.  This field can be 0; if it is 0, then
     * totalSectors16 must be non-zero.
     */
    uint32_t totalSectors32;
    /**
     * Count of sectors occupied by one FAT on FAT32 volumes.
     */
    uint32_t sectorsPerFat32;
    /**
     * This field is only defined for FAT32 media and does not exist on
     * FAT12 and FAT16 media.
     * Bits 0-3 -- Zero-based number of active FAT, Only valid if
     * mirroring is disabled.
     * Bits 4-6 -- Reserved.
     * Bit 7 -- 0 means the FAT is mirrored at runtime into all FATs,
     * 1 means only one FAT is active; it is the one referenced in bits 0-3.
     * Bits 8-15 -- Reserved.
     */
    uint16_t fat32Flags;
    /**
     * FAT32 version. High byte is major revision number.
     * Low byte is minor revision number. Only 0.0 define.
     */
    uint16_t fat32Version;
    /**
     * Cluster number of the first cluster of the root directory for FAT32.
     * This usually 2 but not required to be 2.
     */
    uint32_t fat32RootCluster;
    /**
     * Sector number of FSINFO structure in the reserved area of the
     * FAT32 volume. Usually 1.
     */
    uint16_t fat32FSInfo;
    /**
     * If non-zero, indicates the sector number in the reserved area
     * of the volume of a copy of the boot record. Usually 6.
     * No value other than 6 is recommended.
     */
    uint16_t fat32BackBootBlock;
    /**
     * Reserved for future expansion. Code that formats FAT32 volumes
     * should always set all of the bytes of this field to 0.
     */
    uint8_t fat32Reserved[12];
  };

  /**
   * Boot sector for a FAT16 or FAT32 volume.
   */
  struct fbs_t {
    /**
     * X86 jmp to boot program
     */
    uint8_t jmpToBootCode[3];
    /**
     * Informational only - don't depend on it
     */
    char oemName[8];
    /**
     * BIOS Parameter Block
     */
    bpb_t bpb;
    /**
     * For int0x13 use value 0X80 for hard drive
     */
    uint8_t driveNumber;
    /**
     * Used by Windows NT - should be zero for FAT
     */
    uint8_t reserved1;
    /**
     * 0X29 if next three fields are valid
     */
    uint8_t bootSignature;
    /**
     * Usually generated by combining date and time
     */
    uint32_t volumeSerialNumber;
    /**
     * Should match volume label in root dir
     */
    char volumeLabel[11];
    /**
     * Informational only - don't depend on it
     */
    char fileSystemType[8];
    /**
     * X86 boot code
     */
    uint8_t bootCode[420];
    /**
     * Must be 0X55
     */
    uint8_t bootSectorSig0;
    /**
     * Must be 0XAA
     */
    uint8_t bootSectorSig1;
  };

  /** FAT16 end of chain value used by Microsoft. */
  static uint16_t const EOC16 = 0XFFFF;

  /** Minimum value for FAT16 EOC.  Use to test for EOC. */
  static uint16_t const EOC16_MIN = 0XFFF8;

  /** FAT32 end of chain value used by Microsoft. */
  static uint32_t const EOC32 = 0X0FFFFFFF;

  /** Minimum value for FAT32 EOC.  Use to test for EOC. */
  static uint32_t const EOC32_MIN = 0X0FFFFFF8;

  /** Mask a for FAT32 entry. Entries are 28 bits. */
  static uint32_t const ENTRY32_MASK = 0X0FFFFFFF;

  /**
   * FAT short directory entry. Short means short 8.3 name, not the
   * entry size.
   */
  struct dir_t {
    /**
     * Short 8.3 name.
     * The first eight bytes contain the file name with blank fill.
     * The last three bytes contain the file extension with blank fill.
     */
    uint8_t name[11];
    /**
     * Entry attributes. The upper two bits of the attribute byte are
     * reserved and should always be set to 0 when a file is created
     * and never modified or looked at after that.  See defines that
     * begin with DIR_ATT_.
     */
    uint8_t attributes;
    /**
     * Reserved for use by Windows NT. Set value to 0 when a file is
     * created and never modify or look at it after that.
     */
    uint8_t reservedNT;
    /**
     * The granularity of the seconds part of creationTime is 2 seconds
     * so this field is a count of tenths of a second and its valid
     * value range is 0-199 inclusive. (WHG note - seems to be hundredths)
     */
    uint8_t creationTimeTenths;
    /**
     * Time file was created.
     */
    uint16_t creationTime;
    /**
     * Date file was created.
     */
    uint16_t creationDate;
    /**
     * Last access date. Note that there is no last access time, only
     * a date.  This is the date of last read or write. In the case of
     * a write, this should be set to the same date as lastWriteDate.
     */
    uint16_t lastAccessDate;
    /**
     * High word of this entry's first cluster number (always 0 for a
     * FAT12 or FAT16 volume).
     */
    uint16_t firstClusterHigh;
    /**
     * Time of last write. File creation is considered a write.
     */
    uint16_t lastWriteTime;
    /**
     * Date of last write. File creation is considered a write.
     */
    uint16_t lastWriteDate;
    /**
     * Low word of this entry's first cluster number.
     */
    uint16_t firstClusterLow;
    /**
     * 32-bit unsigned holding this file's size in bytes.
     */
    uint32_t fileSize;
  };

  /** Type name for directoryEntry */
  /** escape for name[0] = 0XE5 */
  static uint8_t const DIR_NAME_0XE5 = 0X05;
  /** name[0] value for entry that is free after being "deleted" */
  static uint8_t const DIR_NAME_DELETED = 0XE5;
  /** name[0] value for entry that is free and no allocated entries follow */
  static uint8_t const DIR_NAME_FREE = 0X00;

  /** file is read-only */
  static uint8_t const DIR_ATT_READ_ONLY = 0X01;
  /** File should hidden in directory listings */
  static uint8_t const DIR_ATT_HIDDEN = 0X02;
  /** Entry is for a system file */
  static uint8_t const DIR_ATT_SYSTEM = 0X04;
  /** Directory entry contains the volume label */
  static uint8_t const DIR_ATT_VOLUME_ID = 0X08;
  /** Entry is for a directory */
  static uint8_t const DIR_ATT_DIRECTORY = 0X10;
  /** Old DOS archive bit for backup support */
  static uint8_t const DIR_ATT_ARCHIVE = 0X20;
  /** Test value for long name entry */
  static uint8_t const DIR_ATT_LONG_NAME = 0X0F;
  /** Test mask for long name entry */
  static uint8_t const DIR_ATT_LONG_NAME_MASK = 0X3F;
  /** defined attribute bits */
  static uint8_t const DIR_ATT_DEFINED_BITS = 0X3F;
  /** Mask for file/subdirectory tests */
  static uint8_t const DIR_ATT_FILE_TYPE_MASK = (DIR_ATT_VOLUME_ID | DIR_ATT_DIRECTORY);
  static uint8_t const DIR_ATT_SKIP = (DIR_ATT_VOLUME_ID | DIR_ATT_DIRECTORY);

  /**
   * Directory entry is part of a long name
   */
  static inline uint8_t DIR_IS_LONG_NAME(const dir_t* dir)
  {
    return (dir->attributes & DIR_ATT_LONG_NAME_MASK) == DIR_ATT_LONG_NAME;
  }

  /**
   * Directory entry is for a file
   */
  static inline uint8_t DIR_IS_FILE(const dir_t* dir)
  {
    return (dir->attributes & DIR_ATT_FILE_TYPE_MASK) == 0;
  }

  /**
   * Directory entry is for a subdirectory
   */
  static inline uint8_t DIR_IS_SUBDIR(const dir_t* dir)
  {
    return (dir->attributes & DIR_ATT_FILE_TYPE_MASK) == DIR_ATT_DIRECTORY;
  }

  /**
   * Directory entry is for a file or subdirectory
   */
  static inline uint8_t DIR_IS_FILE_OR_SUBDIR(const dir_t* dir)
  {
    return (dir->attributes & DIR_ATT_VOLUME_ID) == 0;
  }

  /** FAT entry */
  typedef uint16_t fat_t;

  union cache16_t {
    /** Used to access cached file data blocks. */
    uint8_t data[512];
    /** Used to access cached FAT entries. */
    fat_t fat[256];
    /** Used to access cached directory entries. */
    dir_t dir[16];
    /** Used to access a cached Master Boot Record. */
    mbr_t mbr;
    /** Used to access to a cached FAT16 boot sector. */
    fbs_t fbs;
  };

  /**
   * FAT date representation support
   * Date Format. A FAT directory entry date stamp is a 16-bit field
   * that is basically a date relative to the MS-DOS epoch of
   * 01/01/1980. Here is the format (bit 0 is the LSB of the 16-bit
   * word, bit 15 is the MSB of the 16-bit word):
   *
   * Bits 0-4: Day of month, valid value range 1-31 inclusive.
   * Bits 5-8: Month of year, 1 = January, valid value range 1-12
   * inclusive.
   * Bits 9-15: Count of years from 1980, valid value range 0-127
   * inclusive (1980-2107).
   */
  union date_t {
    uint16_t as_uint16;
    struct {
      uint8_t day:5;
      uint8_t month:4;
      uint8_t year:7;
    };
    date_t(uint16_t y, uint8_t m, uint8_t d)
    {
      year = (y - 1980);
      month = m;
      day = d;
    }
    date_t(uint16_t date) { as_uint16 = date; }
    uint16_t YEAR() { return (1980 + year); }
    uint8_t MONTH() { return (month); }
    uint8_t DAY() { return (day); }
    operator uint16_t() { return (as_uint16); }
  };

  /** Output stream operator for FAT date value */
  friend IOStream& operator<<(IOStream& outs, date_t& date);

  /**
   * FAT Time Format. A FAT directory entry time stamp is a 16-bit
   * field that has a granularity of 2 seconds. Here is the format
   * (bit 0 is the LSB of the 16-bit word, bit 15 is the MSB of the
   * 16-bit word).
   *
   * Bits 0-4: 2-second count, valid value range 0-29 inclusive
   * (0-58 seconds).
   * Bits 5-10: Minutes, valid value range 0-59 inclusive.
   * Bits 11-15: Hours, valid value range 0-23 inclusive.
   *
   * The valid time range is from Midnight 00:00:00 to 23:59:58.
   */
  union time_t {
    uint16_t as_uint16;
    struct {
      uint8_t seconds:5;
      uint8_t minutes:6;
      uint8_t hours:5;
    };
    time_t(uint16_t h, uint8_t m, uint8_t s)
    {
      hours = h;
      minutes = m;
      seconds = s >> 1;
    }
    time_t(uint16_t now) { as_uint16 = now; }
    uint16_t HOURS() { return (hours); }
    uint8_t MINUTES() { return (minutes); }
    uint8_t SECONDS() { return (seconds << 1); }
    operator uint16_t() { return (as_uint16); }
  };

  /** Output stream operator for FAT time value */
  friend IOStream& operator<<(IOStream& outs, time_t& time);

  /** Default date for file timestamps is 1 Jan 2000 */
  static const uint16_t DEFAULT_DATE = ((2000 - 1980) << 9) | (1 << 5) | 1;

  /** Default time for file timestamp is 1 am */
  static const uint16_t DEFAULT_TIME = (1 << 11);

public:

  class File : public IOStream::Device {
  public:
    /**
     * Construct file access instance. Must be use open() before any
     * operation are possible.
     */
    File() : IOStream::Device(), m_flags(0) {}

    /**
     * Open a file by file name and mode flags. The file must be in
     * the root directory and must have a DOS 8.3 name. Returns true
     * if successful otherwise false for failure. Reasons for failure
     * include the FAT volume has not been initialized, a file is
     * already open, file name, is invalid, the file does not exist,
     * is a directory, or can't be opened in the access mode specified
     * by oflag.
     * @param[in] fileName a valid 8.3 DOS name for a file in the root.
     * @param[in] oflag mode of file open (create, read, write, etc).
     * @return bool.
     */
    bool open(const char* fileName, uint8_t oflag);

    /**
     * Checks the file's open/closed status.
     * @return the value true if a file is open otherwise false;
     */
    bool is_open(void) const { return (m_flags & O_RDWR) != 0; }

    /**
     * The sync() call causes all modified data and directory fields
     * to be written to the storage device.
     * @return true if successful otherwise false for failure. Reasons
     * for failure include a call to sync() before a file has been
     * opened or an I/O error.
     */
    bool sync();

    /**
     * Remove a file.  The directory entry and all data for the file
     * are deleted. This function should not be used to delete the 8.3
     * version of a file that has a long name. For example if a file
     * has the long name "New Text Document.txt" you should not delete
     * @return bool, true if successful otherwise false for
     * failure. Reasons for failure include the file is not open for
     * write or an I/O error occurred.
     */
    bool remove();

    /**
     * Close a file and force cached data and directory information to
     * be written to the storage device.
     * @return bool, true if successful otherwise false for
     * failure. Reasons for failure include the file is not open for
     * write or an I/O error occurred.
     */
    bool close()
    {
      if (!sync()) return false;
      m_flags = 0;
      return true;
    }

    /**
     * Sets the file's read/write position relative to mode.
     * @param[in] pos new position in bytes from given mode.
     * @param[in] mode absolute, relative and from end.
     * @return bool, true if successful otherwise false for
     * failure.
     */
    bool seek(uint32_t pos, uint8_t whence = SEEK_SET);

    /**
     * Return current position.
     */
    uint32_t tell() { return (m_curPosition); }

    /**
     * Rewind to the start of the file.
     */
    void rewind() { m_curPosition = m_curCluster = 0; }

    /**
     * Return number of bytes in file.
     */
    uint32_t size() { return (m_fileSize); }

    /**
     * Truncate a file to a specified length. The current file
     * position will be maintained if it is less than or equal to \a
     * length otherwise it will be set to end of file.
     * @param[in] length The desired length for the file.
     * @return bool, true if successful otherwise false for
     * failure. Reasons for failure include file is read only, file is
     * a directory, \a length is greater than the current file size or
     * an I/O error occurs.
     */
    bool truncate(uint32_t size);

    /**
     * @override{IOStream::Device}
     * Write character to the file.
     * @param[in] c character to write.
     * @return character written or EOF(-1).
     */
    virtual int putchar(char c);

    /** Overloaded virtual member function write. */
    using IOStream::Device::write;

    /**
     * @override{IOStream::Device}
     * Write data from buffer with given size to the file.
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @return number of bytes written or EOF(-1).
     */
    virtual int write(const void *buf, size_t size);

    /**
     * @override{IOStream::Device}
     * Read character from the file.
     * @return character or EOF(-1).
     */
    virtual int getchar();

    /** Overloaded virtual member function read. */
    using IOStream::Device::read;

    /**
     * @override{IOStream::Device}
     * Read data to given buffer with given size from the file.
     * @param[in] buf buffer to read into.
     * @param[in] size number of bytes to read.
     * @return number of bytes read or EOF(-1).
     */
    virtual int read(void* buf, size_t size);

  protected:
    uint8_t m_flags;          // see above for bit definitions
    int16_t m_dirEntryIndex;  // index of directory entry for open file
    fat_t m_firstCluster;     // first cluster of file
    uint32_t m_fileSize;      // fileSize
    fat_t m_curCluster;       // current cluster
    uint32_t m_curPosition;   // current byte offset

    static uint8_t isEOC(fat_t cluster) { return cluster >= 0XFFF8; }
    bool addCluster();
    bool freeChain(fat_t cluster);
    bool open(uint16_t entry, uint8_t oflag);
    bool dirEntry(dir_t* dir);
  };

  /**
   * The directory list function output selectors.
   */
  enum {
    LS_DATE = 1,
    LS_SIZE = 2
  } __attribute__((packed));

  /**
   * Initialize a FAT16 volume.
   * @param[in] dev SD device where the volume is located.
   * @param[in] part partition to be used. Legal values for \a part
   * are 1-4 to use the corresponding partition on a device formatted
   * with a MBR, Master Boot Record, or zero if the device is
   * formatted as a super floppy with the FAT boot sector in block zero.
   * @return The value one, true, is returned for success and the
   * value zero, false, is returned for failure.  reasons for failure
   * include not finding a valid FAT16 file system in the specified
   * partition, a call to begin() after a volume has been successful
   * initialized or an I/O error.
   */
  static bool begin(SD* sd, uint8_t partion);

  /**
   * Initialize a FAT16 volume. First try partition 1 then try super
   * floppy format.
   * @param[in] dev Sd device where the volume is located.
   * @return The value one, true, is returned for success and the
   * value zero, false, is returned for failure.  reasons for failure
   * include not finding a valid FAT16 file system, a call to begin()
   * after a volume has been successful initialized or an I/O error.
   *
   */
  static bool begin(SD* sd);

  /**
   * List directory contents to given iostream with selected
   * information.
   * @param[in] outs output stream.
   * @param[in] flags The inclusive OR of LS_DATE (file modification
   * date) and LS_SIZE (file size).
   */
  static void ls(IOStream& outs, uint8_t flags = 0);

  /**
   * Remove a file. The directory entry and all data for the file are
   * deleted.
   * @param[in] fileName name of the file to be removed.
   * @note this function should not be used to delete the 8.3 version
   * of a file that has a long name. For example if a file has the
   * long name "New Text Document.txt" you should not delete the 8.3
   * name "NEWTEX~1.TXT".
   * @return The value one, true, is returned for success and the
   * value zero, false, is returned for failure. Reasons for failure
   * include the file is read only, \a fileName is not found or an I/O
   * error occurred.
   */
  static bool rm(const char* fileName)
  {
    FAT16::File file;
    if (!file.open(fileName, O_WRITE)) return false;
    return file.remove();
  }

protected:
  // SD device (Fix: Should be an IOBlock::Device)
  static SD *device;

  // Volume info
  static bool volumeInitialized; 	// true if volume has been initialized
  static uint8_t fatCount;		// number of FATs
  static uint8_t blocksPerCluster;	// must be power of 2
  static uint16_t rootDirEntryCount;  	// should be 512 for FAT16
  static fat_t blocksPerFat;		// number of blocks in one FAT
  static fat_t clusterCount;		// total clusters in volume
  static uint32_t fatStartBlock;	// start of first FAT
  static uint32_t rootDirStartBlock;	// start of root dir
  static uint32_t dataStartBlock;	// start of data clusters

  // block cache
  static uint8_t const CACHE_FOR_READ  = 0;    // cache a block for read
  static uint8_t const CACHE_FOR_WRITE = 1;    // cache a block and set dirty
  static cache16_t cacheBuffer;		// 512 byte cache for raw blocks
  static uint32_t cacheBlockNumber; 	// Logical number of block in the cache
  static uint8_t cacheDirty;         	// cacheFlush() will write block if true
  static uint32_t cacheMirrorBlock;  	// mirror block for second FAT

  // callback function for date/time
  static void (*dateTime)(uint16_t* date, uint16_t* time);

  // define fields in flags_ require sync directory entry
  static uint8_t const F_OFLAG = O_RDWR | O_APPEND | O_SYNC;
  static uint8_t const F_FILE_DIR_DIRTY = 0X80;

  static bool make83Name(const char* str, uint8_t* name);

  static uint8_t blockOfCluster(uint32_t position)
  {
    return (position >> 9) & (blocksPerCluster - 1);
  }
  static uint16_t cacheDataOffset(uint32_t position)
  {
    return position & 0X1FF;
  }
  static dir_t* cacheDirEntry(uint16_t index, uint8_t action = 0);
  static uint8_t cacheRawBlock(uint32_t blockNumber, uint8_t action = 0);
  static uint8_t cacheFlush(void);
  static void cacheSetDirty(void)
  {
    cacheDirty |= CACHE_FOR_WRITE;
  }
  static uint32_t dataBlockLba(fat_t cluster, uint8_t blockOfCluster)
  {
    return (dataStartBlock +
	    (uint32_t)(cluster - 2) * blocksPerCluster +
	    blockOfCluster);
  }

  static bool fatGet(fat_t cluster, fat_t* value);
  static bool fatPut(fat_t cluster, fat_t value);

  static bool read(dir_t* dir, uint16_t* index, uint8_t skip = DIR_ATT_SKIP);
  static void printDirName(IOStream& outs, const dir_t& dir, uint8_t width);
};

#endif
