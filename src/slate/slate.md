# Telem Generation thing
This is a code generation script that takes a set of YAML specifications, each describining sets of chanelized data, and produces C++ headers to aid in sending telemetry packets.

### Slate
Each YAML file describes one slate, which is a fixed set of channels that are sent together from one device to another. A seralized slate has a fixed size and structure, and does not contain any channel metadata. A seralized slate consists of a header, including a timestamp, and an unique hash over the structure of the slate.

### Metaslate
We also generate a metaslate, which is a FlatBuffer encoding of the structure of the slate. This is embedded as a binary blob. When a client needs to interpret a Slate it has not seen before, it can query the source device for the hash it needs to decode, and the device can return the metaslate blob, which provides the details needed to decode the slate messages.

## Commanding Option 1: Command Queue
Have a command queue that just holds a bunch of commands to be executed at a future time and then executes them when that time comes. So to pulse a valve you send a command that immedately opens, and then queues a close command for a later time.

## Commanding Option 2: Controller
Have a controller to handle timed operations. You tell the controller to turn on for some time, and it performs the pulse.

__I think the right option is controllers, because that's what the system will be built on more fundementally.__

## Notes
- so for now the metaslate might be msgpack lol tooling is hard
- zlib compression on the msgpack was very effective!
- eventually should move to flatbuffer for metaslate...
- need to add data heirachy