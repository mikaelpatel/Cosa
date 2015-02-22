/**
 * @file Cosa/FS.hh
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

#ifndef COSA_FS_HH
#define COSA_FS_HH

/**
 * File System file open modes; use one or many.
 */
enum {
  O_READ = 0X01,		//!< Open for reading.
  O_RDONLY = O_READ, 		//!< Same as O_READ.
  O_WRITE = 0X02, 		//!< Open for write.
  O_WRONLY = O_WRITE, 		//!< Same as O_WRITE.
  O_RDWR = O_READ | O_WRITE, 	//!< Open for reading and writing.
  O_APPEND = 0X04, 		//!< The file offset shall be set to the
				//!< end of the file prior to each write.
  O_SYNC = 0X08,		//!< Synchronous writes.
  O_CREAT = 0X10,		//!< Create the file if nonexistent.
  O_EXCL = 0X20,		//!< If O_CREAT and O_EXCL are set,
				//!< open() shall fail if the file
				//!< exists.
  O_TRUNC = 0X40		//!< Truncate the file to zero length.
} __attribute__((packed));

/**
 * File System file seek mode; use one of.
 */
enum {
  SEEK_SET = 0,			//!< Absolute position.
  SEEK_CUR = 1,			//!< Relative to current position.
  SEEK_END = 2			//!< Relative to end of file.
} __attribute__((packed));

#endif
