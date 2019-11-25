PATH := /opt/gcc-linaro-4.9-2015.02-3-x86_64_aarch64-elf/bin:$(PATH)

C_FILES := $(shell find src -name '*.c')
CPP_FILES := $(shell find src -name '*.cpp')
SOURCES := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o)

all: kernel8.img regression

#####################################################
#				Building for target
#####################################################

ARMGNU ?= aarch64-elf
BUILDDIR = ./build

ASS_FLAGS = --warn --fatal-warnings -mcpu=cortex-a53 -march=armv8-a+crc
CXX_FLAGS = -Wall -O3 -mcpu=cortex-a53 -march=armv8-a+crc -mtune=cortex-a53 -nostdlib -nostartfiles -fno-exceptions -fno-rtti -ffreestanding -std=c++11 -Isrc
OBJS := $(BUILDDIR)/boot.o $(foreach src, $(SOURCES), $(BUILDDIR)/$(src))


$(BUILDDIR)/boot.o: src/utils/boot.s
	@echo "[ASM] $<"
	@mkdir -p "$(dir $@)"
	@$(ARMGNU)-as $(ASS_FLAGS) src/utils/boot.s -o $(BUILDDIR)/boot.o

$(BUILDDIR)/%.o: %.cpp
	@echo "[CXX] $<"
	@mkdir -p "$(dir $@)"
	@$(ARMGNU)-g++ $(CXX_FLAGS) -Wa,-adhln -g -c $< -o $@ > $(basename $@).asm

kernel8.img: $(OBJS)
	@echo "[LD]   program_bl.elf"
	@$(ARMGNU)-ld $(OBJS) -T src/utils/memmap_bl -o $(BUILDDIR)/program_bl.elf
	@echo "[SREC] program_bl.srec"
	@$(ARMGNU)-objcopy --srec-forceS3 $(BUILDDIR)/program_bl.elf -O srec $(BUILDDIR)/program_bl.srec
	@echo "[LD]   program.elf"
	@$(ARMGNU)-ld $(OBJS) -T src/utils/memmap -o $(BUILDDIR)/program.elf
	@echo "[BIN]  kernel8.img"
	@$(ARMGNU)-objcopy $(BUILDDIR)/program.elf -O binary $(BUILDDIR)/kernel8.img
	@$(ARMGNU)-objdump -D $(BUILDDIR)/program.elf > $(BUILDDIR)/program.list
	@$(ARMGNU)-nm -n $(BUILDDIR)/program.elf > $(BUILDDIR)/program.map


#####################################################
#				  Host testing
#####################################################

CXX_HOST = g++
TESTBUILDDIR = ./testbuild

CXX_FLAGS_TEST = -std=c++11 -Wall -D__HOST__ -Isrc -Itest -fprofile-arcs -ftest-coverage
TEST_FILES := $(shell find test -name '*.cpp')
TEST_OBJS := $(foreach src, $(CPP_FILES:.cpp=.o), $(TESTBUILDDIR)/$(src)) $(foreach test, $(TEST_FILES:.cpp=.o), $(TESTBUILDDIR)/$(test))
TEST_OBJS := $(filter-out %/main.o, $(TEST_OBJS))


$(TESTBUILDDIR)/%.o: %.cpp
	@echo "[CXX] $<"
	@mkdir -p "$(dir $@)"
	@$(CXX_HOST) $(CXX_FLAGS_TEST) $(L_INC) -o $@ -c "$<"

$(TESTBUILDDIR)/regression: $(TEST_OBJS)
	@echo "[LD]  $@"
	@mkdir -p "$(dir $@)"
	@$(CXX_HOST) $(CXX_FLAGS_TEST) $(TEST_OBJS) test/main.cpp -o $(TESTBUILDDIR)/regression

regression: $(TEST_OBJS) $(TESTBUILDDIR)/regression
	@echo "Running regression"
	@valgrind -q --leak-check=full --error-exitcode=1 $(TESTBUILDDIR)/regression

coverage: regression
	@gcovr -e test/catch.hpp
	@gcovr -b -e test/catch.hpp


#####################################################
#				Generate documentation
#####################################################

DOXYGENDIR = ./doxy

# Generate documentation using doxygen
doxygen: kernel8.img
	@doxygen Doxyfile


#####################################################
# 					Cleaning up
#####################################################

clean: clean_build clean_testbuild clean_doxygen
	rm -rf $(BUILDDIR)/src
	rm main.gcda main.gcno

clean_build:
	rm -rf $(BUILDDIR)

clean_testbuild:
	rm -rf $(TESTBUILDDIR)

clean_doxygen:
	rm -rf $(DOXYGENDIR)
