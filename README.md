# Space Salmon

Space Salmon is a a flight computer, designed by Tim Vrakas for the Stanford Student Space Initaive in 2018. This codebase provides datalogging, and eventually flight software development.

### Weird Build Bugs
 * You need to edit `cstdio` in your arm-none-eabi, and comment out several `#undef` entries:  
 //#undef printf  
 //#undef sprintf  
 //#undef vsprintf  
 //#undef snprintf  
 //#undef vsnprintf  


### Sensor Plan
`SystemSensors` struct will have a set of sensor objects that define the SPI and CS configuration for each board
BMP and BMI libraries need to be re-writen as C++ classes so that they can co-exist.

