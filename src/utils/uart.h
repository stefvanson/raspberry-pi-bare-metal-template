#ifndef _UART_H_
#define _UART_H_

#include "common.h"

/* baud_rate = clockfreq (250MHz) / (8 * (AUX_MU_BAUD + 1)) */
#define AUX_MU_BAUD_MIDI                (999)  // MIDI is spec'ed at 31.25k (+-1%) baud
#define AUX_MU_BAUD_115k2_BAUD          (270)

#ifdef CONFIG_UART_BAUD_RATE
    #define UART_BAUD_RATE              (CONFIG_UART_BAUD_RATE)
#else
    #define UART_BAUD_RATE              (AUX_MU_BAUD_MIDI)
#endif

#ifdef __HOST__
    #include "ring_buffer.h"
    extern volatile uint32_t test_aux_mu_io;
    extern volatile uint32_t test_aux_mu_lsr;
#endif

/*! \brief Class for accessing the mini UART peripheral. */
class Uart {
    public:
        /*! \brief Return type indicating errors. */
        typedef enum {
            UART_OK = 0,            //!< Everything went fine
            UART_NO_DATA_AVAILABLE, //!< No data was available in the receive FIFO
        } Error;

        static void init();
        static void deinit();
        static void start();
        static Error getChar(unsigned char* receivedByte);
        static void send(char c);
        static void sendString(const char* s);
        static void sendHex(uint64_t v);
        #ifdef __HOST__
            static RingBuffer<volatile uint32_t, 32> testBuffer;
        #endif
};

#endif /* _UART_H_ */
