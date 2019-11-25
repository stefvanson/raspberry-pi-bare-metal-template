# Raspberry Pi bare metal project template

## Purpose
Starting point to get something going quickly for Raspberry Pi 3B+ projects. It
includes:

- A UART driver for printing debug messages.
- MMU configuration (required for enabling cache, so major performance boost).
- A Doxyfile for automatic documentation generation.
- Tests and the `catch.hpp` header file.
- A Makefile to run all of the above. It constructs a binary for deploying with
  or without a bootloader (`_bl` postfix)

## Setup
This project depends on the following tools to be installed:
- doxygen
- valgrind

Also, you might have to download the cross compiler [here](https://releases.linaro.org/archive/15.02/components/toolchain/binaries/aarch64-none-elf/)
and change the first line of the Makefile to where you put it.

## Building
Simply run `make` for building the project and `make doxygen` for generating the
project documentation in html.

## TODO
\todo Add bootloader as git submodule and add flashing instructions accordingly.
\todo Add code coverage measurement for tests.
\todo Add style checker and static analyzer?
