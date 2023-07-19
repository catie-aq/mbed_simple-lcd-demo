# Zest Display LCD Simple Demo
A simple example to demonstrate how to use the Zest_Display_LCD board.

## Requirements
### Hardware requirements
The following boards are required:
- Zest_Display_LCD
- Any Zest_Core board

### Software requirements
Zest Display LCD Simple Demo makes use of the following libraries (automatically
imported by `mbed deploy` or `mbed import`):
- [Ilitek 9163c library](https://github.com/catie-aq/mbed_ilitek-ili9163c)
- [LVGL](https://github.com/lvgl/lvgl)

## Usage
To clone **and** deploy the project in one command, use `mbed import` and skip to the
target enabling instructions:
```shell
mbed import https://gitlab.com/catie_6tron/zest-display-lcd-simple-demo.git zest-display-lcd-simple-demo
```

Alternatively:

- Clone to "zest-display-lcd-demo" and enter it:
  ```shell
  git clone https://gitlab.com/catie_6tron/zest-display-lcd-simple-demo.git zest-display-lcd-demo
  cd zest-display-lcd-demo
  ```

- Deploy software requirements with:
  ```shell
  mbed deploy
  ```

- Clone custom target repository if necessary:
  ```shell
  git clone YOUR_CUSTOM_TARGET_REPOSITORY your-custom-target
  ```

Define your target and toolchain:
```shell
cp your-custom-target/custom_targets.json . # In case of custom target
mbed target YOUR_MBED_OS_TARGET
mbed toolchain GCC_ARM
```

Compile the project:
```shell
mbed compile
```

Program the target device with a Segger J-Link debug probe and
[`sixtron_flash`](https://github.com/catie-aq/6tron-flash) tool:
```shell
sixtron_flash
```

Debug on the target device with the probe and Segger
[Ozone](https://www.segger.com/products/development-tools/ozone-j-link-debugger)
software.
