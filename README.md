# FishyFramework
This repo holds the embedded software for several of SSI's PCBs. It uses PlatformIO to build a number of frameworks and targets. [Tim Vrakas](https://github.com/Timvrakas) is currently the principal developer, reach out on [Slack](https://ssi-teams.slack.com/messages/timv) or by Email. 

## Boards

### SpaceSalmon
[SpaceSalmon](https://wiki.stanfordssi.org/Space_Salmon) is a flight computer, designed by Tim Vrakas for the Stanford Student Space Initiative in 2018. Due to COVID-19, support for this hardware has not been well maintained.

### ThunderGuppy
[ThunderGuppy](https://wiki.stanfordssi.org/ThunderGuppy) is a hardware revision based on the SpaceSalmon architecture. It has similar hardware to SpaceSalmon, but is designed to fit in a 2" airframe. It is currently well supported.

### Quail
[Quail](https://ssi-wiki.stanford.edu/Quail) is an EGSE control system being developed in 2021 for SSI's liquid propulsion program. It is currently under active development.
#### TODOs
* SHITL - It would be great if we could get Hardware-In-The-Loop testing for Quail. As is we have the Travis CL which doesn't do much. SHITL would allow us to quickly verify that new revisions work with actual hardware. We could use the existing TG SHITL as a guide.
* Dependencies - All libraries have their own library.json. I don't think these are all currently tracked via git. This makes modularity difficult as you have to edit both the platformio.ini file and all the individual jsons. A fix for this would be useful for the whole framework.
* Slate - Taring and units. These can be added via SensorSlate.
* Documentation - It's rather awful right now. At the very least we should go through and comment the header files. We can also add inline comments on the cpp files as needed.
* Memory Management - RAM is sparse. We need to verify that tasks aren't hogging up more memory than is absolutely necessary. We should also make sure we aren't doing things inefficiently. Ex) Dont pass jsons between tasks and serialize a bunch of times. That requires statically allocating a bunch of memory. Instead pass char*'s. 
* Squibs - Not a huge deal since we're only using one Squib, but for some reason we can't do two. Not a clue a why.
* Harware Verification - We haven't had completely working Quails in a fair bit. If everything on this branch still interfaces as expected it would be a miracle. We need to get started on another testing cycle. At the very least we should get a cold flow with the new board/software going ASAP.
* Standardized Logic - The current paradigm is very Slate-centric. There are two main types of Slate Endpoints: SensorSlate and ActuatorSlate. SensorSlate variables are uneditable. Quail tells us what these are. ActuatorSlate variables are set by the user. There should be no gaps in the logic between setting ActuatorSlate variables and updating physical hardware. To accomplish this part of ActuatorSlate updates should include an update call to some task. We should ensure that this paradigm is respected throughout the architecture.
* Sequencing - It's gotta be rewritten. It's currently wayyy to memory intensive.
* Configs - I'd like to store all hardware defaults and software shortcuts (ie: things that aren't quite Slate variables) in a central config file. We should ensure that this paradigm is respected throughout the architecture. Part of the config file should be a rundown of all the error types, no more print statements.
* DEF - For modularity's sake, let's introduce some more #ifdefs throughout. They're really useful.

## Running the Framework

### How To Setup The Build environment
* Install VSCode
* Add the extension Platform IO in the extension tab on the left
* Using git, clone the repo: `git clone https://github.com/stanford-ssi/SpaceSalmon.git`
* Open the repo in VSCode
* Use the check button in the bottom bar of the VSCode window to build
* Use the right arrow button next to the build button to upload

### Uploading
If BOSSAC uploading fails, use this:  
`python uf2conv.py -b 0x4000 .pio/build/XXX/firmware.bin`  

### Lessons Learned
* You can't write data into a buffer into another task, beacause you don't know that its constructor has been initialized! The main case is logging. Don't try logging from the constructors! We should probably prevent the logger from taking input before its initialized...
* Libraries work best when they don't export a bunch of header files. The internal header structure should use relative include paths, and careful attention should be paid to what headers end up in the project search path.
* Sometimes if you have too many static things, stuff goes very wrong. Particularly really big StrBuffers? idk.

### SHITL
The code supports SHITL (Some Hardware in the Loop) testing. This allows you to feed a log of sensor data from a past flight into the hardware in the place of live sensor data. To perform SHTIL you must do the following. The logs from a SHITL test are treated the same as flight logs.
* Enable the `sys.shitl` boolean flag, which will require you to rebuilding and reflash the firmware. The blinking pattern will be noticeably different than when running off sensor data. In SHITL mode, the reading of sensor data from the SD card will illuminate the Sensor LED.
* Place a `shitl.txt` file on the SD card. You may trim from both ends of a flight log to make the SHITL test faster.
Under SHITL, the system runs at ~180% speed, which is limited by SD access and CPU time. Sensor data is not re-logged, but filter data and flightPlan logs are printed for every data point provided.
After performing a SHITL, the output filter data should be compared to implementations in `sim/`, and the objectives of the flight plan should be verified (Test with real igniters!)
