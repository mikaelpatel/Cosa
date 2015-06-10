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

#include "FAT16.hh"

bool
FAT16::make83Name(const char* str, uint8_t* name)
{
  uint8_t c;
  uint8_t n = 7;
  uint8_t i = 0;
  while (i < 11) name[i++] = ' ';
  i = 0;
  while ((c = *str++) != '\0') {
    if (c == '.') {
      if (n == 10) return (false);
      n = 10;
      i = 8;
    }
    else {
      PGM_P p = (PGM_P) PSTR("|<>^+=?/[];,*\"\\");
      uint8_t b;
      while ((b = pgm_read_byte(p++))) if (b == c) return (false);
      if (i > n || c < 0X21 || c > 0X7E) return (false);
      name[i++] = c < 'a' || c > 'z' ?  c : c + ('A' - 'a');
    }
  }
  return (name[0] != ' ');
}

SD *FAT16::device = NULL;

bool FAT16::volumeInitialized = 0;
uint8_t FAT16::fatCount;
uint8_t FAT16::blocksPerCluster;
uint16_t FAT16::rootDirEntryCount;
FAT16::fat_t FAT16::blocksPerFat;
FAT16::fat_t FAT16::clusterCount;
uint32_t FAT16::fatStartBlock;
uint32_t FAT16::rootDirStartBlock;
uint32_t FAT16::dataStartBlock;

uint32_t FAT16::cacheBlockNumber = 0XFFFFFFFF;
FAT16::cache16_t FAT16::cacheBuffer;
uint8_t FAT16::cacheDirty = 0;
uint32_t FAT16::cacheMirrorBlock = 0;
void (*FAT16::dateTime)(uint16_t* date, uint16_t* time) = NULL;

bool
FAT16::begin(SD* sd, uint8_t part)
{
  // Error if invalid partition
  if (UNLIKELY(part > 4)) return (false);
  device = sd;
  uint32_t volumeStartBlock = 0;

  // If part == 0 assume super floppy with FAT16 boot sector in block zero
  // If part > 0 assume mbr volume with partition table
  if (part) {
    if (!cacheRawBlock(volumeStartBlock)) return (false);
    volumeStartBlock = cacheBuffer.mbr.part[part - 1].firstSector;
  }
  if (!cacheRawBlock(volumeStartBlock)) return (false);

  // Check boot block signature
  if (cacheBuffer.data[510] != BOOTSIG0 ||
      cacheBuffer.data[511] != BOOTSIG1) return (false);

  bpb_t* bpb = &cacheBuffer.fbs.bpb;
  fatCount = bpb->fatCount;
  blocksPerCluster = bpb->sectorsPerCluster;
  blocksPerFat = bpb->sectorsPerFat16;
  rootDirEntryCount = bpb->rootDirEntryCount;
  fatStartBlock = volumeStartBlock + bpb->reservedSectorCount;
  rootDirStartBlock = fatStartBlock + bpb->fatCount*bpb->sectorsPerFat16;
  dataStartBlock = rootDirStartBlock + ((32*bpb->rootDirEntryCount + 511)/512);
  uint32_t totalBlocks = bpb->totalSectors16 ? bpb->totalSectors16 : bpb->totalSectors32;
  clusterCount = (totalBlocks - (dataStartBlock - volumeStartBlock)) / bpb->sectorsPerCluster;

  // Check valid FAT16 volume
  if ((bpb->bytesPerSector != 512)	 // Only allow 512 byte blocks
      || (bpb->sectorsPerFat16 == 0)     // Zero for FAT32
      || (clusterCount < 4085)           // FAT12 if true
      || (totalBlocks > 0X800000)	 // Max size for FAT16 volume
      || (bpb->reservedSectorCount == 0) // invalid volume
      || (bpb->fatCount == 0)		 // invalid volume
      || (bpb->sectorsPerFat16 < (clusterCount >> 8))
      || (bpb->sectorsPerCluster == 0)
      || (bpb->sectorsPerCluster & (bpb->sectorsPerCluster - 1))) {
    return (false);
  }
  volumeInitialized = true;
  return (true);
}

bool
FAT16::begin(SD* sd)
{
  return (begin(sd, 1) || begin(sd, 0));
}

bool
FAT16::File::sync(void)
{
  if (m_flags & F_FILE_DIR_DIRTY) {
    // Cache directory entry
    dir_t* d = cacheDirEntry(m_dirEntryIndex, CACHE_FOR_WRITE);
    if (!d) return (false);

    // Update file size and first cluster
    d->fileSize = m_fileSize;
    d->firstClusterLow = m_firstCluster;

    // Set modify time if user supplied a callback date/time function
    if (dateTime) {
      dateTime(&d->lastWriteDate, &d->lastWriteTime);
      d->lastAccessDate = d->lastWriteDate;
    }
    m_flags &= ~F_FILE_DIR_DIRTY;
  }
  return (cacheFlush());
}

bool
FAT16::File::open(const char* fileName, uint8_t oflag)
{
  uint8_t dname[11];   // name formated for dir entry
  int16_t empty = -1;  // index of empty slot
  dir_t* p;            // pointer to cached dir entry

  if (!volumeInitialized || is_open()) return (false);

  // Check valid 8.3 file name
  if (!make83Name(fileName, dname)) return (false);

  for (uint16_t index = 0; index < rootDirEntryCount; index++) {
    if (!(p = cacheDirEntry(index))) return (false);
    if (p->name[0] == DIR_NAME_FREE || p->name[0] == DIR_NAME_DELETED) {
      // Remember first empty slot
      if (empty < 0) empty = index;
      // Done if no entries follow
      if (p->name[0] == DIR_NAME_FREE) break;
    }
    else if (!memcmp(dname, p->name, 11)) {
      // Don't open existing file if O_CREAT and O_EXCL
      if ((oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) return (false);
      // Open existing file
      return (open(index, oflag));
    }
  }

  // Error if directory is full
  if (empty < 0) return (false);

  // Only create file if O_CREAT and O_WRITE
  if ((oflag & (O_CREAT | O_WRITE)) != (O_CREAT | O_WRITE)) return (false);
  if (!(p = cacheDirEntry(empty, CACHE_FOR_WRITE))) return (false);

  // Initialize as empty file
  memset(p, 0, sizeof(dir_t));
  memcpy(p->name, dname, 11);

  // Set timestamps with user function or use default date/time
  if (dateTime) {
    dateTime(&p->creationDate, &p->creationTime);
  } else {
    p->creationDate = DEFAULT_DATE;
    p->creationTime = DEFAULT_TIME;
  }
  p->lastAccessDate = p->creationDate;
  p->lastWriteDate = p->creationDate;
  p->lastWriteTime = p->creationTime;

  // Force created directory entry will be written to storage device
  if (!cacheFlush()) return (false);

  // Open entry
  return (open(empty, oflag));
}

bool
FAT16::File::open(uint16_t index, uint8_t oflag)
{
  if (!volumeInitialized || is_open()) return (false);
  if ((oflag & O_TRUNC) && !(oflag & O_WRITE)) return (false);
  dir_t* d = cacheDirEntry(index);
  // If bad file index or I/O error
  if (!d) return (false);
  // Error if unused entry
  if (d->name[0] == DIR_NAME_FREE || d->name[0] == DIR_NAME_DELETED)
    return (false);
  // Error if long name, volume label or subdirectory
  if ((d->attributes & (DIR_ATT_VOLUME_ID | DIR_ATT_DIRECTORY)) != 0)
    return (false);
  // Don't allow write or truncate if read-only
  if ((d->attributes & DIR_ATT_READ_ONLY) && (oflag & (O_WRITE | O_TRUNC)))
    return (false);

  m_curCluster = 0;
  m_curPosition = 0;
  m_dirEntryIndex = index;
  m_fileSize = d->fileSize;
  m_firstCluster = d->firstClusterLow;
  m_flags = oflag & (O_RDWR | O_SYNC | O_APPEND);
  if (oflag & O_TRUNC) return (truncate(0));
  return (true);
}

bool
FAT16::File::remove(void)
{
  // Error if file is not open for write
  if (!(m_flags & O_WRITE)) return (false);
  if (m_firstCluster && !freeChain(m_firstCluster)) return (false);
  dir_t* d = cacheDirEntry(m_dirEntryIndex, CACHE_FOR_WRITE);
  if (!d) return (false);
  d->name[0] = DIR_NAME_DELETED;
  m_flags = 0;
  return (cacheFlush());
}

int
FAT16::File::getchar()
{
  uint8_t res;
  return (read(&res, sizeof(res)) == sizeof(res) ? res : -1);
}

int
FAT16::File::read(void* buf, size_t nbyte)
{
  // Convert void pointer to uin8_t pointer
  uint8_t* dst = reinterpret_cast<uint8_t*>(buf);

  // Error if not open for read
  if (!(m_flags & O_READ)) return (IOStream::EOF);

  // Don't read beyond end of file
  if ((m_curPosition + nbyte) > m_fileSize) nbyte = m_fileSize - m_curPosition;

  // Bytes left to read in loop
  uint16_t nToRead = nbyte;
  while (nToRead > 0) {
    uint8_t blkOfCluster = blockOfCluster(m_curPosition);
    uint16_t blockOffset = cacheDataOffset(m_curPosition);
    if (blkOfCluster == 0 && blockOffset == 0) {
      // Start next cluster
      if (m_curCluster == 0) {
        m_curCluster = m_firstCluster;
      } else {
        if (!fatGet(m_curCluster, &m_curCluster)) return (IOStream::EOF);
      }
      // Return error if bad cluster chain
      if (m_curCluster < 2 || isEOC(m_curCluster)) return (IOStream::EOF);
    }

    // Cache data block
    if (!cacheRawBlock(dataBlockLba(m_curCluster, blkOfCluster)))
      return (IOStream::EOF);

    // Location of data in cache
    uint8_t* src = cacheBuffer.data + blockOffset;

    // Max number of byte available in block
    uint16_t n = 512 - blockOffset;

    // Lesser of available and amount to read
    if (n > nToRead) n = nToRead;

    // Copy data to caller
    memcpy(dst, src, n);

    m_curPosition += n;
    dst += n;
    nToRead -= n;
  }
  return (nbyte);
}

int
FAT16::File::write(const void* buf, size_t nbyte)
{
  uint16_t nToWrite = nbyte;
  const uint8_t* src = reinterpret_cast<const uint8_t*>(buf);

  // Error if file is not open for write
  if (!(m_flags & O_WRITE)) return (IOStream::EOF);

  // Go to end of file if O_APPEND
  if ((m_flags & O_APPEND) && m_curPosition != m_fileSize) {
    if (!seek(0, SEEK_END)) return (IOStream::EOF);
  }

  while (nToWrite > 0) {
    uint8_t blkOfCluster = blockOfCluster(m_curPosition);
    uint16_t blockOffset = cacheDataOffset(m_curPosition);
    if (blkOfCluster == 0 && blockOffset == 0) {
      // Start of new cluster
      if (m_curCluster == 0) {
        if (m_firstCluster == 0) {
          // Allocate first cluster of file
          if (!addCluster()) return (IOStream::EOF);
        } else {
          m_curCluster = m_firstCluster;
        }
      } else {
        fat_t next;
        if (!fatGet(m_curCluster, &next)) return (IOStream::EOF);
        if (isEOC(next)) {
          // Add cluster if at end of chain
          if (!addCluster()) return (IOStream::EOF);
        } else {
          m_curCluster = next;
        }
      }
    }
    uint32_t lba = dataBlockLba(m_curCluster, blkOfCluster);
    if (blockOffset == 0 && m_curPosition >= m_fileSize) {
      // Start of new block don't need to read into cache
      if (!cacheFlush()) return (IOStream::EOF);
      cacheBlockNumber = lba;
      cacheSetDirty();
    } else {
      // Rewrite part of block
      if (!cacheRawBlock(lba, CACHE_FOR_WRITE)) return (IOStream::EOF);
    }
    uint8_t* dst = cacheBuffer.data + blockOffset;

    // Max space in block
    uint16_t n = 512 - blockOffset;

    // Lesser of space and amount to write
    if (n > nToWrite) n = nToWrite;

    // Copy data to cache
    memcpy(dst, src, n);

    m_curPosition += n;
    nToWrite -= n;
    src += n;
  }
  if (m_curPosition > m_fileSize) {
    // Update fileSize and insure sync will update dir entry
    m_fileSize = m_curPosition;
    m_flags |= F_FILE_DIR_DIRTY;
  }
  else if (dateTime && nbyte) {
    // Force sync will update modified date and time
    m_flags |= F_FILE_DIR_DIRTY;
  }

  if (m_flags & O_SYNC) {
    if (!sync()) return (IOStream::EOF);
  }
  return (nbyte);
}

int
FAT16::File::putchar(char c)
{
  return (write(&c, sizeof(c)));
}

bool
FAT16::File::seek(uint32_t pos, uint8_t whence)
{
  if (whence == SEEK_CUR) pos += m_curPosition;
  else if (whence == SEEK_END) pos = m_fileSize;
  else if (whence != SEEK_SET) return (false);

  // Error if file not open or seek past end of file
  if (!is_open() || pos > m_fileSize) return (false);
  if (pos == 0) {
    // Set position to start of file
    m_curCluster = 0;
    m_curPosition = 0;
    return (true);
  }
  fat_t n = ((pos - 1) >> 9) / blocksPerCluster;
  if (pos < m_curPosition || m_curPosition == 0) {
    // Must follow chain from first cluster
    m_curCluster = m_firstCluster;
  } else {
    // Advance from curPosition
    n -= ((m_curPosition - 1) >> 9)/blocksPerCluster;
  }
  while (n--) {
    if (!fatGet(m_curCluster, &m_curCluster)) return (false);
  }
  m_curPosition = pos;
  return (true);
}

bool
FAT16::File::truncate(uint32_t length)
{
  // Error if file is not open for write
  if (!(m_flags & O_WRITE)) return (false);

  if (length > m_fileSize) return (false);

  // Filesize and length are zero - nothing to do
  if (m_fileSize == 0) return (true);
  uint32_t newPos = m_curPosition > length ? length : m_curPosition;
  if (length == 0) {
    // Free all clusters
    if (!freeChain(m_firstCluster)) return (false);
    m_curCluster = m_firstCluster = 0;
  }
  else {
    fat_t toFree;
    if (!seek(length)) return (false);
    if (!fatGet(m_curCluster, &toFree)) return (false);
    if (!isEOC(toFree)) {
      // Free extra clusters
      if (!fatPut(m_curCluster, EOC16)) return (false);
      if (!freeChain(toFree)) return (false);
    }
  }
  m_fileSize = length;
  m_flags |= F_FILE_DIR_DIRTY;
  if (!sync()) return (false);
  return seek(newPos);
}

bool
FAT16::File::dirEntry(FAT16::dir_t* dir)
{
  if (!sync()) return (false);
  dir_t* p = cacheDirEntry(m_dirEntryIndex, CACHE_FOR_WRITE);
  if (!p) return (false);
  memcpy(dir, p, sizeof(dir_t));
  return (true);
}

bool
FAT16::read(dir_t* dir, uint16_t* index, uint8_t skip)
{
  dir_t* p;
  for (uint16_t i = *index; ; i++) {
    if (i >= rootDirEntryCount) return (false);
    if (!(p = cacheDirEntry(i))) return (false);

    // Done if beyond last used entry
    if (p->name[0] == DIR_NAME_FREE) return (false);

    // Skip deleted entry
    if (p->name[0] == DIR_NAME_DELETED) continue;

    // Skip long names
    if ((p->attributes & DIR_ATT_LONG_NAME_MASK) == DIR_ATT_LONG_NAME) continue;

    // Skip if attribute match
    if (p->attributes & skip) continue;

    // Return found index
    *index = i;
    break;
  }
  memcpy(dir, p, sizeof(dir_t));
  return (true);
}

void
FAT16::printDirName(IOStream& outs, const dir_t& dir, uint8_t width)
{
  uint8_t w = 0;
  for (uint8_t i = 0; i < 11; i++) {
    if (dir.name[i] == ' ') continue;
    if (i == 8) {
      outs << '.';
      w++;
    }
    outs << (char) dir.name[i];
    w++;
  }
  if (DIR_IS_SUBDIR(&dir)) {
    outs << '/';
    w++;
  }
  while (w < width) {
    outs << ' ';
    w++;
  }
}

void
FAT16::ls(IOStream& outs, uint8_t flags)
{
  dir_t d;
  for (uint16_t index = 0; read(&d, &index, DIR_ATT_VOLUME_ID); index++) {
    // Print file name with possible blank fill
    printDirName(outs, d, flags & (LS_DATE | LS_SIZE) ? 14 : 0);

    // Print modify date/time if requested
    if (flags & LS_DATE) {
      date_t date(d.lastWriteDate);
      time_t time(d.lastWriteTime);
      outs << date << ' ' << time;
    }

    // Print size if requested
    if (DIR_IS_FILE(&d) && (flags & LS_SIZE)) {
      outs << ' ' << d.fileSize;
    }
    outs << endl;
  }
}

bool
FAT16::File::addCluster()
{
  // Start search after last cluster of file or at cluster two in FAT
  fat_t freeCluster = m_curCluster ? m_curCluster : 1;

  for (fat_t i = 0; ; i++) {
    // Return no free clusters
    if (i >= clusterCount) return (false);
    // Fat has clusterCount + 2 entries
    if (freeCluster > clusterCount) freeCluster = 1;
    freeCluster++;
    fat_t value;
    if (!fatGet(freeCluster, &value)) return (false);
    if (value == 0) break;
  }

  // Mark cluster allocated
  if (!fatPut(freeCluster, EOC16)) return (false);

  if (m_curCluster != 0) {
    // Link cluster to chain
    if (!fatPut(m_curCluster, freeCluster)) return (false);
  } else {
    // first cluster of file so update directory entry
    m_flags |= F_FILE_DIR_DIRTY;
    m_firstCluster = freeCluster;
  }
  m_curCluster = freeCluster;
  return (true);
}

FAT16::dir_t*
FAT16::cacheDirEntry(uint16_t index, uint8_t action)
{
  if (index >= rootDirEntryCount) return NULL;
  if (!cacheRawBlock(rootDirStartBlock + (index >> 4), action)) return NULL;
  return &cacheBuffer.dir[index & 0XF];
}

uint8_t
FAT16::cacheFlush(void)
{
  if (cacheDirty) {
    if (!device->write(cacheBlockNumber, cacheBuffer.data)) {
      return (false);
    }
    if (cacheMirrorBlock) {
      if (!device->write(cacheMirrorBlock, cacheBuffer.data)) {
        return (false);
      }
      cacheMirrorBlock = 0;
    }
    cacheDirty = 0;
  }
  return (true);
}

uint8_t
FAT16::cacheRawBlock(uint32_t blockNumber, uint8_t action)
{
  if (cacheBlockNumber != blockNumber) {
    if (!cacheFlush()) return (false);
    if (!device->read(blockNumber, cacheBuffer.data)) return (false);
    cacheBlockNumber = blockNumber;
  }
  cacheDirty |= action;
  return (true);
}

bool
FAT16::fatGet(fat_t cluster, fat_t* value)
{
  if (cluster > (clusterCount + 1)) return (false);
  uint32_t lba = fatStartBlock + (cluster >> 8);
  if (lba != cacheBlockNumber) {
    if (!cacheRawBlock(lba)) return (false);
  }
  *value = cacheBuffer.fat[cluster & 0XFF];
  return (true);
}

bool
FAT16::fatPut(fat_t cluster, fat_t value)
{
  if (cluster < 2) return (false);
  if (cluster > (clusterCount + 1)) return (false);
  uint32_t lba = fatStartBlock + (cluster >> 8);
  if (lba != cacheBlockNumber) {
    if (!cacheRawBlock(lba)) return (false);
  }
  cacheBuffer.fat[cluster & 0XFF] = value;
  cacheSetDirty();
  if (fatCount > 1) cacheMirrorBlock = lba + blocksPerFat;
  return (true);
}

bool
FAT16::File::freeChain(fat_t cluster)
{
  while (1) {
    fat_t next;
    if (!fatGet(cluster, &next)) return (false);
    if (!fatPut(cluster, 0)) return (false);
    if (isEOC(next)) return (true);
    cluster = next;
  }
}

IOStream&
operator<<(IOStream& outs, FAT16::date_t& date)
{
  outs << date.YEAR() << '-';
  uint8_t month = date.MONTH();
  if (month < 9) outs << '0';
  outs << month << '-';
  uint8_t day = date.DAY();
  if (day < 9) outs << '0';
  outs << day;
  return (outs);
}

IOStream&
operator<<(IOStream& outs, FAT16::time_t& time)
{
  uint8_t hours = time.HOURS();
  if (hours < 9) outs << '0';
  outs << hours << ':';
  uint8_t minutes = time.MINUTES();
  if (minutes < 9) outs << '0';
  outs << minutes << ':';
  uint8_t seconds = time.SECONDS();
  if (seconds < 9) outs << '0';
  outs << seconds;
  return (outs);
}

