# RP2040-MC
Exercises in C with FreeRTOS for the RP2040 using either the Raspberry Pi Pico or the UNIT dualMCU board.
Minimum setup possible (so far) using CMake.

## Contents:
- `blink`: Blinks the Raspberry Pi Pico centinel led (build test).
- `tempC`: Reports the RP2040 core temperature via USB-RS232 adapter.
- `usblink`: Blinks the Raspberry Pi Pico centinel led. User defines sleep time via USB-RS232 adapter.

## Build:
1. Clone this repository, then
2. `cd rp2040`
3. `cd build`
4. `cmake .. -DPICO_SDK_FETCH_FROM_GIT=1 -DFREERTOS_FETCH_FROM_GIT=1`
5. `make`
6. `find . -type f -iname "*.uf2"`

Finally upload an `uf2` file to the RP2040.
