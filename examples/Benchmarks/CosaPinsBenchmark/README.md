CosaPinsBenchmark
=================

Measurement of the pins operations; digital/analog input pin read,
digital output pin write and toggle. 

Reading a digital pin in Cosa is 8X faster than Arduino. Writing is
4-12X faster. Reading an analog pin in Cosa is equal to Arduino.

      CosaPinsBenchmark: started  
      free_memory() = 1687  
      sizeof(Event::Handler) = 2  
      sizeof(InputPin) = 4  
      sizeof(OutputPin) = 4  
      sizeof(AnalogPin) = 9  
      65:setup:info:Arduino: 4143 us per 1000 digitalRead(7)  
      73:setup:info:Cosa: 565 us per 1000 inPin.is_set()  
      81:setup:info:Cosa: 502 us per 1000 InputPin::read(7)  

      92:setup:info:Arduino: 8222 us per 1000 digitalWrite(8, 1); digitalWrite(8, 0)  
      101:setup:info:Cosa: 4959 us per 1000 outPin.write(1); outPin.write(0)  
      110:setup:info:Cosa: 2260 us per 1000 outPin.set; outPin.clear()  
      119:setup:info:Cosa: 6465 us per 1000 OutputPin::write(8, 1); OutputPin::write(8, 0)  

      128:setup:info:Arduino: 8191 us per 1000 digitalWrite(8, !digitalRead(8))  
      136:setup:info:Cosa: 3641 us per 1000 outPin.write(!outPin.read())  
      144:setup:info:Cosa: 2009 us per 1000 outPin.is_set/clear/set()  
      152:setup:info:Cosa: 2009 us per 1000 outPin.set/is_clear()  
      160:setup:info:Cosa: 1130 us per 1000 outPin.toggle()  
      168:setup:info:Cosa: 3578 us per 1000 OutputPin::write(8, !OutputPin::read(8))  
      179:setup:info:Cosa: 3672 us per 1000 OutputPin::read/write(8,0/1)  
      187:setup:info:Cosa: 691 us per 1000 OutputPin::toggle(8)  

      200:setup:info:Arduino: 15 us per bit data transfer() digitalWrite()  
      212:setup:info:Cosa: 7 us per bit data transfer() pin.write()  
      224:setup:info:Cosa: 4 us per bit data transfer() pin.write/toggle()  
      236:setup:info:Cosa: 11 us per bit data transfer() OutputPin::write()  
      248:setup:info:Cosa: 4 us per bit data transfer() OutputPin::write/toggle()  
      279:setup:info:Cosa: 3 us per bit data transfer() pin.write/toggle() unrolled  

      286:setup:info:Arduino: 112 us per analogRead()  
      292:setup:info:Cosa: 112 us per analogPin.sample()  
      298:setup:info:Cosa: 112 us per AnalogPin::sample()  

Circuit
-------
This example requires no special circuit. 




