CosaFSMBenchmark
================

The classical finite state machine benchmark with measurement of event
send time. The machine has only one state and for each received event
sends an event to a connected machine. 

The measurement contains the pushing of the event onto the event
queue, pulling and dispatch of the event to the receiving state
machine.  

CosaFSMBenchmark: started  
free_memory() = 1712  
sizeof(FSM) = 12  
sizeof(Echo) = 14  
F_CPU = 16000000  
I_CPU = 16  
EVENTS_MAX = 100000  
124:loop:info:25 us per event (400 cycles)  
124:loop:info:25 us per event (400 cycles)  

Circuit
-------
This example requires no special circuit. 




