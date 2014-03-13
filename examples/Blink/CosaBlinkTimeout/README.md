CosaBlinkTimeout
================

The classic LED blink demonstration written for an RGB LED using three
difference time periods for the different colors. This example shows
how to set an event handler for Pin objects and attach them to
different Watchdog time period queues. When the time period is
exceeded the Watchdog interrupt handler will push a timeout event to
the time period queue (which is a Thing set; Things). The timeout
event is then dispatched which will boardcast the timeout event to all
the attached event handlers. 

Circuit
-------
Connect an RGB LED to pins (5, 6, 7).