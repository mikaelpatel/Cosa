CosaBenchmarkRTC
=================

Measurement and validation of the RTC (Real-Time Clock); micro- and
milli-second. The benchmark shows the number of micro-seconds required
for access of the RTC values, how accurate the DELAY macro and
Watchdog delay is.

CosaBenchmarkRTC: started
free_memory() = 1548
F_CPU = 16000000
I_CPU = 16
Watchdog::ms_per_tick() = 16
Watchdog::ticks() = 3
RTC::us_per_tick() = 1024
RTC::seconds() = 0
67:setup:info:RTC::micros(): 8 us
72:setup:info:RTC::millis(): 44 us
77:setup:info:DELAY(10): 12 us
82:setup:info:DELAY(100): 108 us
87:setup:info:SLEEP(1): 1067296 us
RTC::seconds() = 1
RTC::micros() = 1335080
RTC::millis() = 1362
RTC::micros() = 2436916
RTC::millis() = 2437
RTC::seconds() = 2
RTC::micros() = 3510220
RTC::millis() = 3510
RTC::seconds() = 3
RTC::micros() = 4583532
RTC::millis() = 4584
RTC::seconds() = 4
RTC::micros() = 5656704
RTC::millis() = 5657
RTC::seconds() = 5
RTC::micros() = 6729924
RTC::millis() = 6730
RTC::seconds() = 6
RTC::seconds() = 17
115:setup:info:DELAY(100): 100000 measurement/validation (err = 0)
RTC::seconds() = 27
132:setup:info:RTC::delay(100): 100 measurement/validation (err = 0)
RTC::seconds() = 37
149:setup:info:Watchdog::delay(100): 100 measurement/validation (err = 0)

Circuit
-------
This example requires no special circuit. 