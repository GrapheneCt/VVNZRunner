# VVNZRunner
Kernel plugin for PS Vita that provides API to run Toshiba MeP code on Venezia.
Original idea from PS Vita Henkaku wiki [article](https://wiki.henkaku.xyz/vita/Venezia).

VVNZRunner provides safe way to run MeP code on Venezia processor and functions to transfer data from and to Venezia SPRAM.

# Examples

Example of userland application is in sample folder.

Example of MeP code used in the sample application is in sample_mep folder. To compile MeP code use [mepsdk](https://github.com/TeamMolecule/mepsdk).

# Usage notes

- MeP code must always end with ```jmp 0x80063C``` instruction.
- Safe spram offset is 0x1400 bytes. Everything below belongs to Venezia context and will be reset upon execution.
- For now code memory size is limited to 0x624 bytes.


