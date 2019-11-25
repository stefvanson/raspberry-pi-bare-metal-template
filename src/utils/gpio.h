#ifndef _GPIO_H_
#define _GPIO_H_

#include "common.h"

#ifdef __HOST__
    #define NUM_GPFSEL_REGISTERS        (6)
    extern volatile uint32_t test_gpfsel[];
#endif

/*! \brief Class for configuring the Pi's GPIO pins. */
class Gpio {
    public:
        /*! \brief Alternative functions of pins (matching with GPFSELx register fields). */
        typedef enum {
            GPFSEL_INPUT  = 0b000,  //!< Input pin
            GPFSEL_OUTPUT = 0b001,  //!< Output pin
            GPFSEL_ALT0   = 0b100,  //!< Alternative function 0
            GPFSEL_ALT1   = 0b101,  //!< Alternative function 1
            GPFSEL_ALT2   = 0b110,  //!< Alternative function 2
            GPFSEL_ALT3   = 0b111,  //!< Alternative function 3
            GPFSEL_ALT4   = 0b011,  //!< Alternative function 4
            GPFSEL_ALT5   = 0b010,  //!< Alternative function 5
        } PinFunction;
        /*! \brief Type for pin IDs. */
        typedef uint_fast16_t Pin;

    public:
        static void config(Pin p, PinFunction f);
};

#endif  /* _GPIO_H_ */
