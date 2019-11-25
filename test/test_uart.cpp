#include <catch.hpp>

#include "utils/uart.h"
#include "utils/gpio.h"

static void resetTestRegisters(void) {
    test_gpfsel[1] = 0;
    test_aux_mu_io  = 0;
    test_aux_mu_lsr = 0;
}

/*! \brief Sets the data ready bit of the status register to indicate that a new value
 * is ready. */
static void setDataReady(void) {
    test_aux_mu_lsr |= 0x00000001;
}

/*! \brief Clears the data ready bit of the status register to indicate that no
 * data is available. */
static void clearDataReady(void) {
    test_aux_mu_lsr &= ~0x00000001;
}

TEST_CASE("Test pin configuration and character fetching", "[Uart]" ) {
    unsigned char c;
    resetTestRegisters();

    Uart::init();
    REQUIRE(test_gpfsel[1] == 0x00012000);

    // Fetch character when none available
    REQUIRE(Uart::getChar(&c) == Uart::Error::UART_NO_DATA_AVAILABLE);

    // Set value ready and fetch the value
    setDataReady();
    test_aux_mu_io = 0xFFFFFFA5;
    REQUIRE(Uart::getChar(&c) == Uart::Error::UART_OK);
    REQUIRE(c == 0xA5);
    
    // Fetch character when none available
    clearDataReady();
    REQUIRE(Uart::getChar(&c) == Uart::Error::UART_NO_DATA_AVAILABLE);
    
    // Test the testBuffer with random values
    Uart::testBuffer.push(0x2E);
    Uart::testBuffer.push(0x47);
    REQUIRE(Uart::getChar(&c) == Uart::Error::UART_OK);
    REQUIRE(c == 0x2E);
    REQUIRE(Uart::getChar(&c) == Uart::Error::UART_OK);
    REQUIRE(c == 0x47);
    REQUIRE(Uart::getChar(&c) == Uart::Error::UART_NO_DATA_AVAILABLE);

    Uart::deinit();
    REQUIRE(test_gpfsel[1] == 0x00000000);
}
