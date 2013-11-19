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

Below values are for an Arduino Mini Pro Atmega328P. The speedup is even
larger for Arduino Mega 2560. 

CosaBenchmarkPins: started
free_memory() = 1544
sizeof(Event::Handler) = 2
sizeof(InputPin) = 4
sizeof(OutputPin) = 4
sizeof(AnalogPin) = 12
F_CPU = 16000000
I_CPU = 16

97:loop:info:Measure the time to perform an empty loop block
105:loop:info:nop:507 ns

107:loop:info:Measure the time to perform an input pin read
116:loop:info:inPin.is_set():885 ns
128:loop:info:inPin >> var:885 ns
138:loop:info:InputPin::read(7):568 ns
148:loop:info:read digitalRead(7):568 ns

150:loop:info:Measure the time to perform an output pin write
160:loop:info:outPin.write():916 ns
173:loop:info:outPin._write():821 ns
184:loop:info:outPin.set/clear():916 ns
197:loop:info:outPin._set/_clear():821 ns
208:loop:info:outPin << val:916 ns
219:loop:info:OutputPin::write(8, val):3098 ns
230:loop:info:digitalWrite(8, val):3098 ns
241:loop:info:outPin.toggle():695 ns
254:loop:info:outPin._toggle():600 ns
265:loop:info:OutputPin::toggle(8):252 ns

267:loop:info:Measure the time to perform input pin read/output pin write
276:loop:info:outPin.write(!inPin.read()):1897 ns
286:loop:info:inPin.is_set();outPin.clear/set():1833 ns
298:loop:info:inPin >> var; outPin << !var:1897 ns
308:loop:info:outPin.set(inPin.is_clear()):1897 ns
318:loop:info:OutputPin::write(8, !InputPin::read(7)):3414 ns
331:loop:info:OutputPin::read(7)/write(8,0/1):3890 ns

333:loop:info:Measure the time to perform 8-bit serial data transfer
341:loop:info:pin.write(data,clk):26 us
354:loop:info:pin.write();clock.write(1/0):28 us
369:loop:info:pin._write();clock._write(1/0):25 us
382:loop:info:pin.write/toggle():25 us
397:loop:info:pin._write/_toggle():22 us
410:loop:info:OutputPin::write():91 us
423:loop:info:OutputPin::write/toggle():36 us
455:loop:info:pin.write/toggle() unrolled:19 us

457:loop:info:Measure the time to read analog pin
463:loop:info:analogPin.sample():112 us
472:loop:info:analogPin.sample_request/await():112 us
481:loop:info:analogPin >> var:112 us
488:loop:info:AnalogPin::sample():112 us

490:loop:info:Measure the time to read analog pin with varying prescale
499:loop:info:prescale(128):bits(10):analogPin.sample():112 us
499:loop:info:prescale(64):bits(9):analogPin.sample():56 us
499:loop:info:prescale(32):bits(8):analogPin.sample():30 us
499:loop:info:prescale(16):bits(7):analogPin.sample():17 us
499:loop:info:prescale(8):bits(6):analogPin.sample():10 us
499:loop:info:prescale(4):bits(5):analogPin.sample():7 us
499:loop:info:prescale(2):bits(4):analogPin.sample():5 us

Circuit
-------
This example requires no special circuit. 




