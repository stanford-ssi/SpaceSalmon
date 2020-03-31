## 2020 TODOs
 - Get SpaceSalmon Hardware working
 - Get Radio Telemetry working
 - Get CAN Working
 - Better Data/Filter math (multi-sensor?)

## Old TODOs
- Test Bus Messaging
- add event timer to sensorTask, to define timing
- make battery/adc threaded
- make squib threaded
- fix assert warnings
- enable ECC ram!

- Some heuristic for sensor health. A way to tell when the sensor is not giving realistic data.
- Some form of error detection, logging and recovery. Explore watchdog timers, an global error code buffer, or something
- Improve performance: Increase SPI clock speed, async SD interface
- I2C stack development: Work on piping the GPS data from the RF carrier, and telemetry to the radio
- Filter Development: used data from logging flight to develop altitude filter
- Flight Management: write code to manage chute deployment etc.