This folder contains files for controlling the ODD.

The 96 channels are arranged as 4 groups of 24 channels, each comprising 3 'bytes' (referred to as 'ports' of data lines).

Ports 0-7 operate PCBs 0-7, 9 is left open on the back plane inside the control box, and 9-11 are available on the front panel. During initialization, the control software sets ports 0-7, 8, 9 for output and ports 10, 11 for input. Currently, pin one on port 9 switches to HIGH for the duration of any odor presentation. 
