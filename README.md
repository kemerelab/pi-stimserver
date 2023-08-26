

# Setting up the Raspberry Pi

These instructions are adapted from [here](https://ubuntu.com/tutorials/how-to-install-ubuntu-on-your-raspberry-pi).

## I. Creating a new boot disk.

Using the Raspberry Pi Imager software, here are the steps:

  1. (This has been tested using Ubuntu Server 22.04. It probably will work for others.) Click on `Choose OS`, 
     then `Other general-purpose OS`, then `Ubuntu`, then `Ubuntu Server 22.04.03 LTS (64 bit)`.
  2. Click on `Choose Storage` and select the micro-SD card you have inserted into your computer (e.g., using a
     USB adaptor).
  3. **Important!** Don't click on `Write` yet!!!
  4. Click on the gear icon in the bottom right corner. In MacOS, it may ask if you want to prefill WiFi settings. 
     You can click `No`.
  5. Click the checkbox next to `Enable SSH`.
  6. Make sure the checkbox next to `Set username and password` is checked. Fill in the username and password 
     you will use.
  7. For our institutional WiFi, we need to configure it after the disk is created, so uncheck the box next to 
     `Configure Wireless LAN`.
  8. Check the box next to `Set locale settings` and choose your timezone and keyboard layout. (For our group, 
     it is `America/Chicago` and `us`.)
  9. Click on `Save`, and then `Write`, then `Yes` to erase media (assuming you've double checked you don't want
     whatever is on it anymore!).

## II. Setting up networking

The next steps are a bit complicated if you're using a Mac or Windows machine that can't mount the ext4 partitions
that are used by Linux (and are now on the formatted MicroSD card). I used Ubuntu running in Parallels for the next
step. You could also use a Linux desktop or install the appropriate software for your OS to let your computer read/write
the partitions. Alternatively, you can plug a monitor and keyboard into your Pi and do these steps after booting for the
first time.

Ubuntu Server uses "netplan" to configure networking. If you install a desktop version of Ubuntu or Raspbian,
you'll have a GUI to configure networking, but for this system, we want minimal processes running, and so the Server
version is preferred. To configure the networking, edit the file `/etc/netplan/50-cloud-init.yaml` file. Our goal is
to set up the hardwired Ethernet to be on a local network, unconnected to the broader Internet, and use the WiFi
for internet connectivity. Note that the comment in the file says that the configuration will not persist across 
instance reboots. This doesn't apply to the raspberry pi.


The file should thus look something like this when you're done:

```
# This file configures the network for a WiFi internet connection and a static Ethernet IP address.
# The static IP address 192.168.0.2/24 assumes that the Trodes server connecting to the Pi will have
# an address in the range of 192.168.0.xxx.

network:
  version: 2
  ethernets:
    eth0:
      dhcp4: no
      addresses: [192.168.0.2/24]
  wifis:
    wlan0:
      dhcp4: true
      access-points:
        "Rice Visitor": {}

```

Note that we've specified a particular example open network for the WiFi configuration which is relevant
to our specific scenario.

If you have edited this file after the first boot, you'll need to run the two commands: `sudo netplan generate`
and then `sudo netplan apply`.

You should now be able to access your Raspberry Pi on the local network using ssh - i.e., `ssh your-user-name@192.168.0.2`.
Once you log in, you should run `sudo apt update` and then `sudo apt upgrade` to update your system libraries.


## III. Installing the software

1. Prerequisite packages: `sudo apt install build-essential git libbcm2835-dev`.
2. Clone the pi-stimserver repository: `git clone https://github.com/kemerelab/pi-stimserver`
3. Run `sudo ./install.sh`, which will compile the code, copy it to `/usr/local/bin`, and install the systemd service.
   If the server installs correctly, the LED on the PCB should start flashing.

4. (Alternativel) You'll need access to the internet, but alternatively to steps 2 and 3, you can copy the `install.sh`, 
   `stimserver.c`, and `stimserver.service` files over to the Pi using a USB disk.

## IV. Installing the firmware onto the microcontroller

The easiest way to program the firmware onto the Pico microntroller before you plug the IO PCB into the Raspberry Pi. To do this, connect the Pico board using a micro-USB cable. Hold the button down as you plug it in, and it will show up as a USB drive called
`RPI-RP2`. Copy the firmware blob - [pico-firmware-blog/stim-pulser.uf2](pico-firmware-blog/stim-pulser.uf2) - into the drive
and if the programming is successful the drive will "unplug". You can then install the PCB onto the Raspberry Pi and reboot it. The
green LED on the Pico microntroller will flash when ever stimulation is triggered if the firmware is installed correctly. 


## V. (Optional) Compiling new firmware
### Setup for compiling (e.g. on my MacOS)
To compile the RP2040 firmware, we needed to:
 - Download the gcc-arm cross compiler here - https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads - and tar -xf the file somewhere
  - Download the SDK: `git clone https://github.com/raspberrypi/pico-sdk.git; cd pico-sdk; git submodule update --init;`
 - Install cmake (`brew install cmake`)
 - (Optional) Download the examples: `git clone https://github.com/raspberrypi/pico-examples.git`
 - Set some environmental variables:
```
 1024  export PICO_SDK_PATH="$HOME/Code/Pico/pico-sdk"
 1106  export PICO_TOOLCHAIN_PATH=/Users/ckemere/Code/arm-gnu-toolchain-12.3.rel1-darwin-arm64-arm-none-eabi
 1107  export PATH=$PATH:/Users/ckemere/Code/arm-gnu-toolchain-12.3.rel1-darwin-arm64-arm-none-eabi/bin
```

 - Compile the sdk (and examples). `cd pico-sdk; mkdir build; cd build; cmake ..; make`

### Compiling the firmware

  1. Change into the firmware directory, `pico-firmware-src`. 
  2. Make a new directory, e.g., `mkdir build`. 
  3. Change into this directory (`cd build`) and run cmake, `cmake ..`. (The `..` tells cmake to look at the CMakeLists.txt file in the src directory.)
  4. If cmake completes without error, you can run `make`. This will compile the code and generate the `stim-pulser.uf2` and `stim-pulser.elf` files.

## (Optional) Setup for programing a Pico using the debug interface on from a Raspberry Pi.
_from the [Getting Started Guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)_. In order to install OpenOCD, we needed the following packages:
`automake autoconf build-essential texinfo libtool libftdi-dev libusb-1.0-0-dev pkg-config`. 

Then we ran:
```
git clone https://github.com/raspberrypi/openocd.git --branch rp2040 --recursive --depth=1
cd openocd
./bootstrap
./configure --enable-ftdi --enable-sysfsgpio --enable-bcm2835gpio
make -j4
sudo make install
```

We connected SWDIO (the left debug pin when the usb connector is facing north) to the Pi GPIO 24 (pin 18) and SWCLK to Pi GPIO 25 (pin 22), with the center ground pin connected via a different Pico ground pin. (Note that there's other suggested pins, GPIO 14 and 15, which should be connected to enable UART interface during debugging.)

After copying the `.elf` file over to the Pi, we finally, we have the command `openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg -c "program stim-pulser.elf verify reset exit"`.
