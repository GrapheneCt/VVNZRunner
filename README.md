# VVNZRunner

Kernel plugin for PS Vita that provides API to run Toshiba MeP code on Venezia.
Original idea from PS Vita Henkaku wiki [article](https://wiki.henkaku.xyz/vita/Venezia).

VVNZRunner provides safe way to run MeP code on Venezia processor and functions to transfer data from and to Venezia SPRAM.

![alt text](https://github.com/GrapheneCt/VVNZRunner/blob/main/scr.PNG?raw=true)

# V-Thread support

V-Threads are threads that run on Venezia cores. Up to 6 cores can run tasks simultaneously in normal mode and up to 7 in extended mode (enabled with sceCodecEngineChangeNumWorkerCoresMax()). All tasks that fall outside of limit will be automatically scheduled for execution. Note that even when running V-Thread, vnzBridgeExec() is blocking on ARM side. Refer to sample_mep for V-Thread creation procedure.

# Example: stb_dxt encoder

Example features Venezia port of stb_dxt encoder. It encodes input RGBA8888 file to DDS DXT1. Some code has been taken from [here](https://github.com/imgdrop/dds.js/blob/038d7d9518e1d19cbec94a91e7d3d4d39cfab948/encode/dds-enc.c).

To compile MeP code use [mepsdk](https://github.com/TeamMolecule/mepsdk).

Check CMakeLists for flags used to compile MeP code for Venezia.

# Usage notes

- Custom mep code must use special linker script with location counter set to base injection address (0x8E3790 by default). There is example in sample_mep.
- Safe spram offset is 0x1404 bytes. Everything below belongs to Venezia context and will be reset upon execution.


