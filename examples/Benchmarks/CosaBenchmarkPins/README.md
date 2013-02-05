CosaBenchmarkPins
=================

Measurement of the pins operations; digital/analog input pin read,
digital output pin write and toggle. 

Reading a digital pin in Cosa is 6-7X faster than Arduino. Writing is
2-10X faster. Reading an analog pin in Cosa is equal to Arduino.

      CosaBenchmarkPins: started  
      free_memory() = 1673  
      sizeof(Event::Handler) = 2  
      sizeof(InputPin) = 4  
      sizeof(OutputPin) = 4  
      sizeof(AnalogPin) = 9  
      70:setup:info:Loop: 503 us per 1,000 nop loops  
      81:setup:info:Arduino: 4151 us per 1000 digitalRead(7)  
      91:setup:info:Cosa(6X): 629 us per 1000 inPin.is_set()  
      102:setup:info:Cosa(6X): 629 us per 1000 inPin >> var  
      112:setup:info:Cosa(7X): 566 us per 1000 InputPin::read(7)  

      124:setup:info:Arduino: 8302 us per 1000 digitalWrite(8, 1); digitalWrite(8, 0)  
      135:setup:info:Cosa(1X): 5032 us per 1000 outPin.write(1); outPin.write(0)  
      146:setup:info:Cosa(3X): 2327 us per 1000 outPin.set; outPin.clear()  
      157:setup:info:Cosa(3X): 2327 us per 1000 outPin << 1; outPin << 0  
      168:setup:info:Cosa(1X): 6541 us per 1000 OutputPin::write(8, 1); OutputPin::write(8, 0)  

      179:setup:info:Arduino: 8270 us per 1000 digitalWrite(8, !digitalRead(8))  
      189:setup:info:Cosa(2X): 3711 us per 1000 outPin.write(!outPin.read())  
      199:setup:info:Cosa(3X): 2076 us per 1000 outPin.is_set/clear/set()  
      211:setup:info:Cosa(3X): 2076 us per 1000 outPin >> var; outPin << !var  
      221:setup:info:Cosa(3X): 2076 us per 1000 outPin.set/is_clear()  
      231:setup:info:Cosa(6X): 1195 us per 1000 outPin.toggle()  
      241:setup:info:Cosa(2X): 3648 us per 1000 OutputPin::write(8, !OutputPin::read(8))  
      254:setup:info:Cosa(2X): 3742 us per 1000 OutputPin::read/write(8,0/1)  
      264:setup:info:Cosa(10X): 755 us per 1000 OutputPin::toggle(8)  

      279:setup:info:Arduino: 15 us per bit data transfer() digitalWrite()  
      293:setup:info:Cosa(2X): 7 us per bit data transfer() pin.write()  
      307:setup:info:Cosa(3X): 4 us per bit data transfer() pin.write/toggle()  
      321:setup:info:Cosa(1X): 11 us per bit data transfer() OutputPin::write()  
      335:setup:info:Cosa(3X): 4 us per bit data transfer() OutputPin::write/toggle()  
      368:setup:info:Cosa(5X): 3 us per bit data transfer() pin.write/toggle() unrolled  

      376:setup:info:Arduino: 112 us per analogRead()  
      383:setup:info:Cosa(1X): 112 us per analogPin.sample()  
      392:setup:info:Cosa(1X): 112 us per analogPin >> var  
      399:setup:info:Cosa(1X): 112 us per AnalogPin::sample()  
	        
Circuit
-------
This example requires no special circuit. 




