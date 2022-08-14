## 2022 platform TODOs:
 - implement some kind of C++ function closures
 - get testing working
 - get PIO uploading working
 - get asserts to be reasonable

## Quail 2021 Bringup TODOs:
 - Get SAME54N20A chip support set up (currently using similar chip number)
 - Write IO abstractions for Quail 3.0 Hardware
 - Set up AD7124-8 driver
 - Get Ethernet working

## 2021 General TODOs:
 - Get SpaceSalmon Hardware working
 - Get CAN Working
 - Better Data/Filter math (multi-sensor?)

## Old TODOs
 - Some heuristic for sensor health. A way to tell when the sensor is not giving realistic data.
 - Some form of error detection, logging and recovery. Explore watchdog timers, an global error code buffer, or something
 - Improve performance: Increase SPI clock speed, async SD interface
 - I2C stack development: Work on piping the GPS data from the RF carrier, and telemetry to the radio
 - Filter Development: used data from logging flight to develop altitude filter
 - Flight Management: write code to manage chute deployment etc.