# VVNZRunner
Kernel plugin for PS Vita that provides API to run Toshiba MeP code on Venezia.
Original idea from PS Vita Henkaku wiki [article](https://wiki.henkaku.xyz/vita/Venezia).

VVNZRunner provides safe way to run MeP code on Venezia processor and functions to transfer data from and to Venezia SPRAM.

# V-Thread support

V-Threads are threads that run on Venezia cores. Up to 6 cores can run tasks simultaneously in normal mode and up to 7 in extended mode (enabled with sceCodecEngineChangeNumWorkerCoresMax()). All tasks that fall outside of limit will be automatically scheduled for execution. Note that even when running V-Thread, vnzBridgeExec() is blocking on ARM side. Refer to sample_mep for V-Thread creation procedure.

# Examples

Example of userland application is in sample folder.

Example of MeP code used in the sample application is in sample_mep folder. To compile MeP code use [mepsdk](https://github.com/TeamMolecule/mepsdk).

# Usage notes

- Safe spram offset is 0x1400 bytes. Everything below belongs to Venezia context and will be reset upon execution.
- For now code memory size is limited to 0x624 bytes.


