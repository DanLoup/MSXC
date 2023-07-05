======================
MSXC - An MSX emulator with the goal of allowing to tinker around with the MSX platform itself
======================

Requeriments
======================
I don't have a clue, but given it's an 3.58 Mhz machine being emulated, 
it probably runs on anything that can handle a modern version of windows

Current status
======================
It runs MSX 1 and 2 games, and supports the emulation of the basic MSX 1 system and mostly of the MSX 2 and SCC audio chip. 
The V9938 emulation is incomplete and a bit buggy, but can run most of the popular konami games at least.
It does support a disk drive emulation (read only at the moment).
Internally it supports a very configurable system implemented by completely generic devices (other than the Z80 CPU),
but it still don't expose this to the user yet.
Also none of the more advanced ideas are implemented yet as well.

Plans
=====================
The whole idea of this software is to not only allow you to create your own custom MSX configurations probably via the GUI itself, 
as to have some sort of scripting language that allows you to define hardware as well, and even a built in Z80 assembler to support custom hardware, software development etc..
