#include "uart.h"

#include "gpio.h"

#ifndef __HOST__
    #define AUXENB                          ((volatile uint32_t*) (MMIO_BASE + 0x00215004))
    #define AUX_MU_IO	                    ((volatile uint32_t*) (MMIO_BASE + 0x00215040))
    #define AUX_MU_IER	                    ((volatile uint32_t*) (MMIO_BASE + 0x00215044))
    #define AUX_MU_IIR	                    ((volatile uint32_t*) (MMIO_BASE + 0x00215048))
    #define AUX_MU_LCR	                    ((volatile uint32_t*) (MMIO_BASE + 0x0021504C))
    #define AUX_MU_MCR	                    ((volatile uint32_t*) (MMIO_BASE + 0x00215050))
    #define AUX_MU_LSR	                    ((volatile uint32_t*) (MMIO_BASE + 0x00215054))
    #define AUX_MU_MSR	                    ((volatile uint32_t*) (MMIO_BASE + 0x00215058))
    #define AUX_MU_SCRATCH                  ((volatile uint32_t*) (MMIO_BASE + 0x0021505C))
    #define AUX_MU_CNTL	                    ((volatile uint32_t*) (MMIO_BASE + 0x00215060))
    #define AUX_MU_STAT	                    ((volatile uint32_t*) (MMIO_BASE + 0x00215064))
    #define AUX_MU_BAUD	                    ((volatile uint32_t*) (MMIO_BASE + 0x00215068))
#else // For testing purposes
    RingBuffer<volatile uint32_t, 32> Uart::testBuffer;

    volatile uint32_t test_aux_mu_io = 0;
    #define AUX_MU_IO                       (&test_aux_mu_io)
    volatile uint32_t test_aux_mu_lsr = 0;
    #define AUX_MU_LSR                      (&test_aux_mu_lsr)
#endif

#define AUXENB_MINI_UART_ENABLE         (1 << 0)
#define AUX_MU_IIR_CLEAR_RECEIVE_FIFO   (1 << 1)
#define AUX_MU_IIR_CLEAR_TRANSMIT_FIFO  (1 << 2)
#define AUX_MU_LCR_8BITS_UART           (3 << 0)
#define AUX_MU_LCR_BREAK                (1 << 6)
#define AUX_MU_LCR_DLAB                 (1 << 7)
#define AUX_MU_LSR_DATA_READY           (1 << 0)
#define AUX_MU_CNTL_RECEIVER_ENABLE     (1 << 0)
#define AUX_MU_CNTL_TRANSMITTER_ENABLE  (1 << 1)


/*! \brief Initializes the UART peripheral and used pins. */
void Uart::init() {
    // Configure the pins for UART functionality
    Gpio::config(14, Gpio::GPFSEL_ALT5);
    Gpio::config(15, Gpio::GPFSEL_ALT5);

    #ifndef __HOST__
        // Configure the registers
        *AUXENB |= AUXENB_MINI_UART_ENABLE;  // Enable the Mini UART
        *AUX_MU_IER = 0;                     // No interrupts
        *AUX_MU_LCR = AUX_MU_LCR_8BITS_UART; // 8-bit mode, break bit cleared
        *AUX_MU_CNTL = 0;                    // Disable receiver and transmitter
        *AUX_MU_BAUD = UART_BAUD_RATE;       // Set the baud rate

        // Clear the FIFOs
        *AUX_MU_IIR |= (AUX_MU_IIR_CLEAR_TRANSMIT_FIFO | AUX_MU_IIR_CLEAR_RECEIVE_FIFO); // Clear the FIFOs
    #endif /* __HOST__ */
}

/*! \brief Deinitializes the UART peripheral (and associated pins). */
void Uart::deinit() {
    // Reset to their default state
    Gpio::config(14, Gpio::GPFSEL_INPUT);
    Gpio::config(15, Gpio::GPFSEL_INPUT);

    #ifndef __HOST__
        *AUX_MU_CNTL = 0; // Disable receiver and transmitter
        *AUX_MU_IIR |= (AUX_MU_IIR_CLEAR_TRANSMIT_FIFO | AUX_MU_IIR_CLEAR_RECEIVE_FIFO); // Clear the FIFOs
        *AUXENB &= ~AUXENB_MINI_UART_ENABLE; // Disable the mini UART
    #endif /* __HOST__ */
}

/*! \brief Starts receiving of data over UART.
 *  \note Needs to be initialized first! */
void Uart::start() {
    #ifndef __HOST__
        // Enable the transmitter and receiver
        *AUX_MU_CNTL = AUX_MU_CNTL_RECEIVER_ENABLE | AUX_MU_CNTL_TRANSMITTER_ENABLE;
    #endif /* __HOST__ */
}

/*! \brief Fetches the next received character from the receive FIFO (if any).
 *  \param[in] receivedByte Reference to where the received byte (if any) is
 *  put.
 *  \returns Error code indicating the success of receiving a character. */
Uart::Error Uart::getChar(unsigned char* receivedByte) {
    #ifdef __HOST__
        // If not already manually set, check the testBuffer for input for testing on a host PC
        if (!(test_aux_mu_lsr & AUX_MU_LSR_DATA_READY) &&
            testBuffer.pop(&test_aux_mu_io) <= RingBuffer<volatile uint32_t, 32>::Error::OK_LAST) {
            test_aux_mu_lsr |= AUX_MU_LSR_DATA_READY;
        }
    #endif
    if(*AUX_MU_LSR & AUX_MU_LSR_DATA_READY) {
        *receivedByte = *AUX_MU_IO & 0xFF;
        #ifdef __HOST__
            // Clear the data ready bit after receiving
            test_aux_mu_lsr &= ~AUX_MU_LSR_DATA_READY;
        #endif
        return UART_OK;
    } else {
        return UART_NO_DATA_AVAILABLE;
    }
}

/*! \brief Sends a single byte over UART.
 *  \param[in] c The value to be sent. */
void Uart::send(char c) {
    #ifndef __HOST__
    while(1) {
        // Wait for the transmit FIFO to accept bytes
        if(*AUX_MU_LSR & 0x20) {
            break;
        }
    }
    // Write the byte into the FIFO
    *AUX_MU_IO = c;
    #endif
}

/*! \brief Sends a string. */
void Uart::sendString(const char* s) {
    int i = 0;
    while (s[i] != '\0') {
        send(s[i++]);
    }
}

/*! \brief Prints the hex value of a nibble (half a byte) in ASCII.
 *  \param[in] n The nibble, which should be in the 4 least significant bits. */
static void printNibble(uint8_t n) {
    n &= 0x0F;
    if (n >= 0xA) {
        Uart::send((n-0xA) + 'A');
    } else {
        Uart::send(n + '0');
    }
}

/*! \brief Prints the hexadecimal value, prefixed by 0x and followed by a space.
 *  \param[in] v The value that must be outputted. */
void Uart::sendHex(uint64_t v) {
    sendString("0x");
    int i = sizeof(uint64_t) - 1;
    bool printing = false;
    while (i >= 0) {
        uint8_t byte = (v >> (i*8)) & 0xFF;
        if (printing || byte != 0x00 || i == 0) {
            printing = true;
            printNibble((byte >> 4) & 0x0F);
            printNibble(byte & 0x0F);
        }
        i--;
    }
    send(' ');
}
