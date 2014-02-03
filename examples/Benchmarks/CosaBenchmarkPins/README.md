CosaBenchmarkPins
=================

Measurement of the pins operations; digital/analog input pin read,
digital output pin write and toggle, and serial output with clock. 

Reading a digital pin in Cosa is 6-7X faster than Arduino. Writing is
2-10X faster. Serial output with data and clock pin is 4X faster.
Reading an analog pin in Cosa is equal to Arduino.

The speedup can be explained by the caching of port register pointer
and pin mask in Cosa compared to Arduino. Also most access functions in 
Cosa are inlined. Though object-oriented and in/output operator syntax
Cosa is between 2-10X faster allowing high speed protocols. This comes
with a small price-tag; memory, 4 bytes per digital pin and 12 bytes
per analog pin. The analog pin object holds the latest sample, 
reference voltage, and allows an event handler. This accounts for 
the extra 8 bytes. 

Below values are for an Arduino Nano (ATmega328P). The speedup is even
larger for Arduino Mega 2560. 

	CosaBenchmarkPins: started   
	free_memory() = 1518   
	sizeof(Event::Handler) = 2   
	sizeof(InputPin) = 4   
	sizeof(OutputPin) = 4   
	sizeof(AnalogPin) = 12   
	F_CPU = 16000000   
	I_CPU = 16   
	   
	99:loop:info:Measure the time to perform an empty loop block   
	107:loop:info:nop:505 ns   
	
	109:loop:info:Measure the time to perform an input pin read   
	118:loop:info:inPin.is_set():882 ns   
	130:loop:info:inPin >> var:881 ns   
	140:loop:info:InputPin::read(7):566 ns   
	150:loop:info:read digitalRead(7):566 ns   
	   
	152:loop:info:Measure the time to perform an output pin write     
	162:loop:info:outPin.write():913 ns   
	175:loop:info:outPin._write():818 ns   
	186:loop:info:outPin.set/clear():913 ns   
	199:loop:info:outPin._set/_clear():818 ns   
	210:loop:info:outPin << val:913 ns   
	221:loop:info:OutputPin::write(8, val):3086 ns   
	232:loop:info:digitalWrite(8, val):3086 ns   
	243:loop:info:outPin.toggle():692 ns   
	256:loop:info:outPin._toggle():598 ns   
	267:loop:info:OutputPin::toggle(8):251 ns   
	
	269:loop:info:Measure the time to perform input pin read/output pin write   
	278:loop:info:outPin.write(!inPin.read()):1889 ns   
	288:loop:info:inPin.is_set();outPin.clear/set():1826 ns   
	300:loop:info:inPin >> var; outPin << !var:1889 ns   
	310:loop:info:outPin.set(inPin.is_clear()):1889 ns   
	320:loop:info:OutputPin::write(8, !InputPin::read(7)):3401 ns   
	333:loop:info:OutputPin::read(7)/write(8,0/1):3875 ns   
	
	335:loop:info:Measure the time to perform 8-bit serial data transfer   
	343:loop:info:pin.write(data,clk):26 us   
	356:loop:info:pin.write();clock.write(1/0):28 us   
	371:loop:info:pin._write();clock._write(1/0):25 us   
	384:loop:info:pin.write/toggle():25 us   
	399:loop:info:pin._write/_toggle():21 us   
	412:loop:info:OutputPin::write():91 us   
	425:loop:info:OutputPin::write/toggle():36 us   
	457:loop:info:pin.write/toggle() unrolled:19 us   
	   
	459:loop:info:Measure the time to read analog pin   
	465:loop:info:analogPin.sample():112 us   
	474:loop:info:analogPin.sample_request/await():112 us   
	483:loop:info:analogPin >> var:112 us   
	490:loop:info:AnalogPin::sample():112 us   
	
	492:loop:info:Measure the time to read analog pin with varying prescale   
	501:loop:info:prescale(128):bits(10):analogPin.sample():112 us   
	501:loop:info:prescale(64):bits(9):analogPin.sample():56 us   
	501:loop:info:prescale(32):bits(8):analogPin.sample():30 us   
	501:loop:info:prescale(16):bits(7):analogPin.sample():17 us   
	501:loop:info:prescale(8):bits(6):analogPin.sample():10 us   
	501:loop:info:prescale(4):bits(5):analogPin.sample():6 us   
	501:loop:info:prescale(2):bits(4):analogPin.sample():5 us   
   
Circuit
-------
This example requires no special circuit. 




