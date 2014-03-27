/**
 * @file Cosa/USB/Core.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (c) 2010, Peter Barrett (original author)
 * Copyright (C) 2013, Mikael Patel (refactoring and extensions)
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/USB/Platform.h"

#if defined(USBCON)

#include "Cosa/IOStream/Driver/CDC.hh"

#define delay(x) 			DELAY((x) * 1000)
#define min(a,b) 			((a)<(b)?(a):(b))
#define max(a,b) 			((a)>(b)?(a):(b))

#define TX_RX_LED_INIT			DDRD |= (1<<5), DDRB |= (1<<0)
#define TX_LED_OFF			PORTD |= (1<<5)
#define TX_LED_ON			PORTD &= ~(1<<5)
#define RX_LED_OFF			PORTB |= (1<<0)
#define RX_LED_ON			PORTB &= ~(1<<0)

#define EP_TYPE_CONTROL			0x00
#define EP_TYPE_BULK_IN			0x81
#define EP_TYPE_BULK_OUT		0x80
#define EP_TYPE_INTERRUPT_IN		0xC1
#define EP_TYPE_INTERRUPT_OUT		0xC0
#define EP_TYPE_ISOCHRONOUS_IN		0x41
#define EP_TYPE_ISOCHRONOUS_OUT		0x40

/** 
 * Pulse generation counters to keep track of the number of
 * milliseconds remaining for each pulse type  
 */   
#define TX_RX_LED_PULSE_MS 100
volatile uint8_t TxLEDPulse;
volatile uint8_t RxLEDPulse;

extern const uint16_t STRING_LANGUAGE[] PROGMEM;
extern const uint16_t STRING_IPRODUCT[] PROGMEM;
extern const uint16_t STRING_IMANUFACTURER[] PROGMEM;
extern const DeviceDescriptor USB_DeviceDescriptor PROGMEM;
extern const DeviceDescriptor USB_DeviceDescriptorA PROGMEM;

const uint16_t STRING_LANGUAGE[2] = {
  (3<<8) | (2+2),
  0x0409			
};

const uint16_t STRING_IPRODUCT[17] = {
  (3<<8) | (2+2*16),
#if USB_PID == 0x8036	
  'A','r','d','u','i','n','o',' ','L','e','o','n','a','r','d','o'
#elif USB_PID == 0x8037
  'A','r','d','u','i','n','o',' ','M','i','c','r','o',' ',' ',' '
#elif USB_PID == 0x803C
  'A','r','d','u','i','n','o',' ','E','s','p','l','o','r','a',' '
#elif USB_PID == 0x9208
  'L','i','l','y','P','a','d','U','S','B',' ',' ',' ',' ',' ',' '
#else
  'U','S','B',' ','I','O',' ','B','o','a','r','d',' ',' ',' ',' '
#endif
};

const uint16_t STRING_IMANUFACTURER[12] = {
  (3<<8) | (2+2*11),
#if USB_VID == 0x2341
  'A','r','d','u','i','n','o',' ','L','L','C'
#elif USB_VID == 0x1b4f
  'S','p','a','r','k','F','u','n',' ',' ',' '
#else
  'U','n','k','n','o','w','n',' ',' ',' ',' '
#endif
};

#ifdef CDC_ENABLED
#define DEVICE_CLASS 0x02
#else
#define DEVICE_CLASS 0x00
#endif

// DEVICE DESCRIPTOR
const DeviceDescriptor USB_DeviceDescriptor =
  D_DEVICE(0x00,0x00,0x00,64,USB_VID,USB_PID,0x100,IMANUFACTURER,IPRODUCT,0,1);

const DeviceDescriptor USB_DeviceDescriptorA =
  D_DEVICE(DEVICE_CLASS,0x00,0x00,64,USB_VID,USB_PID,0x100,IMANUFACTURER,IPRODUCT,0,1);

volatile uint8_t _usbConfiguration = 0;

static inline void 
WaitIN(void)
{
  while (!(UEINTX & (1<<TXINI)));
}

static inline void 
ClearIN(void)
{
  UEINTX = ~(1<<TXINI);
}

static inline void 
WaitOUT(void)
{
  while (!(UEINTX & (1<<RXOUTI)));
}

static inline uint8_t 
WaitForINOrOUT()
{
  while (!(UEINTX & ((1<<TXINI)|(1<<RXOUTI))));
  return (UEINTX & (1<<RXOUTI)) == 0;
}

static inline void 
ClearOUT(void)
{
  UEINTX = ~(1<<RXOUTI);
}

void 
Recv(volatile uint8_t* data, uint8_t count)
{
  while (count--) *data++ = UEDATX;
  RX_LED_ON;
  RxLEDPulse = TX_RX_LED_PULSE_MS;	
}

static inline uint8_t 
Recv8()
{
  RX_LED_ON;
  RxLEDPulse = TX_RX_LED_PULSE_MS;
  return (UEDATX);
}

static inline void 
Send8(uint8_t d)
{
  UEDATX = d;
}

static inline void 
SetEP(uint8_t ep)
{
  UENUM = ep;
}

static inline uint8_t 
FifoByteCount()
{
  return (UEBCLX);
}

static inline uint8_t 
ReceivedSetupInt()
{
  return (UEINTX & (1<<RXSTPI));
}

static inline void 
ClearSetupInt()
{
  UEINTX = ~((1<<RXSTPI) | (1<<RXOUTI) | (1<<TXINI));
}

static inline void 
Stall()
{
  UECONX = (1<<STALLRQ) | (1<<EPEN);
}

static inline uint8_t 
ReadWriteAllowed()
{
  return (UEINTX & (1<<RWAL));
}

static inline uint8_t 
Stalled()
{
  return (UEINTX & (1<<STALLEDI));
}

static inline uint8_t 
FifoFree()
{
  return (UEINTX & (1<<FIFOCON));
}

static inline void 
ReleaseRX()
{
  // FIFOCON=0 NAKINI=1 RWAL=1 NAKOUTI=0 RXSTPI=1 RXOUTI=0 STALLEDI=1 TXINI=1
  UEINTX = 0x6B;
}

static inline void 
ReleaseTX()
{
  // FIFOCON=0 NAKINI=0 RWAL=1 NAKOUTI=1 RXSTPI=1 RXOUTI=0 STALLEDI=1 TXINI=0
  UEINTX = 0x3A;
}

static inline uint8_t 
FrameNumber()
{
  return (UDFNUML);
}

uint8_t USBGetConfiguration(void)
{
  return (_usbConfiguration);
}

#define USB_RECV_TIMEOUT
class LockEP
{
  uint8_t _sreg;
public:
  LockEP(uint8_t ep) : _sreg(SREG)
  {
    cli();
    SetEP(ep & 7);
  }
  ~LockEP()
  {
    SREG = _sreg;
  }
};

uint8_t 
USB_Available(uint8_t ep)
{
  LockEP lock(ep);
  return (FifoByteCount());
}

int 
USB_Recv(uint8_t ep, void* d, int len)
{
  if (!_usbConfiguration || len < 0) return (-1);
	
  LockEP lock(ep);
  uint8_t n = FifoByteCount();
  len = min(n,len);
  n = len;
  uint8_t* dst = (uint8_t*)d;
  while (n--)
    *dst++ = Recv8();
  if (len && !FifoByteCount())
    ReleaseRX();
  return (len);
}

int 
USB_Recv(uint8_t ep)
{
  uint8_t c;
  if (USB_Recv(ep, &c, 1) == 1)
    return (c);
  return (-1);
}

uint8_t 
USB_SendSpace(uint8_t ep)
{
  LockEP lock(ep);
  if (!ReadWriteAllowed()) return (0);
  return (64 - FifoByteCount());
}

int 
USB_Send(uint8_t ep, const void* d, int len)
{
  if (!_usbConfiguration) return (-1);

  int res = len;
  const uint8_t* data = (const uint8_t*)d;
  uint8_t timeout = 250;
  while (len) {
    uint8_t n = USB_SendSpace(ep);
    if (n == 0) {
      if (!(--timeout)) return (-1);
      delay(1);
      continue;
    }
    if (n > len) n = len;
    {
      LockEP lock(ep);
      if (!ReadWriteAllowed()) continue;
      len -= n;
      if (ep & TRANSFER_ZERO) {
	while (n--) Send8(0);
      }
      else if (ep & TRANSFER_PGM) {
	while (n--) Send8(pgm_read_byte(data++));
      }
      else {
	while (n--) Send8(*data++);
      }
      if (!ReadWriteAllowed() || ((len == 0) && (ep & TRANSFER_RELEASE)))
	ReleaseTX();
    }
  }
  TX_LED_ON;
  TxLEDPulse = TX_RX_LED_PULSE_MS;
  return (res);
}

extern const uint8_t _initEndpoints[] PROGMEM;
const uint8_t _initEndpoints[] = 
  {
    0,
	
#ifdef CDC_ENABLED
    EP_TYPE_INTERRUPT_IN,		// CDC_ENDPOINT_ACM
    EP_TYPE_BULK_OUT,			// CDC_ENDPOINT_OUT
    EP_TYPE_BULK_IN,			// CDC_ENDPOINT_IN
#endif

#ifdef HID_ENABLED
    EP_TYPE_INTERRUPT_IN		// HID_ENDPOINT_INT
#endif
  };

#define EP_SINGLE_64 0x32
#define EP_DOUBLE_64 0x36

static void 
InitEP(uint8_t index, uint8_t type, uint8_t size)
{
  UENUM = index;
  UECONX = 1;
  UECFG0X = type;
  UECFG1X = size;
}

static void 
InitEndpoints()
{
  for (uint8_t i = 1; i < sizeof(_initEndpoints); i++) {
    UENUM = i;
    UECONX = 1;
    UECFG0X = pgm_read_byte(_initEndpoints+i);
    UECFG1X = EP_DOUBLE_64;
  }
  UERST = 0x7E;
  UERST = 0;
}

static bool 
ClassInterfaceRequest(Setup& setup)
{
  uint8_t i = setup.wIndex;

#ifdef CDC_ENABLED
  if (CDC_ACM_INTERFACE == i) return (CDC_Setup(setup));
#endif

#ifdef HID_ENABLED
  if (HID_INTERFACE == i) return (HID_Setup(setup));
#endif

  return (false);
}

static int _cmark;
static int _cend;

void 
InitControl(int end)
{
  SetEP(0);
  _cmark = 0;
  _cend = end;
}

static bool 
SendControl(uint8_t d)
{
  if (_cmark < _cend) {
    if (!WaitForINOrOUT()) return (false);
    Send8(d);
    if (!((_cmark + 1) & 0x3F)) ClearIN();
  }
  _cmark++;
  return (true);
};

int 
USB_SendControl(uint8_t flags, const void* d, int len)
{
  int sent = len;
  const uint8_t* data = (const uint8_t*)d;
  bool pgm = flags & TRANSFER_PGM;
  while (len--) {
    uint8_t c = pgm ? pgm_read_byte(data++) : *data++;
    if (!SendControl(c)) return (-1);
  }
  return (sent);
}

int 
USB_RecvControl(void* d, int len)
{
  WaitOUT();
  Recv((uint8_t*)d,len);
  ClearOUT();
  return (len);
}

int 
SendInterfaces()
{
  int total = 0;
  uint8_t interfaces = 0;

#ifdef CDC_ENABLED
  total = CDC_GetInterface(&interfaces);
#endif

#ifdef HID_ENABLED
  total += HID_GetInterface(&interfaces);
#endif
  UNUSED(total);
  return (interfaces);
}

static bool 
SendConfiguration(int maxlen)
{
  InitControl(0);	
  uint8_t interfaces = SendInterfaces();
  ConfigDescriptor config = D_CONFIG(_cmark + sizeof(ConfigDescriptor), interfaces);

  InitControl(maxlen);
  USB_SendControl(0,&config,sizeof(ConfigDescriptor));
  SendInterfaces();
  return (true);
}

static uint8_t _cdcComposite = 0;

static bool 
SendDescriptor(Setup& setup)
{
  uint8_t t = setup.wValueH;
  if (USB_CONFIGURATION_DESCRIPTOR_TYPE == t)
    return (SendConfiguration(setup.wLength));

  InitControl(setup.wLength);
#ifdef HID_ENABLED
  if (HID_REPORT_DESCRIPTOR_TYPE == t)
    return (HID_GetDescriptor(t));
#endif

  uint8_t desc_length = 0;
  const uint8_t* desc_addr = 0;
  if (USB_DEVICE_DESCRIPTOR_TYPE == t) {
    if (setup.wLength == 8)
      _cdcComposite = 1;
    desc_addr = _cdcComposite ?  (const uint8_t*)&USB_DeviceDescriptorA : (const uint8_t*)&USB_DeviceDescriptor;
  }
  else if (USB_STRING_DESCRIPTOR_TYPE == t) {
    if (setup.wValueL == 0)
      desc_addr = (const uint8_t*)&STRING_LANGUAGE;
    else if (setup.wValueL == IPRODUCT) 
      desc_addr = (const uint8_t*)&STRING_IPRODUCT;
    else if (setup.wValueL == IMANUFACTURER)
      desc_addr = (const uint8_t*)&STRING_IMANUFACTURER;
    else
      return (false);
  }

  if (desc_addr == 0) return (false);
  if (desc_length == 0)
    desc_length = pgm_read_byte(desc_addr);
  
  USB_SendControl(TRANSFER_PGM,desc_addr,desc_length);
  return (true);
}

ISR(USB_COM_vect)
{
  SetEP(0);
  if (!ReceivedSetupInt()) return;

  Setup setup;
  Recv((uint8_t*)&setup,8);
  ClearSetupInt();

  uint8_t requestType = setup.bmRequestType;
  if (requestType & REQUEST_DEVICETOHOST)
    WaitIN();
  else
    ClearIN();

  bool ok = true;
  if (REQUEST_STANDARD == (requestType & REQUEST_TYPE)) {
    uint8_t r = setup.bRequest;
    switch (r) {
    case GET_STATUS:
      Send8(0);
      Send8(0);
      break;
    case SET_ADDRESS:
      WaitIN();
      UDADDR = setup.wValueL | (1<<ADDEN);
      break;
    case GET_DESCRIPTOR:
      ok = SendDescriptor(setup);
      break;
    case SET_DESCRIPTOR:
      ok = false;
      break;
    case GET_CONFIGURATION:
      Send8(1);
      break;
    case SET_CONFIGURATION:
      if (REQUEST_DEVICE == (requestType & REQUEST_RECIPIENT)) {
	InitEndpoints();
	_usbConfiguration = setup.wValueL;
      } else
	ok = false;
      break;
    case CLEAR_FEATURE:
    case SET_FEATURE:
    case GET_INTERFACE:
    case SET_INTERFACE:
      break;
    }
  }
  else {
    InitControl(setup.wLength);
    ok = ClassInterfaceRequest(setup);
  }
  
  if (ok)
    ClearIN();
  else 
    Stall();
}

void 
USB_Flush(uint8_t ep)
{
  SetEP(ep);
  if (FifoByteCount())
    ReleaseTX();
}

ISR(USB_GEN_vect)
{
  uint8_t udint = UDINT;
  UDINT = 0;

  // End of Reset
  if (udint & (1<<EORSTI)) {
    InitEP(0,EP_TYPE_CONTROL,EP_SINGLE_64);	// init ep0
    _usbConfiguration = 0;			// not configured yet
    UEIENX = 1 << RXSTPE;			// Enable interrupts for ep0
  }

  // Start of Frame - happens every millisecond so we use it for TX
  // and RX LED one-shot timing, too 
  if (udint & (1<<SOFI)) {
#ifdef CDC_ENABLED
    USB_Flush(CDC_TX);			// Send a tx frame if found
    if (USB_Available(CDC_RX))		// Handle received bytes (if any)
      cdc.accept();
#endif
		
    // check whether the one-shot period has elapsed.  if so, turn off the LED
    if (TxLEDPulse && !(--TxLEDPulse))
      TX_LED_OFF;
    if (RxLEDPulse && !(--RxLEDPulse))
      RX_LED_OFF;
  }
}

uint8_t 
USBConnected()
{
  uint8_t f = UDFNUML;
  delay(3);
  return (f != UDFNUML);
}

USBDevice_ USBDevice;

USBDevice_::USBDevice_()
{
}

void 
USBDevice_::attach()
{
  _usbConfiguration = 0;
  UHWCON = 0x01;		  // power internal reg
  USBCON = (1<<USBE)|(1<<FRZCLK); // clock frozen, usb enabled
#if F_CPU == 16000000UL
  PLLCSR = 0x12;		  // Need 16 MHz xtal
#elif F_CPU == 8000000UL
  PLLCSR = 0x02;		  // Need 8 MHz xtal
#endif
  while (!(PLLCSR & (1<<PLOCK))) // wait for lock pll
    ;
  // Some tests on specific versions of macosx (10.7.3), reported some
  // strange behaviuors when the board is reset using the serial
  // port touch at 1200 bps. This delay fixes this behaviour.
  delay(1);

  USBCON = ((1<<USBE)|(1<<OTGPADE)); // Start USB clock
  UDIEN = (1<<EORSTE)|(1<<SOFE); // Enable interrupts for EOR (End of
				 // Reset) and SOF (start of frame) 
  UDCON = 0;			 // enable attach resistor
  TX_RX_LED_INIT;
}

void 
USBDevice_::detach()
{
}

bool 
USBDevice_::configured()
{
  return (_usbConfiguration);
}

void 
USBDevice_::poll()
{
}

#endif
