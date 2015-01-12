/**
 * @file CosaBMP.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * @section Description
 * Short demo, test and benchmark of the BMP file on SD.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/SPI/Driver/SD.hh"
#include "Cosa/FS/FAT16.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

//#define USE_SD_ADAPTER
//#define USE_ETHERNET_SHIELD
#define USE_TFT_ST7735

#if defined(USE_ETHERNET_SHIELD)
SD sd(Board::D4);
OutputPin eth(Board::D10, 1);
#endif

#if defined(USE_TFT_ST7735) || defined(USE_SD_ADAPTER)
SD sd(Board::D8);
#endif

#define SLOW_CLOCK SPI::DIV4_CLOCK
#define FAST_CLOCK SPI::DIV2_CLOCK
#define CLOCK FAST_CLOCK

class BMP {
public:
  struct file_header_t {
    uint16_t signature;
    uint32_t file_size;
    uint16_t reserved[2];
    uint32_t image_offset;
  };
  struct image_header_t {
    uint32_t header_size;
    uint32_t image_width;
    uint32_t image_height;
    uint16_t color_planes;
    uint16_t bits_per_pixel;
    uint32_t compression_method;
    uint32_t image_size;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    uint32_t colors_in_palette;
    uint32_t important_colors;
  };
  struct color24_t {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };
  static const uint16_t SIGNATURE = 0x4d42;
};

IOStream& operator<<(IOStream& outs, BMP::file_header_t& header)
{
  outs << PSTR("signature: ") << hex << header.signature << endl;
  outs << PSTR("file_size: ") << header.file_size << endl;
  outs << PSTR("image_offset: ") << header.image_offset << endl;
  return (outs);
}

IOStream& operator<<(IOStream& outs, BMP::image_header_t& header)
{
  outs << PSTR("header_size: ") << header.header_size << endl;
  outs << PSTR("image_width: ") << header.image_width << endl;
  outs << PSTR("image_height: ") << header.image_height << endl;
  outs << PSTR("color_planes: ") << header.color_planes << endl;
  outs << PSTR("bits_per_pixel: ") << header.bits_per_pixel << endl;
  outs << PSTR("compression_method: ") << header.compression_method << endl;
  outs << PSTR("image_size: ") << header.image_size << endl;
  outs << PSTR("horizontal_resolution: ") << header.horizontal_resolution << endl;
  outs << PSTR("vertical_resolution: ") << header.vertical_resolution << endl;
  outs << PSTR("colors_in_palette: ") << header.colors_in_palette << endl;
  outs << PSTR("important_colors: ") << header.important_colors << endl;
  return (outs);
}

IOStream& operator<<(IOStream& outs, BMP::color24_t pixel)
{
  outs << '<' 
       << pixel.red << '.'
       << pixel.green << '.'
       << pixel.blue
       << '>';
  return (outs);
}

void setup()
{
  Watchdog::begin();
  RTC::begin();
  uart.begin(115200);
  trace.begin(&uart, PSTR("CosaBMP: started"));
  TRACE(free_memory());
  trace.println();

  ASSERT(sd.begin(CLOCK));
  ASSERT(FAT16::begin(&sd));
}

void loop()
{
  FAT16::File file;
  // ASSERT(file.open("FLOWERS.BMP", O_READ));
  ASSERT(file.open("PARROT.BMP", O_READ));

  INFO("Read file header and verify", 0);
  BMP::file_header_t file_header;
  ASSERT(file.read(&file_header, sizeof(file_header)) == sizeof(file_header));
  ASSERT(file_header.signature == BMP::SIGNATURE);
  trace << file_header << endl;

  INFO("Read image header and verify", 0);
  BMP::image_header_t image_header;
  ASSERT(file.read(&image_header, sizeof(image_header)) == sizeof(image_header));
  ASSERT(image_header.header_size == sizeof(image_header));
  ASSERT(image_header.color_planes == 1);
  ASSERT(image_header.bits_per_pixel == 24);
  ASSERT(image_header.compression_method == 0);
  uint32_t image_size = image_header.image_width * image_header.image_height * sizeof(BMP::color24_t);
  TRACE(image_size);
  trace << image_header << endl;

  INFO("Read some pixels and dump in hex", 0);
  BMP::color24_t image_buffer[32];
  ASSERT(file.read(image_buffer, sizeof(image_buffer)) == sizeof(image_buffer));
  trace.print(image_buffer, sizeof(image_buffer), 
	      IOStream::hex, sizeof(BMP::color24_t) * 8);
  trace << endl;

  INFO("Print pixels using output operator", 0);
  for (uint16_t i = 0; i < membersof(image_buffer); i++)
    trace << i << ':' << image_buffer[i] << endl;
  trace << endl;

  INFO("Cleanup and terminate", 0);
  ASSERT(file.close());
  ASSERT(sd.end());
  ASSERT(true == false);
}

