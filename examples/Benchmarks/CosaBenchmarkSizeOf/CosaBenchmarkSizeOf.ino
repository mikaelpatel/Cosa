/**
 * @file CosaBenchmarkSizeOf.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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
 * @section Description
 * Cosa sizeof benchmark; the size of class instances. Static member
 * data are not included. Note that this is also a list of the classes
 * available in Cosa.
 *
 * @section Circuit
 * This example requires no special circuit. Uses serial output.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"

#include "Cosa/AVR/Programmer.hh"
#include "Cosa/AVR/STK500.hh"
#include "Cosa/BitSet.hh"
#include "Cosa/Button.hh"
#include "Cosa/Canvas.hh"
#include "Cosa/Canvas/Driver/ST7735.hh"
#include "Cosa/Canvas/Element/Textbox.hh"
#include "Cosa/Canvas/Font.hh"
#include "Cosa/Canvas/OffScreen.hh"
#include "Cosa/Cipher/Base64.hh"
#include "Cosa/Cipher/RC4.hh"
#include "Cosa/Cipher/Vigenere.hh"
#include "Cosa/Driver/DHT.hh"
#include "Cosa/Driver/DS1302.hh"
#include "Cosa/Driver/HCSR04.hh"
#include "Cosa/Driver/NEXA.hh"
#include "Cosa/EEPROM.hh"
#include "Cosa/Event.hh"
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/FS/FAT16.hh"
#include "Cosa/FSM.hh"
#include "Cosa/Interrupt.hh"
#include "Cosa/INET/DHCP.hh"
#include "Cosa/INET/DNS.hh"
#include "Cosa/INET/HTTP.hh"
#include "Cosa/INET/NTP.hh"
#include "Cosa/INET/SNMP.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/IOStream/Driver/CDC.hh"
#include "Cosa/IOStream/Driver/RS485.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/IOStream/Driver/WIO.hh"
#include "Cosa/IR.hh"
#include "Cosa/IoT/MQTT.hh"
#include "Cosa/IoT/ThingSpeak.hh"
#include "Cosa/Interrupt.hh"
#include "Cosa/Keypad.hh"
#include "Cosa/LCD.hh"
#include "Cosa/LCD/Driver/HD44780.hh"
#include "Cosa/LCD/Driver/PCD8544.hh"
#include "Cosa/LCD/Driver/ST7565.hh"
#include "Cosa/LCD/Driver/VLCD.hh"
#include "Cosa/LED.hh"
#include "Cosa/Linkage.hh"
#include "Cosa/Listener.hh"
#include "Cosa/Menu.hh"
#include "Cosa/Nucleo/Thread.hh"
#include "Cosa/Nucleo/Semaphore.hh"
#include "Cosa/Nucleo/Mutex.hh"
#include "Cosa/Nucleo/Actor.hh"
#include "Cosa/OWI.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/PinChangeInterrupt.hh"
#include "Cosa/Pin.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/PWMPin.hh"
#include "Cosa/IOPin.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/AnalogPins.hh"
#include "Cosa/AnalogComparator.hh"
#include "Cosa/ProtocolBuffer.hh"
#include "Cosa/Queue.hh"
#include "Cosa/Registry.hh"
#include "Cosa/Rotary.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Servo.hh"
#include "Cosa/Socket.hh"
#include "Cosa/Socket/Driver/W5100.hh"
#include "Cosa/SPI.hh"
#include "Cosa/SPI/Driver/SD.hh"
#include "Cosa/ProtoThread.hh"
#include "Cosa/Time.hh"
#include "Cosa/Timer.hh"
#include "Cosa/Touch.hh"
#include "Cosa/Trace.hh"
#include "Cosa/TWI.hh"
#include "Cosa/TWI/Driver/ADXL345.hh"
#include "Cosa/TWI/Driver/AT24CXX.hh"
#include "Cosa/TWI/Driver/BMP085.hh"
#include "Cosa/TWI/Driver/DS1307.hh"
#include "Cosa/TWI/Driver/DS3231.hh"
#include "Cosa/TWI/Driver/HMC5883L.hh"
#include "Cosa/TWI/Driver/L3G4200D.hh"
#include "Cosa/TWI/Driver/MPU6050.hh"
#include "Cosa/TWI/Driver/PCF8574.hh"
#include "Cosa/TWI/Driver/PCF8591.hh"
#include "Cosa/Wireless.hh"
#include "Cosa/Wireless/Driver/CC1101.hh"
#include "Cosa/Wireless/Driver/NRF24L01P.hh"
#include "Cosa/Wireless/Driver/RFM69.hh"
#include "Cosa/Wireless/Driver/VWI.hh"

void setup()
{
  // Start the trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkSizeOf: started"));

  // Check amount of free memory and size of instance
  TRACE(free_memory());
  TRACE(sizeof(Programmer));
  TRACE(sizeof(STK500));
  TRACE(sizeof(BitSet<64>));
  TRACE(sizeof(Button));
  TRACE(sizeof(Canvas));
  TRACE(sizeof(ST7735));
  TRACE(sizeof(Textbox));
  TRACE(sizeof(OffScreen<64,128>));
#if defined(USBCON)
  TRACE(sizeof(CDC));
#endif
  TRACE(sizeof(Base64));
  TRACE(sizeof(RC4));
  TRACE(sizeof(Vigenere<8>));
  TRACE(sizeof(DHT));
  TRACE(sizeof(DS1302));
  TRACE(sizeof(HCSR04));
  TRACE(sizeof(NEXA::Receiver));
  TRACE(sizeof(NEXA::Transmitter));
  TRACE(sizeof(EEPROM));
  TRACE(sizeof(Event));
  TRACE(sizeof(Event::queue));
  TRACE(sizeof(ExternalInterrupt));
  TRACE(sizeof(FAT16));
  TRACE(sizeof(FAT16::File));
  TRACE(sizeof(FSM));
  TRACE(sizeof(DHCP));
  TRACE(sizeof(DNS));
  TRACE(sizeof(HTTP::Client));
  TRACE(sizeof(HTTP::Server));
  TRACE(sizeof(MQTT::Client));
  TRACE(sizeof(ThingSpeak::Client));
  TRACE(sizeof(ThingSpeak::Channel));
  TRACE(sizeof(ThingSpeak::TalkBack));
  TRACE(sizeof(NTP));
  TRACE(sizeof(SNMP));
  TRACE(sizeof(SNMP::MIB));
  TRACE(sizeof(SNMP::MIB2_SYSTEM));
  TRACE(sizeof(Interrupt::Handler));
  TRACE(sizeof(IOBuffer<64>));
  TRACE(sizeof(IOStream));
#if !defined(__ARDUINO_TINY__)
  TRACE(sizeof(RS485));
  TRACE(sizeof(UART));
#endif
  TRACE(sizeof(WIO));
  TRACE(sizeof(IR::Receiver));
  TRACE(sizeof(Keypad));
  TRACE(sizeof(LCDKeypad));
  TRACE(sizeof(LCD));
  TRACE(sizeof(HD44780));
  TRACE(sizeof(PCD8544));
  TRACE(sizeof(ST7565));
  TRACE(sizeof(VLCD));
  TRACE(sizeof(LED));
  TRACE(sizeof(Linkage));
  TRACE(sizeof(Link));
  TRACE(sizeof(Head));
  TRACE(sizeof(Listener<int>));
  TRACE(sizeof(Menu));
  TRACE(sizeof(Menu::Action));
  TRACE(sizeof(Menu::Walker));
  TRACE(sizeof(Menu::KeypadController));
  TRACE(sizeof(Menu::RotaryController));
  TRACE(sizeof(Nucleo::Thread));
  TRACE(sizeof(Nucleo::Semaphore));
  TRACE(sizeof(Nucleo::Mutex));
  TRACE(sizeof(Nucleo::Actor));
  TRACE(sizeof(OWI));
  TRACE(sizeof(OWI::Driver));
  TRACE(sizeof(OWI::Search));
  TRACE(sizeof(DS18B20));
  TRACE(sizeof(Periodic));
  TRACE(sizeof(Pin));
  TRACE(sizeof(InputPin));
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(IOPin));
  TRACE(sizeof(PWMPin));
  TRACE(sizeof(AnalogPin));
  TRACE(sizeof(AnalogPins));
  TRACE(sizeof(AnalogComparator));
  TRACE(sizeof(ProtocolBuffer));
  TRACE(sizeof(Queue<int,32>));
  TRACE(sizeof(Registry));
  TRACE(sizeof(Rotary::Encoder));
  TRACE(sizeof(Rotary::Dial<int>));
  TRACE(sizeof(Rotary::AcceleratedDial<int, 900>));
  TRACE(sizeof(RTC));
  TRACE(sizeof(Servo));
  TRACE(sizeof(Socket));
#if !defined(__ARDUINO_TINY__)
  TRACE(sizeof(W5100));
  TRACE(sizeof(W5100::Driver));
#endif
  TRACE(sizeof(SPI::Driver));
  TRACE(sizeof(SPI::Slave));
  TRACE(sizeof(SD));
  TRACE(sizeof(ProtoThread));
  TRACE(sizeof(clock_t));
  TRACE(sizeof(time_t));
  TRACE(sizeof(Timer));
  TRACE(sizeof(Touch));
  TRACE(sizeof(Trace));
  TRACE(sizeof(TWI::Driver));
  TRACE(sizeof(TWI::Slave));
  TRACE(sizeof(ADXL345));
  TRACE(sizeof(AT24CXX));
  TRACE(sizeof(BMP085));
  TRACE(sizeof(DS1307));
  TRACE(sizeof(DS3231));
  TRACE(sizeof(HMC5883L));
  TRACE(sizeof(L3G4200D));
  TRACE(sizeof(MPU6050));
  TRACE(sizeof(PCF8574));
  TRACE(sizeof(PCF8591));
  TRACE(sizeof(Watchdog));
  TRACE(sizeof(Wireless::Driver));
  TRACE(sizeof(CC1101));
  TRACE(sizeof(NRF24L01P));
  TRACE(sizeof(RFM69));
  TRACE(sizeof(VWI));
}

void loop()
{
  ASSERT(true == false);
}

/**
@section Output
CosaBenchmarkSizeOf: started
free_memory() = 1638
sizeof(Programmer) = 2
sizeof(STK500) = 8
sizeof(BitSet<64>) = 8
sizeof(Button) = 12
sizeof(Canvas) = 7
sizeof(ST7735) = 23
sizeof(Textbox) = 21
sizeof(OffScreen<64,128>) = 1031
sizeof(Base64) = 1
sizeof(RC4) = 258
sizeof(Vigenere<8>) = 11
sizeof(DHT) = 32
sizeof(DS1302) = 13
sizeof(HCSR04) = 16
sizeof(NEXA::Receiver) = 33
sizeof(NEXA::Transmitter) = 8
sizeof(EEPROM) = 2
sizeof(Event) = 5
sizeof(Event::queue) = 82
sizeof(ExternalInterrupt) = 8
sizeof(FAT16) = 1
sizeof(FAT16::File) = 18
sizeof(FSM) = 12
sizeof(DHCP) = 34
sizeof(DNS) = 6
sizeof(HTTP::Client) = 4
sizeof(HTTP::Server) = 4
sizeof(MQTT::Client) = 6
sizeof(ThingSpeak::Client) = 2
sizeof(ThingSpeak::Channel) = 4
sizeof(ThingSpeak::TalkBack) = 8
sizeof(NTP) = 7
sizeof(SNMP) = 6
sizeof(SNMP::MIB) = 2
sizeof(SNMP::MIB2_SYSTEM) = 10
sizeof(Interrupt::Handler) = 2
sizeof(IOBuffer<64>) = 69
sizeof(IOStream) = 3
sizeof(RS485) = 157
sizeof(UART) = 9
sizeof(WIO) = 38
sizeof(IR::Receiver) = 33
sizeof(Keypad) = 23
sizeof(LCDKeypad) = 23
sizeof(LCD) = 3
sizeof(HD44780) = 16
sizeof(PCD8544) = 25
sizeof(ST7565) = 26
sizeof(VLCD) = 14
sizeof(LED) = 10
sizeof(Linkage) = 6
sizeof(Link) = 6
sizeof(Head) = 6
sizeof(Listener<int>) = 8
sizeof(Menu) = 1
sizeof(Menu::Action) = 2
sizeof(Menu::Walker) = 23
sizeof(Menu::KeypadController) = 25
sizeof(Menu::RotaryController) = 38
sizeof(Nucleo::Thread) = 33
sizeof(Nucleo::Semaphore) = 7
sizeof(Nucleo::Mutex) = 2
sizeof(Nucleo::Actor) = 45
sizeof(OWI) = 9
sizeof(OWI::Driver) = 18
sizeof(OWI::Search) = 20
sizeof(DS18B20) = 33
sizeof(Periodic) = 8
sizeof(Pin) = 4
sizeof(InputPin) = 4
sizeof(OutputPin) = 4
sizeof(IOPin) = 5
sizeof(PWMPin) = 4
sizeof(AnalogPin) = 12
sizeof(AnalogPins) = 18
sizeof(AnalogComparator) = 6
sizeof(ProtocolBuffer) = 4
sizeof(Queue<int,32>) = 66
sizeof(Registry) = 4
sizeof(Rotary::Encoder) = 22
sizeof(Rotary::Dial<int>) = 30
sizeof(Rotary::AcceleratedDial<int, 900>) = 36
sizeof(RTC) = 1
sizeof(Servo) = 11
sizeof(Socket) = 16
sizeof(W5100) = 133
sizeof(W5100::Driver) = 28
sizeof(SPI::Driver) = 11
sizeof(SPI::Slave) = 41
sizeof(SD) = 13
sizeof(ProtoThread) = 9
sizeof(clock_t) = 4
sizeof(time_t) = 7
sizeof(Timer) = 10
sizeof(Touch) = 19
sizeof(Trace) = 3
sizeof(TWI::Driver) = 3
sizeof(TWI::Slave) = 3
sizeof(ADXL345) = 3
sizeof(AT24CXX) = 13
sizeof(BMP085) = 37
sizeof(DS1307) = 3
sizeof(DS3231) = 3
sizeof(HMC5883L) = 13
sizeof(L3G4200D) = 3
sizeof(MPU6050) = 3
sizeof(PCF8574) = 5
sizeof(PCF8591) = 4
sizeof(Watchdog) = 1
sizeof(Wireless::Driver) = 8
sizeof(CC1101) = 32
sizeof(NRF24L01P) = 41
sizeof(RFM69) = 31
sizeof(VWI) = 161
264:void loop():assert:true == false
*/
