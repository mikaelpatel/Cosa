/**
 * @file Cosa/AVR/STK500.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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

#include "Cosa/AVR/STK500.hh"
#include "Cosa/Power.hh"

uint8_t
STK500::getchar()
{
  while (!m_dev->available()) Power::sleep(SLEEP_MODE_IDLE);
  return (m_dev->getchar());
}

void 
STK500::read(void* buf, size_t count)
{
  uint8_t* bp = (uint8_t*) buf;
  while (count--) *bp++ = getchar();
}

void
STK500::putchar(uint8_t c)
{
  while (m_dev->putchar(c) == IOStream::EOF) 
    Power::sleep(SLEEP_MODE_IDLE);
}

void 
STK500::write(void* buf, size_t count)
{
  uint8_t* bp = (uint8_t*) buf;
  while (count--) putchar(*bp++);
}

bool
STK500::is_insync()
{
  if (getchar() == CRC_EOP) {
    putchar(INSYNC);    
    return (true);
  }
  putchar(NOSYNC);    
  return (false);
}

void 
STK500::response()
{
  putchar(OK);
}

void 
STK500::response(uint8_t param)
{
  putchar(param);
  putchar(OK);
}

void 
STK500::response(uint8_t param1, uint8_t param2)
{
  putchar(param1);
  putchar(param2);
  putchar(OK);
}

void 
STK500::response(uint8_t param1, uint8_t param2, uint8_t param3)
{
  putchar(param1);
  putchar(param2);
  putchar(param3);
  putchar(OK);
}

void 
STK500::response(const char* param, size_t count)
{
  while (count--) putchar(*param++);
  putchar(OK);
}

void 
STK500::response_P(str_P param)
{
  const char* p = (const char*) param;
  char c;
  while ((c = pgm_read_byte(p++)) != 0) putchar(c);
  putchar(OK);
}

void 
STK500::failed(uint8_t param)
{
  putchar(param);
  putchar(FAILED);
}

void 
STK500::illegal(uint8_t resp)
{
  putchar(resp);
}

void
STK500::set_parameter()
{
  uint8_t param = getchar();
  uint8_t value = getchar();
  UNUSED(value);
  if (!is_insync()) return;
  switch (param) {
  case HW_VER:
  case SW_MAJOR:
  case SW_MINOR:
  case LEDS:
  case VTARGET:
  case VADJUST:
  case OSC_PSCALE:
  case OSC_CMATCH:
  case RESET_DURATION:
  case SCK_DURATION:
  case BUFSIZEL:
  case BUFSIZEH:
  case DEVICE:
  case PROGMODE:
  case PARAMODE:
  case POLLING:
  case SELFTIMED:
  case TOPCARD_DETECT:
  default:
    failed(param);
    return;
  }
  response();
}

void
STK500::get_parameter()
{
  uint8_t param = getchar();
  if (!is_insync()) return;
  uint8_t value = 0;
  switch (param) {
  case HW_VER:
    value = 42;
    break;
  case SW_MAJOR:
    value = 6;
    break;
  case SW_MINOR:
    value = 7;
    break;
  case PROGMODE:
    value = 'S';
    break;
  case LEDS:
  case VTARGET:
  case VADJUST:
  case OSC_PSCALE:
  case OSC_CMATCH:
  case RESET_DURATION:
  case SCK_DURATION:
  case BUFSIZEL:
  case BUFSIZEH:
  case DEVICE:
  case PARAMODE:
  case POLLING:
  case SELFTIMED:
  case TOPCARD_DETECT:
    break;
  default:
    failed(param);
    return;
  }
  response(value);
}

void
STK500::set_device()
{
  param_t param;
  read(&param, sizeof(param));
  if (!is_insync()) return;
  m_prog->set_flash_pagesize(swap((int16_t) param.pagesize));
  m_state = READY_STATE;
  response();
}

void
STK500::set_device_ext()
{
  extparam_t extparam;
  read(&extparam, sizeof(extparam));
  if (!is_insync()) return;
  response();
}

void
STK500::enter_progmode()
{
  if (!is_insync()) return;
  if ((m_state != READY_STATE) || !m_prog->begin()) {
    illegal(NODEVICE);
    return;
  }
  m_state = PROG_STATE;
  response();
}

void
STK500::leave_progmode()
{
  if (!is_insync()) return;
  if (m_state == PROG_STATE) m_prog->end();
  m_state = (m_state == PROG_STATE) ? READY_STATE : IDLE_STATE;
  response();
}

void 
STK500::chip_erase()
{
  if (!is_insync()) return;
  m_prog->chip_erase();
  response();
}  

void 
STK500::load_address()
{
  univ16_t addr;
  addr.low = getchar();
  addr.high = getchar();
  if (!is_insync()) return;
  m_addr = addr.as_uint16;
  response();
}  

void 
STK500::universal()
{
  uint8_t buf[4];
  read(buf, sizeof(buf));
  if (!is_insync()) return;
  uint8_t res = m_prog->transfer(buf);
  m_prog->await();
  response(res);
}  

void 
STK500::universal_multi()
{
  uint16_t bytes = getchar() + 1;
  uint8_t buf[bytes];
  read(buf, bytes);
  if (!is_insync()) return;
  uint8_t* bp = buf;
  while (bytes--) m_prog->transfer(*bp++);
  response();
}  

void 
STK500::prog_flash()
{
  uint8_t low = getchar();
  uint8_t high = getchar();
  if (!is_insync()) return;
  m_prog->load_program_memory_page_low_byte(m_addr, low);
  m_prog->load_program_memory_page_high_byte(m_addr, high);
  m_addr += 1;
  response();
}  

void 
STK500::prog_data()
{
  uint8_t data = getchar();
  if (!is_insync()) return;
  m_prog->write_eeprom_memory(m_addr, data);
  m_addr += 1;
  response();
}  

void 
STK500::prog_fuse()
{
  uint8_t low = getchar();
  uint8_t high = getchar();
  if (!is_insync()) return;
  m_prog->write_fuse_bits(low);
  m_prog->write_fuse_high_bits(high);
  response();
}  

void 
STK500::prog_lock()
{
  uint8_t lock = getchar();
  if (!is_insync()) return;
  m_prog->write_lock_bits(lock);
  response();
}  

void 
STK500::prog_page()
{
  univ16_t bytes;
  bytes.high = getchar();
  bytes.low = getchar();
  uint16_t count = bytes.as_uint16;
  uint8_t memtype = getchar();
  uint8_t buf[count];
  read(buf, count);
  if (!is_insync()) return;
  if (memtype == 'F') {
    m_prog->write_program_memory(m_addr, buf, count);
  }
  else if (memtype == 'E') {
    m_prog->write_eeprom_memory(m_addr << 1, buf, count);
  }
  m_addr += (count / 2);
  response();
}  

void 
STK500::prog_fuse_ext()
{
  uint8_t low = getchar();
  uint8_t high = getchar();
  uint8_t ext = getchar();
  if (!is_insync()) return;
  m_prog->write_fuse_bits(low);
  m_prog->write_fuse_high_bits(high);
  m_prog->write_extended_fuse_bits(ext);
  response();
}  

void 
STK500::read_flash()
{
  if (!is_insync()) return;
  uint8_t low = m_prog->read_program_memory_low_byte(m_addr);
  uint8_t high = m_prog->read_program_memory_high_byte(m_addr);
  m_addr += 1;
  response(low, high);
}

void 
STK500::read_data()
{
  if (!is_insync()) return;
  uint8_t data = m_prog->read_eeprom_memory(m_addr);
  m_addr += 1;
  response(data);
}

void 
STK500::read_fuse()
{
  if (!is_insync()) return;
  uint8_t low = m_prog->read_fuse_bits();
  uint8_t high = m_prog->read_fuse_high_bits();
  response(low, high);
}

void 
STK500::read_lock()
{
  if (!is_insync()) return;
  uint8_t bits = m_prog->read_lock_bits();
  response(bits);
}

void 
STK500::read_page()
{
  univ16_t bytes;
  bytes.high = getchar();
  bytes.low = getchar();
  uint16_t count = bytes.as_uint16;
  uint8_t memtype = getchar();
  if (!is_insync()) return;
  uint8_t buf[count];
  if (memtype == 'F') {
    m_prog->read_program_memory(buf, m_addr, count);
  }
  else if (memtype == 'E') {
    m_prog->read_eeprom_memory(buf, m_addr << 1, count);
  }
  m_addr += count / 2;
  write(buf, count);
  response();
}

void  
STK500::read_sign()
{
  if (!is_insync()) return;
  uint8_t high = m_prog->read_signature_byte(0);
  uint8_t middle = m_prog->read_signature_byte(1);
  uint8_t low = m_prog->read_signature_byte(2);
  response(high, middle, low);
}

void  
STK500::read_osccal()
{
  if (!is_insync()) return;
  uint8_t cal = m_prog->read_calibration_byte();
  response(cal);
}

void  
STK500::read_fuse_ext()
{
  if (!is_insync()) return;
  uint8_t low = m_prog->read_fuse_bits();
  uint8_t high = m_prog->read_fuse_high_bits();
  uint8_t ext = m_prog->read_extended_fuse_bits();
  response(low, high, ext);
}

void 
STK500::run()
{
  uint8_t cmnd = getchar();
  switch (cmnd) {
  case GET_SYNC:
  case CHECK_AUTOINC: 
    if (is_insync()) response(); 
    return;
  case GET_SIGN_ON:
    if (is_insync()) response_P(PSTR("Cosa AVR ISP")); 
    return;
  case SET_PARAMETER: 
    set_parameter(); 
    return;
  case GET_PARAMETER:
    get_parameter(); 
    return;
  case SET_DEVICE:
    set_device(); 
    return;
  case SET_DEVICE_EXT:
    set_device_ext(); 
    return;
  case ENTER_PROGMODE:
    enter_progmode(); 
    return;
  case LEAVE_PROGMODE:
    leave_progmode(); 
    return;
  case CHIP_ERASE: 
    chip_erase(); 
    return;
  case LOAD_ADDRESS:
    load_address(); 
    return;
  case UNIVERSAL:
    universal(); 
    return;
  case UNIVERSAL_MULTI: 
    universal_multi(); 
    return;
  case PROG_FLASH:
    prog_flash();
    return;
  case PROG_DATA:
    prog_data(); 
    return;
  case PROG_FUSE: 
    prog_fuse(); 
    return;
  case PROG_LOCK: 
    prog_lock(); 
    return;
  case PROG_PAGE:
    prog_page(); 
    return;
  case PROG_FUSE_EXT: 
    prog_fuse_ext(); 
    return;
  case READ_FLASH: 
    read_flash(); 
    return;
  case READ_DATA: 
    read_data(); 
    return;
  case READ_FUSE: 
    read_fuse(); 
    return; 
  case READ_LOCK: 
    read_lock(); 
    return; 
  case READ_PAGE:
    read_page(); 
    return;
  case READ_SIGN:
    read_sign(); 
    return;
  case READ_OSCCAL: 
    read_osccal(); 
    return;
  case READ_FUSE_EXT: 
    read_fuse_ext(); 
    return; 
  }
  if (is_insync()) illegal(UNKNOWN);
}
