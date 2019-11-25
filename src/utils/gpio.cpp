#include "gpio.h"

#ifndef __HOST__
    #define GPFSEL0         ((volatile uint32_t*) (MMIO_BASE + 0x00200000))
    #define GPFSEL1         ((volatile uint32_t*) (MMIO_BASE + 0x00200004))
    #define GPFSEL2         ((volatile uint32_t*) (MMIO_BASE + 0x00200008))
    #define GPFSEL3         ((volatile uint32_t*) (MMIO_BASE + 0x0020000C))
    #define GPFSEL4         ((volatile uint32_t*) (MMIO_BASE + 0x00200010))
    #define GPFSEL5         ((volatile uint32_t*) (MMIO_BASE + 0x00200014))
    #define GPSET0          ((volatile uint32_t*) (MMIO_BASE + 0x0020001C))
    #define GPSET1          ((volatile uint32_t*) (MMIO_BASE + 0x00200020))
    #define GPCLR0          ((volatile uint32_t*) (MMIO_BASE + 0x00200028))
    #define GPCLR1          ((volatile uint32_t*) (MMIO_BASE + 0x0020002C))
    #define GPLEV0          ((volatile uint32_t*) (MMIO_BASE + 0x00200034))
    #define GPLEV1          ((volatile uint32_t*) (MMIO_BASE + 0x00200038))
    #define GPEDS0          ((volatile uint32_t*) (MMIO_BASE + 0x00200040))
    #define GPEDS1          ((volatile uint32_t*) (MMIO_BASE + 0x00200044))
    #define GPREN0          ((volatile uint32_t*) (MMIO_BASE + 0x0020004C))
    #define GPREN1          ((volatile uint32_t*) (MMIO_BASE + 0x00200050))
    #define GPFEN0          ((volatile uint32_t*) (MMIO_BASE + 0x00200058))
    #define GPFEN1          ((volatile uint32_t*) (MMIO_BASE + 0x0020005C))
    #define GPHEN0          ((volatile uint32_t*) (MMIO_BASE + 0x00200064))
    #define GPHEN1          ((volatile uint32_t*) (MMIO_BASE + 0x00200068))
    #define GPLEN0          ((volatile uint32_t*) (MMIO_BASE + 0x00200070))
    #define GPLEN1          ((volatile uint32_t*) (MMIO_BASE + 0x00200074))
    #define GPAREN0         ((volatile uint32_t*) (MMIO_BASE + 0x0020007C))
    #define GPAREN1         ((volatile uint32_t*) (MMIO_BASE + 0x00200080))
    #define GPAFEN0         ((volatile uint32_t*) (MMIO_BASE + 0x00200088))
    #define GPAFEN1         ((volatile uint32_t*) (MMIO_BASE + 0x0020008C))
    #define GPPUD           ((volatile uint32_t*) (MMIO_BASE + 0x00200094))
    #define GPPUDCLK0       ((volatile uint32_t*) (MMIO_BASE + 0x00200098))
    #define GPPUDCLK1       ((volatile uint32_t*) (MMIO_BASE + 0x0020009C))
#else
    volatile uint32_t test_gpfsel[NUM_GPFSEL_REGISTERS] = {0};
    #define GPFSEL0         (&test_gpfsel[0])
#endif

#define GPIO_NUM_PINS       (54)

/*! \brief Configures the GPIO pin as the specified function.
 *  \param[in] p The GPIO pin number.
 *  \param[in] f The (alternate function). */
void Gpio::config(Pin p, PinFunction f) {
    // Configure pin as an output
    if (p < GPIO_NUM_PINS) {
        // Determine the associated GPIO Selection register and bits
        int register_number = p / 10;
        int bit_index = (p - 10 * register_number) * 3;
        volatile uint32_t* reg_ptr = GPFSEL0 + register_number;

        // Clear the old function bits
        *reg_ptr &= ~(0x07 << bit_index);
        // Set the actual function bits
        *reg_ptr |= f << bit_index;
    }
}
