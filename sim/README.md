This directory has data and scripts for testing and developing altitude filters.

## `./flight-data`
This is a collection of data from past flights. The .txt files are trimmed versions of the logs directly off the flight computers. The .pkl files are the same data (or a subset) but parsed for faster use in python scripts.

### 2019-05-04-Olympus-FAR
This was a very standard (and successful) sub-scale test of a 4" minimum diameter rocket on a Pro98 motor (not sure which one). It's a good test case for nominal use. The original data was sampled at 50Hz, but shortly after all the code was updated to 100Hz, so the "ddr" versions are Double Data Rate, with every other point interpolated to simulate 100Hz data.

### 2020-02-15-corkscrew
This was the infamous spin-stabilized SpaceShot sub-scale launch. We spun the rocket in a giant blender-like contraption, and lit it. It did not go well. But it's data!

## `./SHITL-cases`
These are scripts that are ready to be coppied onto an SD card to run as SHITL tests. They're basically pre-processed versions of the data from past flights.

Please add more data!
Please contribute your data!