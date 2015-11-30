/**
 * @file CosaFAT16logger.ino
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
 * Simple FAT16/SD data logger; analog samples in text or binary
 * format.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <SD.h>
#include <FAT16.h>

#include "Cosa/AnalogPin.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Time.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"

// Configuration: Storage, SD adapter/shield, CSV separator
//#define USE_TEXT_FORMAT
#define USE_BINARY_FORMAT

//#define USE_SD_ADAPTER
#define USE_SD_DATA_LOGGING_SHIELD
//#define USE_ETHERNET_SHIELD
//#define USE_TFT_ST7735

//#define CSV ','
#define CSV ';'

#define SAMPLE_MAX membersof(analog_pin_map)
//#define SAMPLE_MAX 3

#if defined(WICKEDDEVICE_WILDFIRE) || defined(USE_SD_ADAPTER)
SD sd;

#elif defined(USE_ETHERNET_SHIELD)
SD sd(Board::D4);
OutputPin eth(Board::D10, 1);

#elif defined(USE_TFT_ST7735)
SD sd;
OutputPin tft(Board::D10, 1);

#elif defined(USE_SD_DATA_LOGGING_SHIELD)
#include <DS1307.h>
SD sd(Board::D10);
DS1307 rtc;
#endif

// Configuration: SD SPI clock
#define SLOW_CLOCK SPI::DIV4_CLOCK
#define FAST_CLOCK SPI::DIV2_CLOCK
#define CLOCK FAST_CLOCK

// Number of samples
const uint16_t SAMPLES = 1000;

// Time period between samples
const uint16_t PERIOD = 16;

// Sample rate
const uint16_t RATE = (1000 + (PERIOD/2)) / PERIOD;

// Timestamp; seconds and fraction in milli-seconds
struct timestamp_t {
  clock_t sec;
  uint16_t ms;
};

// Log entry with timestamp and analog pin samples
struct entry_t {
  timestamp_t timestamp;
  uint16_t sample[SAMPLE_MAX];
};

// Wall clock (seconds)
RTT::Clock clock;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaFAT16logger: started"));
  trace.flush();
  Watchdog::begin();
  RTT::begin();
  ASSERT(sd.begin(CLOCK));
  ASSERT(FAT16::begin(&sd));

#if defined(USE_SD_DATA_LOGGING_SHIELD)
  time_t now;
  ASSERT(rtc.get_time(now));
  now.to_binary();
  clock.time(now);
  RTT::millis(0L);
  trace << PSTR("Date: ") << now << endl;
  trace.flush();
#endif
}

void loop()
{
  static uint16_t err = 0;
  uint32_t start, stop, ms, max;
  FAT16::File file;
  IOStream cout(&file);
  entry_t entry;

  // Some configuration statistics
  trace << PSTR("Period: ") << PERIOD << PSTR(" ms")
	<< endl;
  trace << PSTR("Rate: ") << RATE << PSTR(" entries/second, ")
	<< SAMPLE_MAX * RATE
	<< PSTR(" values/second")
	<< endl;
  trace << PSTR("Samples: ") << SAMPLE_MAX << PSTR(" values per entry")
	<< endl;
  trace << PSTR("Log: ") << SAMPLES << PSTR(" entries, ")
	<< SAMPLES * SAMPLE_MAX << PSTR(" values")
	<< endl;
    trace << flush;

  // Open log file and write header (text format)
  ASSERT(file.open("LOG.CSV", O_TRUNC | O_WRITE | O_CREAT));
#if defined(USE_TEXT_FORMAT)
  cout << PSTR("Timestamp") << CSV << PSTR("ms");
  for (uint8_t i = 0; i < SAMPLE_MAX; i++)
    cout << CSV << 'A' << i;
  cout << endl;
#endif

  // Sample and write entries to log file
  max = 0;
  for (uint16_t j = 0; j < SAMPLES; j++) {
    start = RTT::millis();
    entry.timestamp.sec = clock.time();
    entry.timestamp.ms = start % 1000;
    AnalogPin::powerup();
    for (uint8_t i = 0; i < SAMPLE_MAX; i++) {
      Board::AnalogPin pin;
      pin = (Board::AnalogPin) pgm_read_byte(analog_pin_map + i);
      entry.sample[i] = AnalogPin::sample(pin);
    }
    AnalogPin::powerdown();
#if defined(USE_TEXT_FORMAT)
    cout << entry.timestamp.sec << CSV << entry.timestamp.ms;
    for (uint8_t i = 0; i < SAMPLE_MAX; i++)
      cout << CSV << entry.sample[i];
    cout << endl;
#endif
#if defined(USE_BINARY_FORMAT)
    file.write(&entry, sizeof(entry));
#endif
    stop = RTT::millis();
    ms = stop - start + 1;
    if (ms > max) max = ms;
    if (PERIOD > ms) delay(PERIOD - ms); else err += 1;
  }
  ASSERT(file.close());

  // Reopen log and print contents
  ASSERT(file.open("LOG.CSV", O_READ));
#if defined(USE_TEXT_FORMAT)
  int c;
  while ((c = file.getchar()) != IOStream::EOF) trace << (char) c;
#endif
#if defined(USE_BINARY_FORMAT)
  trace << PSTR("Timestamp") << CSV << PSTR("ms");
  for (uint8_t i = 0; i < SAMPLE_MAX; i++)
    trace << CSV << 'A' << i;
  trace << endl;
  while (file.read(&entry, sizeof(entry)) == sizeof(entry)) {
    trace << time_t(entry.timestamp.sec) << CSV << entry.timestamp.ms;
    for (uint8_t i = 0; i < SAMPLE_MAX; i++)
      trace << CSV << entry.sample[i];
    trace << endl;
  }
#endif
  trace << PSTR("Size: ") << file.tell() << PSTR(" bytes") << endl;
  trace << PSTR("Errors: ") << err << endl;
  trace << PSTR("Period max: ") << max << PSTR(" ms") << endl;
  ASSERT(file.close());
  ASSERT(sd.end());

  ASSERT(true == false);
}
