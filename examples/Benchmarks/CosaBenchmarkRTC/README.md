CosaBenchmarkRTC
=================

Measurement and validation of the RTC (Real-Time Clock); micro- and
milli-second. The benchmark shows the number of micro-seconds required
for access of the RTC values, how accurate the DELAY macro and
Watchdog delay is.

CosaBenchmarkRTC: started
free_memory() = 1574
F_CPU = 16000000
I_CPU = 16
Watchdog::ms_per_tick() = 16
Watchdog::ticks() = 3
RTC::us_per_tick() = 1024
RTC::seconds() = 0
67:setup:info:RTC::micros(): 8 us
72:setup:info:RTC::millis(): 4 us
77:setup:info:DELAY(10): 12 us
82:setup:info:DELAY(100): 100 us
RTC::seconds() = 0
RTC::seconds() = 1
RTC::seconds() = 2
RTC::seconds() = 3
RTC::seconds() = 4
RTC::seconds() = 14
106:setup:info:DELAY(100): 100000 measurement/validation (err = 0)
RTC::seconds() = 24
123:setup:info:Watchdog::delay(100): 100 measurement/validation (err = 0)
RTC::seconds() = 34
140:setup:info:RTC::delay(100): 100 measurement/validation (err = 0)

Circuit
-------
This example requires no special circuit. 