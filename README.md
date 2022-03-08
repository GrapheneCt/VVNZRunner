# VVNZRunner

Kernel plugin for PS Vita that provides API to run Toshiba MeP code on Venezia.
It supports execution of MeP ELFs directly from userland-allocated memblock.

Original idea from PS Vita Henkaku wiki [article](https://wiki.henkaku.xyz/vita/Venezia).

VVNZRunner provides safe way to run MeP code on Venezia processor and functions to transfer data from and to Venezia SPRAM.

![alt text](https://github.com/GrapheneCt/VVNZRunner/blob/main/scr.PNG?raw=true)

# V-Thread support

V-Threads are threads that run on Venezia cores. Up to 6 cores can run tasks simultaneously in normal mode and up to 7 in extended mode (enabled with sceCodecEngineChangeNumWorkerCoresMax()). All tasks that fall outside of limit will be automatically scheduled for execution. Note that even when running V-Thread, vnzBridgeExec() is blocking on ARM side. Refer to sample_mep for V-Thread creation procedure.

# Venezia memory management system

Venezia uses specific memory management system for all external memory. You must use it for any memory passed to Venezia from ARM side to ensure cache consistency. More info can be found [here](https://github.com/GrapheneCt/VVNZRunner/blob/main/sample_mep/include/vnz/memory.h).

# Sample

Full sample coming soon

To compile MeP code use [mepsdk](https://github.com/TeamMolecule/mepsdk).

Check CMakeLists for flags used to compile MeP code for Venezia.

# Usage notes

- Using global variables in MeP elf (or anything else that will end up in data segment) is not allowed. Workaround for this is to pass custom memblock to Venezia that will be used to store static variables


