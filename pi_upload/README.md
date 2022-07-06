### Pi upload
--

The goal of this tool is to enable remote code upload and debugging of a board. This way you don't have to carry the board everywhere with you to run the code on hardware - you can just leave it at home and connect using this method.

Currently I don't support debugging over serial but its trivial to add. I've only used this with quail which uses ethernet to comunicate and the pi forwards the ethenet connection for remote debugging.


### Usage
--
After compiling the code you should be able to upload to the remote board using this command on your laptop. (Change the path to the binary for the specific board you're using). 

```./pi_upload/up.sh .pio/build/quail/firmware.bin```

Make sure to compile your code first!

### Setup
--

- Make it so that you can ssh to the pi using a private key
- Git clone this repo onto the pi
- Edit `pi_upload/up.sh` on your laptop to have the pi's hostname and repo location
- Connect the board to the pi using usb (for code upload) and ethernet (for debugging)
- Solder the board's reset pin the the raspberry pi's pin 21
- Install `usbmount` using `sudo apt install usbmount` to automount the board
- Follow the instruction in [this repo](https://github.com/stanfordroboticsclub/RemoteVPN) to setup ZeroTier to access the pi from anywhere in the world
- Optionally enable raspi-config's overlayfs for extra reliability