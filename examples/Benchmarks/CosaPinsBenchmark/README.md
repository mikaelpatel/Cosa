CosaPinsBenchmark
=================

Measurement of the pins operations; digital/analog input pin read,
digital output pin write and toggle. 

Reading a digital pin in Cosa is 8X faster than Arduino. Writing is
4-8X faster. Reading an analog pin in Cosa is equal to Arduino.

CosaPinsBenchmark: started  
free_memory() = 1762  
sizeof(InputPin) = 6  
sizeof(OutputPin) = 6  
55:setup:info:Cosa: 566 us per 1000, is_set()  
63:setup:info:Arduino: 4150 us per 1000, digitalRead()  
73:setup:info:Cosa: 2264 us per 1000, set()-clear()  
83:setup:info:Arduino: 8237 us per 1000, digitalWrite(1)-digitalWrite(0)  
92:setup:info:Cosa: 1132 us per 1000 toggle()  
99:setup:info:Cosa: 112 us per sample()  
106:setup:info:Arduino: 112 us per analogRead()  

Circuit
-------
This example requires no special circuit. 




