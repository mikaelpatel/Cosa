CosaButton
==========

Demonstration of Cosa debouncing of switch/buttons connected to an
input pin. The input pin object will receive falling/rising/changed
events from the button which will filter changes on the pin using low
frequency sampling (64 ms). 

Circuit
-------
A button/switch should be connected to Arduino pin 7 and ground. No
additional components are needed as the input pin is configured with
input pullup resistor. 

The Arduino Pin 13 (builtin LED) is used for on-off state.


