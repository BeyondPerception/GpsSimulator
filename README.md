# GpsSimulator
A Qt frontend for creating gps simulations and transmitting them with an SDR.

# Setup
The following is a guide to exactly replicate the GPS Simulator appliance.

## Setting up the software environment.
This setup guide assumes you are using a raspberry pi 4B with a user named `pi`. A HackRF One SDR should be plugged in to one of the USB ports, and a GPS receiver should be connected over GPIO pins.

### OS Setup
Go to this [link](https://www.raspberrypi.org/software/operating-systems/) and download the Raspberry Pi OS Lite image. Write the image to a microSD card using your favorite method (dd, rufus, etc.)

Boot the raspberry pi and login (default username: `pi`, default password: `raspberry`), and enter the config screen using `sudo raspi-config`. Complete the following steps:

- `System Options > Wireless LAN` to setup wifi, and follow any propmpts.
- `System Options > Boot / Auto Login` and select `Console Autologin`.
- `Interface Options > SSH` and enable the SSH server.
- `Interface Options > Serial Port` and select `No` for `Would you like a login shell to be accessible over serial?`, and then `Yes` to `Would you like the serial hardware to be enabled?`
- Set `Localisation Options` if desired.
- Finally, select `Finish` and choose `Yes` to the reboot prompt.

Next, edit the file `/etc/security/limits.conf` and add the following line to the end of the file:

`pi        - rtprio - max realtime priority`

Ensure you log out of the `pi` account and log back in before using the appliance.

### Dependencies

Run `sudo apt update`, then install the following packages with `sudo apt install <package-name>`
- hackrf
- gpsd
- git
- cmake
- fluxbox
- xserver-xorg
- xinit
- qt5-default

Setup gps-sdr-sim using the following commands:

```
$ git clone https://github.com/osqzss/gps-sdr-sim ~/gps-sdr-sim
$ cd ~/gps-sdr-sim
$ gcc gpssim.c -lm -O3 -o gps-sdr-sim
```

### Gui Setup


### Firmware Update

The HackRF One board needs a firmware patch to function properly. Navigate to [this](https://github.com/BeyondPerception/hackrf) repository and follow the instructions. This can be done on an entirely seperate computer from the raspberry pi, or the `hackrf_usb.bin` file can be transfered to the pi to be flashed on to the board.

### Building this project