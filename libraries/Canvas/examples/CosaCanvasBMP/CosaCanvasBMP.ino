/**
 * @file CosaCanvasBMP.ino
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
 * Cosa demonstration of drawing a BMP image on Canvas. The test
 * bitmap (parrot.bmp) is 30X the size of the available SRAM on the
 * standard Arduino.
 *
 * @section Circuit
 * @code
 *                           ST7735
 *                       +------------+
 * (GND)---------------1-|GND         |
 * (VCC)---------------2-|VCC         |
 *                      -|            |
 * (RST)---------------6-|RESET       |
 * (D9)----------------7-|A0          |
 * (MOSI/D11)----------8-|SDA         |
 * (SCK/D13)-----------9-|SCK         |
 * (SS/D10)-----------10-|CS          |
 *                      -|            |
 * (VCC)----[330]-----15-|LED+        |
 * (GND)--------------16-|LED-        |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <SD.h>
#include <FAT16.h>
#include <Canvas.h>
#include <GDDRAM.h>
#include <ST7735.h>

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/UART.hh"

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
    uint8_t blue;
    uint8_t green;
    uint8_t red;

    operator Canvas::color16_t()
    {
      return (Canvas::color16_t(red,green,blue));
    }
  };

  static const uint16_t SIGNATURE = 0x4d42;

  class File : public Canvas::Image {
  public:
    /**
     * Open given bitmap file for drawing on canvas. Return true(1) if
     * successful otherwise false(0).
     * @param[in] filename.
     * @return bool.
     */
    bool open(const char* filename);

    /**
     * Close bitmap file. Return true(1) if successful otherwise
     * false(0).
     * @return bool.
     */
    bool close();

    /**
     * @override Canvas::Image
     * Read the given number of pixel into the given buffer.
     * Return true(1) if successful otherwise false(0).
     * @param[in] buf pixel buffer pointer.
     * @param[in] count number of pixels to read.
     * @return bool.
     */
    virtual bool read(Canvas::color16_t* buf, size_t count);

  private:
    FAT16::File m_file;
  };
};

bool
BMP::File::open(const char* filename)
{
  // Open the file for reading
  if (!m_file.open(filename, O_READ)) return (false);

  // Read the file header and check signature
  BMP::file_header_t file_header;
  if ((m_file.read(&file_header, sizeof(file_header)) != sizeof(file_header))
      || (file_header.signature != BMP::SIGNATURE))
    return (false);

  // Read the image header and check details
  BMP::image_header_t image_header;
  if ((m_file.read(&image_header, sizeof(image_header)) != sizeof(image_header))
      || (image_header.header_size != sizeof(image_header))
      || (image_header.color_planes != 1)
      || (image_header.bits_per_pixel != 24)
      || (image_header.compression_method != 0))
    return (false);

  // Set canvas image dimensions
  WIDTH = image_header.image_width;
  HEIGHT = image_header.image_height;
  return (true);
}

bool
BMP::File::close()
{
  return (m_file.close());
}

bool
BMP::File::read(Canvas::color16_t* buf, size_t count)
{
  // Read the next batch of pixels and translate to canvas color
  color24_t tmp[count];
  if (!m_file.read(tmp, count * sizeof(color24_t))) return (false);
  for (size_t i = 0; i < count; i++) buf[i] = tmp[i];
  return (true);
}

SD sd(Board::D8);
ST7735 tft;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaCanvasBMP: started"));
  Watchdog::begin();
  RTT::begin();
  ASSERT(sd.begin(SPI::DIV2_CLOCK));
  ASSERT(FAT16::begin(&sd));
  ASSERT(tft.begin());
}

void loop()
{
  uint32_t pixels = 0L;
  BMP::File image;

  MEASURE("fill screen:", 1) {
    tft.set_canvas_color(Canvas::BLUE);
    tft.fill_screen();
  }

  MEASURE("open image file:", 1) {
    ASSERT(image.open("PARROT.BMP"));
  }
  INFO("image width: %ud", image.WIDTH);
  INFO("image height: %ud", image.HEIGHT);
  INFO("image pixels: %ud", pixels = (image.WIDTH * image.HEIGHT));

  MEASURE("draw image:", 1) {
    tft.draw_image(0, 0, &image);
  }
  INFO("draw pixel: %ul us", trace.measure / pixels);
  INFO("transfer rate: %ul KBps", 2000L * pixels / trace.measure);

  MEASURE("close image file:", 1) {
    ASSERT(image.close());
  }

  ASSERT(true == false);
}
