CosaBenchmarkFSM
================

The classical finite state machine benchmark with measurement of event
send time. The machine has only one state and for each received event
sends an event to a connected machine. 

The measurement contains the pushing of the event onto the event
queue, pulling and dispatch of the event to the receiving state
machine.  

	CosaBenchmarkFSM: started   
	free_memory() = 1536   
	sizeof(Event::Handler) = 2   
	sizeof(Link) = 6   
	sizeof(FSM) = 12   
	sizeof(Echo) = 14   
	EVENTS_MAX = 100000   
	F_CPU = 16000000   
	I_CPU = 16   
	134:loop:info:11 us per event (176 cycles)   
	134:loop:info:11 us per event (176 cycles)   
	134:loop:info:11 us per event (176 cycles)   
	134:loop:info:11 us per event (176 cycles)   
	134:loop:info:11 us per event (176 cycles)   
	
Circuit
-------
This example requires no special circuit. 

	


