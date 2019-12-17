# Space Salmon

Space Salmon is a a flight computer, designed by Tim Vrakas for the Stanford Student Space Initaive in 2018. This codebase provides datalogging, and eventually flight software development.

## ThunderGuppy
ThunderGuppy is the hardware version based on the SpaceSalmon architecure. It has similar hardware, but is designed to fit in a 2" airframe.

### How To Setup The Build environment
* Install VSCode
* Add the extension Platform IO in the extension tab on the left
* Using git, clone the repo at https://github.com/stanford-ssi/SpaceSalmon.git 
* Open the the repo in VSCode
* Follow the instructions in the "Weird Bugs Section" of the README
* Use the check button in the bottom bar of the VSCode window to build
* Use the right arrow button next to the build button to upload

### Weird Build Bugs
* You need to edit the `cstdio` file in the folowing location: `~/.platformio/packages/toolchain-gccarmnoneeabi/arm-none-eabi/include/c++/7.2.1/cstdio` and comment out these five `#undef` entries. They are not all next to each other in the file.  
//#undef printf  
//#undef sprintf  
//#undef vsprintf  
//#undef snprintf  
//#undef vsnprintf

### Uploading
If BOSSAC uploading fails, use this:  
`python uf2conv.py -b 0x4000 .pio\build\guppy\firmware.bin`  

### Lessons Learned
* You can't write data into a buffer into another task, beacuse you don't know that it's constructor has been intialized! The main case is logging. Don't try logging from the constructors! We should probably prevent the logger from taking input before its initialized...

### TODOs
* fix the `#undef` bug listed above
* fix the build version script, have it linked in last

### Task Priority
4 - Filter
3 - Sensor
2 - Altimeter
1 - Logger
