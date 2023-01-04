# A brief history of fishy telemetry interfaces

I'm taking this note on 12/29/2022, immedately following the merge of the codegen_slate development branch, which was a total rewrite of the Quail telemetry system. There have been several different telemetry systems used in this codebase over the years, and I'll try to recall each here, and hopefully carry forward some learnings from each.

## SkyBass, 2017-18
In 2017-2018, SkyBass printed tabulated data over USBSerial in CSV format. This was quick and dirty, but was impossible to read, inefficent, and not super fast to serialize. The logged telemetry was entirely different, it used an unformatted SD Card with a custom "filesystem" where sets of log entries were stored to flash blocks, with no lookup or addressing. This could be decoded by a python script on a Linux machine, and can only be described as "Joank". Flight telemetry was sent to a secondary MCU over a protocol called [i forgot what it's called...] and that system encoded telemetry as a packed struct of compressed values (modulus encoding and rounding to ints). Everything was hard coded on the ground.

## SpaceSalmon, 2019
In 2019, I got to impose some beauracratic structure onto the anarchy. Space Salmon relied heavily on JSON data structrures, and the same format was used for SD Logging and USBSerial. I added FatFs so we could have files, and that made life a lot nicer. We still used a secondary system for telemetry, on the top board in the SpaceStack, but because we never got CAN working between the boards, it only ever sent GPS data.

## ThunderGuppy/Stratochovy, 2020-2022
Recent ThunderGuppy and StratoChovy boards added an SX1262 LoRa module connected to the main flight computer. Making this work required a dedicated RFMAC state machine, which could coordinate 2-way comms between radios on the same frequency. This requrired significant tuning to get working well. Both of these board still log to USBSerial and SD in JSON, which can support 100Hz data rate. Flight telemetry is sent as packed structs of compressed values in a hand-coded layout. The ground radio is nearly identical hardware, and acts as a transparent bridge, encoding received packets in Base64 and wrapping them in JSON before sending over USBSerial. A Python Tk GUI handles logging and decoding of the packed data, and can be easily extended to support multiple different telemetry encodings (which must be selected manually). Uplink is very primitive, just a string, encoded base64, wrapped in JSON, sent over USBSerial, and transmitted over LoRa. Arming/disarming is based entirely on parsing the first character of that string.

## Quail 1.0, 2019
Pre-2020 Quails used CSV logging over SD and USBSerial, as well as a cursed manual line entry commanding system over USB. There was no control GUI, and only Damian knew how to make it do the thing.

## Quail 3.0, 2021 (#fa6602d)
In late 2021, Quail 3.0 brought the system into the FishyFramework fold, and there was an intermediate telemetry system using JSON and application-specific command parsers. The TXTask would manually serialize data from all over the sysetm into a JSONObject, and several dozen command handlers would manually parse commands and deliver data to the correct tasks. All of these commands were over USBSerial, The LoRa implementation never got fleshed out. 

## Quail 3.0, Spring 2022 (#4e77649)
In Spring 2022, there was a real Quail software team (Tim, Ahmed, Michal, Max) for the first time, and with the addition of Ethernet support, there was a desire to have a better telemetry protocol. We made heavy use of C++ inheritance, recursive serialziation (to JSON), and structured the application code around the slate as a central data structure. We also introduced the concept of a metaslate, a data structure that extended the slate to provide low-rate metadata about channels and configuration. This system was very cool and when used with the Olympus-Groundsation Web GUI, was fully functional. However, it suffered from performance issues. It relied on the dynamic structure of JSON, which was slow to seralize, and consumed lots of memory, to the point that running averages had to be de-scoped to fit within 256kB. The use of C++ runtime polymorphisim also impacted performance, and led to lots of hard to understand bugs. It was in some ways elegant, but in others incredibly painful to work with.

Part of the issue is that we didn't properly isolate build-time metaprogramming and runtime polymorphisim. Almost all the C++ tricks we used to build the slate structure were hacks to allow us to use a declarative syntax within the code to build a complex application structured around slate entries. This is possible, but it's not a good idea: it's slow, it uses lots of memory, and the syntax tools of C++ aren't designed for it. One better approach would be to use C++ templates for build time metaprogramming. But that's very painful, and tends to cause hard to diagnose bugs, and it demands a level of C++ experience that few SSI members have.

## Quail 3.1, Summer 2022 (#d8609fd)
In denial about his graduation, Tim decided to rewrite a lot of the Quail telemetry system over the summer of 2022. The main goals were to reduce wasted memory to fit within a 192kB footprint, and to explore more powerful metaprograming using python and Jinja2. In addition to an overhaul of the Ethernet stack, this was a new slate from the ground up. In this new approach, a slate is defined in a YAML file in the codebase. A build-time script parses this file, and builds an encoding based on it's contents. A handfull of utility classes help standardize the network interface of the device. A fixed protobuf interface (not tied to the slate contents) is used to list all available slates, proivde metaslates on request, and direct flows of slate data to clients. The device presents a single unified commanding interface, which allows writing to a slate feild.

This system significanly improved the performance, both in speed and in memory use. All telemetry is encoded as primitive data types (floats and ints), and no JSON encoding needed. Because all the formats are fixed, there's much less logic needed to encode telemetry and decode commands. Meanwhile, the system is easier to use and understand. The python scripting allows powerful automation while generating code from the delarative YAML syntax. While the generated code is long and repetitive, it's also very straightforward, which makes debugging easy. The wrapper code uses C++17 tricks to provide a clean interface without duplucated boilerplate.

## So what remains?
so many things remain.
- The euponymonus gang has made a Pigeon. To start, it can fall back on the existing guppy-ground code, but to make good use of the Ethernet interface, it should use what quail uses, and more.
- The current system only handles cycle-rate, fixed, tabular data, i.e. a specific set of numbers that are telemered together, exactly once per application cycle. 
- We probably want to be able to handle higher-rate intermediate telemetry, where a sensor might record data at several kHz for a few seconds before turning off. The current system can't really handle that.
- We probably want to be able to handle arbitrary streams of data, perhaps explicitly packetized, perhaps not. If we could redirect stdout over a text stream, that would let us read `printf()` over Ethernet. If we want a radio to transparently deliver arbitrary binary packets over Ethernet (so like, Pidgeon), we don't currently have a way to do that either.
- Quail dosen't log to SD card right now. It probably makes sense to just log everything as it's sent over Ethernet, but in what format? More than one way to skin a cat.
- Quail is not very bandwidth constrained, so the packed struct of uncompressed data is plenty small. But over LoRa, we measure packets in bits, not bytes. We probably need to take a subset of the data in the slate (only the imporant stuff), compress it in a field-specific way, and send it over LoRa. How can we set that up in a sensible way? 
    - The data sent over LoRa is a compressed alias of the data in the slate. How do we handle that duplication?
    - Does the encoding use templates? 
    - Does it use the same system, or is radio telemetry a seperate system from ethernet telemetry, that gets tied in at the application level?
    - What about commanding? Can we use a similar scheme with assignment by slate offset?
- Not all devices have physical ethernet interfaces, but we could use USB Ethernet emulation to make boards appear as ethernet dongles when connected over USB. I'm not sure there's a huge use case but it seems hella cool. I think TinyUSB provides an LWIP interface?