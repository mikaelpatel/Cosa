CosaPinsBenchmark
=================

Measurement of the pins operations; digital/analog input pin read,
digital output pin write and toggle. 

Reading a digital pin in Cosa is 8X faster than Arduino. Writing is
4-12X faster. Reading an analog pin in Cosa is equal to Arduino.

CosaPinsBenchmark: started  
free_memory() = 1760  
sizeof(InputPin) = 6  
sizeof(OutputPin) = 6  
sizeof(AnalogPin) = 10  
56:setup:info:Arduino: 4087 us per 1000 digitalRead(7)  
64:setup:info:Cosa: 566 us per 1000 inPin.is_set()  
72:setup:info:Cosa: 503 us per 1000 Pin::is_set(7)  

83:setup:info:Arduino: 8237 us per 1000 digitalWrite(8, 0); digitalWrite(8, 1)  
92:setup:info:Cosa: 2264 us per 1000 outPin.write(0); outPin.write(1)  
101:setup:info:Cosa: 2264 us per 1000 outPin.set; outPin.clear()  
109:setup:info:Cosa: 1132 us per 1000 outPin.toggle()  
118:setup:info:Cosa: 1069 us per 1000 OutputPin::write(8, 0); OutputPin::write(8, 1)  
126:setup:info:Cosa: 692 us per 1000 OutputPin::toggle(8)  

133:setup:info:Arduino: 112 us per analogRead()  
139:setup:info:Cosa: 112 us per analogPin.sample()  
145:setup:info:Cosa: 112 us per AnalogPin::sample()  

Circuit
-------
This example requires no special circuit. 




