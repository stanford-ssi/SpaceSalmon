# ethernet demo

This project serves to implement a wrapper for the Ethernet driver on the Quail board. Features include the wrapping of provided API functions into tasks that can be assigned varying priority levels, each of which servign differing functionalities. Another objective of this project is to reduce overall clutter via streamlining includes into the dedicated SSIEth library.

# Motivation

In light of the use of the Quail board for firing rockets, there are several logistical and safety concerns that make Ethernet a more appealing choice as opposed to alternatives such as USB. These reasons shall be detailed below:

1) Better signal over Ethernet. Due to safety concerns surrounding rocket launches (I don't want my face blown off), we would like to gather data from the rocket pre-launch and during the launch process from a safe distance away. Ethernet has better signal transmission than USB, so we can make longer Ethernet cables, i.e. Ethernet cables come in longer lengths than USB cables. Hence, we would like to use Ethernet for our purposes, as we don't have USB cables in that size.

2) Better bandwidth. With Ethernet we can have multiple devices transmitting over the same Ethernet cable, as opposed to multiple cables which we will require for USB.

3) Ethernet radios. It is significantly easier to obtain Ethernet radios off the shelf for use with the rocket. Such radios can have ranges of up to several kilometers, which suits our purposes. On the other hand, USB radios with similar ranges are not readily available.

It is the combination of these three reasons that lead us to choose Ethernet over USB.

Additionally, while some basic API has been provided by the chip manufacturer, we are opting to write our own driver leveraging the provided API to give us a greater degree of customizability, as well as increased modularity for ease of use.

# Tasks implemented:

Webserver task: Enables connection to a website from which logging of data can be controlled.

Command task: Sets the Quail to wait for incoming commands via TCP (currently set to port 2002).

Sensor task: Sets the Quail to continuously broadcast sensor data via UDP (currently set to port 2001).

# General Difficulties Encountered

We ran into numerous issues building the driver. Due to the dependencies included in the code provided by the manufacturer, there were several libraries that were redefined during the build process which caused us to run into errors. At the same time, to maximise compatibility, we wanted to reduce the changes we made to the original libraries, so several workarounds had to be thought up.

Additionally, there were also issues in loading the code onto the board. We had to experiment with a vaariety of hardware and software options before coming up with a method that works. We have left basic diagnostic steps in the README should you find yourself plagued with similar misfortune.

We also ran into some issues with getting the connection to go through, though that arose primarily due to the fact that the API provided was not entirely clear on what each function did, and some slight confusion about how some of the transmission protocols worked.

Many of these issues were caused by a lack of documentation originally, so we have (hopefully) provided better documentation and clearer instructions in the README so that you won't have to go through the same shit we did.

# Interesting Problems/Tricky things

The hardware for our project had never been used before, so before we could make a cool implementation, we needed to verify that it was somewhat functional. The PHY link was working, because the lights were blinking on both ends of the cable. Our first task was to use the manufacturer (ATMEL) provided libraries to get a connection running on the hardware.

Then, we could restructure and integrate the ethernet code into the existing codebase.The biggest headache was probably #include issues with libraries we were trying to use. The ATMEL libraries are not well documented and are very hefty. I spent a long time just trying to get the various layers to compile.

The most tricky bug was a memory corruption issue related to the DEBUG() handler I provided to the IP stack. I implemented a wrapper that was using variadic arguments and calling _vnsprintf() to save to a buffer, and then printing the buffer over USB console. This was working, but it appears it was overwriting the buffer (even though we were providing the length to _vsNprintf()…) This was causing the pointers to some of the semaphores to get mangled. I narrowed down on the issue using the JTAG debugger, which can set watchpoints on memory, and halt execution when it’s modified, just like we did in class.

# Useful resources

Fortunately LWIP loves us enough to leave us a documentation website for their function calls, which were used a lot in the netconn api. Additionally, the manufacturer provides sample code for different applications as well, these serve as the foundation upon which some of our tasks would be built. I would recommend using them for reference or as a starting point.

## Laptop Configuration

-- see screenshot

## Graph Data

```python
python client.py 192.168.1.1 1000
```

## TODO

website start button
get a poe switch and demo with a security camera and the 100ft ethernet cable
write a couple of paragraphs - See above
