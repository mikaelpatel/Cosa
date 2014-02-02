
CosaBenchmarkUART
=================

Benchmarking IOStream and UART functions; measure time to print
characters and numbers through the IOStream interface and IOBuffer
to the UART.

CosaBenchmarkUART: started
free_memory() = 1570
1
one character:16 us
1
one character string:60 us
1
integer:76 us
1
long integer:100 us

10
two characters:52 us
10
two character string:68 us
10
integer:100 us
10
long integer:148 us

100
three characters:60 us
100
three character string:72 us
100
integer:116 us
100
long integer:192 us

127
int8_t:116 us
32767
int16_t:172 us
2147483647
int32_t:532 us

255
uint8_t:116 us
65535
uint16_t:160 us
4294967295
uint32_t:536 us

Circuit
-------
This example requires no special circuit. 




