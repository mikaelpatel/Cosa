CosaPinsBenchmark
=================

Measurement of the pins operations; digital/analog input pin read,
digital output pin write and toggle. 

Reading a digital pin in Cosa is 8X faster than Arduino. Writing is
4-12X faster. Reading an analog pin in Cosa is equal to Arduino.

      CosaPinsBenchmark: started  
      free_memory() = 1740  
      sizeof(InputPin) = 6  
      sizeof(OutputPin) = 6  
      sizeof(AnalogPin) = 10  
      58:setup:info:Arduino: 4087 us per 1000 digitalRead(7)  
      66:setup:info:Cosa: 566 us per 1000 inPin.is_set()  
      74:setup:info:Cosa: 503 us per 1000 InputPin::read(7)  

      85:setup:info:Arduino: 8237 us per 1000 digitalWrite(8, 1); digitalWrite(8, 0)  
      84:setup:info:Cosa: 4968 us per 1000 outPin.write(1);outPin.write(0)  
      103:setup:info:Cosa: 2264 us per 1000 outPin.set;	outPin.clear()  
      112:setup:info:Cosa: 6477 us per 1000 OutputPin::write(8, 1); OutputPin::write(8, 0)  

      121:setup:info:Arduino: 8206 us per 1000 digitalWrite(8, !digitalRead(8))  
      129:setup:info:Cosa: 3647 us per 1000 outPin.write(!outPin.read())  
      137:setup:info:Cosa: 1132 us per 1000 outPin.toggle()  
      145:setup:info:Cosa: 3584 us per 1000 OutputPin::write(8, !OutputPin::read(8))  
      153:setup:info:Cosa: 692 us per 1000 OutputPin::toggle(8)  

      166:setup:info:Arduino: 15 us per bit data transfer() digitalWrite()  
      178:setup:info:Cosa: 7 us per bit data transfer() pin.write()  
      190:setup:info:Cosa: 4 us per bit data transfer() pin.write/toggle()  
      202:setup:info:Cosa: 11 us per bit data transfer() OutputPin::write()  
      214:setup:info:Cosa: 4 us per bit data transfer() OutputPin::write/toggle()  
      245:setup:info:Cosa: 3 us per bit data transfer() pin.write/toggle() unrolled  

      252:setup:info:Arduino: 112 us per analogRead()  
      258:setup:info:Cosa: 112 us per analogPin.sample()  
      264:setup:info:Cosa: 112 us per AnalogPin::sample()  

Circuit
-------
This example requires no special circuit. 




