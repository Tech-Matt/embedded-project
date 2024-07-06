# embedded-project
Trying to build a simple HAL in Rust for MSP432 with API as close as possible to the C DriverLib API. 

## Embedded rust book pointer
- chapter 2.0

## Dependencies
- rustup target add thumbv7em-none-eabihf for Cortex M4F (with floating point unit)
- Rust toolchain
- build-essential (in ubuntu)
- cargo install cargo-binutils
- libssl-dev
- pkg-config
- cargo install cargo-generate
- sudo apt install gdb-multiarch openocd qemu-system-arm

## Setting Up udev Rules for MSP432 on Linux

### Identify the Device
1. Connect your MSP432 board to your computer via USB.
2. Open a terminal and run:
   ```sh
   dmesg | grep tty

Look for entries related to the MSP432 (e.g., /dev/ttyACM0).

    Alternatively, use lsusb to list USB devices:

    sh

lsusb

Identify your MSP432 device from the list. For example:

yaml

    Bus 002 Device 004: ID 0451:bef3 Texas Instruments, Inc.

Create a udev Rule

    Create a new udev rules file:

    sh

sudo nano /etc/udev/rules.d/99-msp432.rules

Add the following rule (replace XXXX and YYYY with the Vendor ID and Product ID from lsusb output):

sh

SUBSYSTEM=="usb", ATTR{idVendor}=="XXXX", ATTR{idProduct}=="YYYY", MODE="0666", GROUP="plugdev"

Example for Vendor ID 0451 and Product ID bef3:

sh

    SUBSYSTEM=="usb", ATTR{idVendor}=="0451", ATTR{idProduct}=="bef3", MODE="0666", GROUP="plugdev"

Reload udev Rules

    Reload the udev rules:

    sh

    sudo udevadm control --reload-rules

Apply Changes

    Unplug and replug your MSP432 device.

Verify Permissions

    Verify that the device permissions are correctly set:

    sh

ls -l /dev/ttyACM0  # Replace /dev/ttyACM0 with the correct device file

Ensure that the device file has the correct permissions and is part of the plugdev group.
