#include <catch.hpp>

#include "utils/gpio.h"

static void resetFunctionSelectRegisters(void) {
    for (int i = 0; i < NUM_GPFSEL_REGISTERS; i++) {
        test_gpfsel[i] = 0;
    }
}

TEST_CASE("Pin function configuration", "[Gpio]" ) {
    resetFunctionSelectRegisters();
    // Test two values in GPFSEL0
    Gpio::config(0, Gpio::GPFSEL_ALT5);
    Gpio::config(9, Gpio::GPFSEL_ALT3);
    // Test last pin
    Gpio::config(53, Gpio::GPFSEL_ALT1);
    // Test pin that does not exist
    Gpio::config(54, Gpio::GPFSEL_ALT3);

    // Also test the other pin functions in non-checked registers
    Gpio::config(15, Gpio::GPFSEL_INPUT);
    Gpio::config(16, Gpio::GPFSEL_OUTPUT);
    Gpio::config(21, Gpio::GPFSEL_ALT0);
    Gpio::config(37, Gpio::GPFSEL_ALT2);
    Gpio::config(40, Gpio::GPFSEL_ALT4);

    // Check the register values
    REQUIRE(test_gpfsel[0] == 0x38000002);
    REQUIRE(test_gpfsel[1] == 0x00040000);
    REQUIRE(test_gpfsel[2] == 0x00000020);
    REQUIRE(test_gpfsel[3] == 0x00C00000);
    REQUIRE(test_gpfsel[4] == 0x00000003);
    REQUIRE(test_gpfsel[5] == 0x00000A00);

    // Overwrite pin function with inverted function bits
    Gpio::config(53, Gpio::GPFSEL_ALT5);
    REQUIRE(test_gpfsel[5] == 0x00000400);
}
