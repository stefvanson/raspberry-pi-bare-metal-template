/*! \brief Main file.*/

#include "utils/uart.h"


// Functions that are defined in the boot.s assembly
extern "C" void mmu_configure(void);
extern "C" void mmu_enable(void);

/*! \brief Main function. */
int main() {
    mmu_configure();
    mmu_enable();
    Uart::init();
    Uart::start();

    Uart::sendString("Program started\n");
    while(1);

    return 0;
}
