# Raspberry Pi bare metal project template

## Purpose
Starting point to get something going quickly for Raspberry Pi 3B+ projects. It
includes:

- A UART driver for printing debug messages.
- MMU configuration (required for enabling cache, so major performance boost).
- A Doxyfile for automatic documentation generation.
- Tests and the `catch.hpp` header file.
- A Makefile to run all of the above. It constructs a binary for deploying with
  or without a bootloader (`_bl` postfix).
- It includes a separate bootloader project as a git submodule. Run `git
  submodule update` to fetch the correct commit of it.

## Setup
This project depends on the following tools to be installed:
- doxygen
- valgrind
- gcovr

Also, you might have to download the cross compiler [here](https://releases.linaro.org/archive/15.02/components/toolchain/binaries/aarch64-none-elf/)
and change the first line of the Makefile to where you put it.

## Building
1. Run `make` for building the project.
2. Run `make doxygen` for generating the project documentation in html.
3. Run `make coverage` to print coverage reports of the tests.

## TODO
- \todo Add flashing/deploying instructions.
- \todo Add style checker and static analyzer?
