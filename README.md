# Space Salmon

Space Salmon is a a flight computer, designed by Tim Vrakas for the Stanford Student Space Initaive in 2018. This codebase provides datalogging, and eventually flight software development.

## ThunderGuppy
ThunderGuppy is the hardware version based on the SpaceSalmon architecure. It has similar hardware, but is designed to fit in a 2" airframe.

### Weird Build Bugs
 * You need to edit the `cstdio` file in the folowing location: `~/.platformio/packages/toolchain-gccarmnoneeabi/arm-none-eabi/include/c++/7.2.1/cstdio` and comment out these five `#undef` entries. They are not all next to each other in the file.  
 //#undef printf  
 //#undef sprintf  
 //#undef vsprintf  
 //#undef snprintf  
 //#undef vsnprintf

 ### Uploading
 If BOSSAC uploading fails, use this:  
 `python uf2conv.py -b 0x4000 .pio\build\salmon\firmware.bin`  

 ### TODOs
 * fix the `#undef` bug listed above
 * fix the build version script, have it linked in last