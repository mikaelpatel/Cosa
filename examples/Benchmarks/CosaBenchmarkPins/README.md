CosaBenchmarkPins
=================

Measurement of the pins operations; digital/analog input pin read,
digital output pin write and toggle. 

Reading a digital pin in Cosa is 8X faster than Arduino. Writing is
4-12X faster. Reading an analog pin in Cosa is equal to Arduino.

      CosaBenchmarkPins: started  
      free_memory() = 1678  
      sizeof(Event::Handler) = 2  
      sizeof(InputPin) = 4  
      sizeof(OutputPin) = 4  
      sizeof(AnalogPin) = 9  
      68:setup:info:Arduino: 4151 us per 1000 digitalRead(7)  
      76:setup:info:Cosa: 566 us per 1000 inPin.is_set()  
      84:setup:info:Cosa: 503 us per 1000 InputPin::read(7)  

      95:setup:info:Arduino: 8239 us per 1000 digitalWrite(8, 1); digitalWrite(8, 0)  
      104:setup:info:Cosa: 4969 us per 1000 outPin.write(1);
      outPin.write(0)  
      113:setup:info:Cosa: 2264 us per 1000 outPin.set; outPin.clear()  
      122:setup:info:Cosa: 6478 us per 1000 OutputPin::write(8, 1); OutputPin::write(8, 0)  

      131:setup:info:Arduino: 8207 us per 1000 digitalWrite(8, !digitalRead(8))  
      139:setup:info:Cosa: 3648 us per 1000 outPin.write(!outPin.read())  
      147:setup:info:Cosa: 2013 us per 1000 outPin.is_set/clear/set()  
      155:setup:info:Cosa: 2013 us per 1000 outPin.set/is_clear()  
      163:setup:info:Cosa: 1132 us per 1000 outPin.toggle()  
      171:setup:info:Cosa: 3585 us per 1000 OutputPin::write(8, !OutputPin::read(8))  
      182:setup:info:Cosa: 3679 us per 1000 OutputPin::read/write(8,0/1)  
      190:setup:info:Cosa: 692 us per 1000 OutputPin::toggle(8)  

      203:setup:info:Arduino: 15 us per bit data transfer() digitalWrite()  
      215:setup:info:Cosa: 7 us per bit data transfer() pin.write()  
      227:setup:info:Cosa: 4 us per bit data transfer() pin.write/toggle()  
      239:setup:info:Cosa: 11 us per bit data transfer() OutputPin::write()  
      251:setup:info:Cosa: 4 us per bit data transfer() OutputPin::write/toggle()  
      282:setup:info:Cosa: 3 us per bit data transfer() pin.write/toggle() unrolled  

      289:setup:info:Arduino: 112 us per analogRead()  
      295:setup:info:Cosa: 112 us per analogPin.sample()  
      301:setup:info:Cosa: 112 us per AnalogPin::sample()  

Circuit
-------
This example requires no special circuit. 




