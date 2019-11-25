#include "debug.h"

/*! \brief Dumps the contents of a certain memory section over the
 *         serial connection.
 *  \param[in] pMem Pointer to the start of the memory section that must be
 *             dumped.
 *  \param[in] size The number of bytes that must be dumped. */
void Debug::memDump(uint8_t* pMem, unsigned int size) {
    for (unsigned i = 0; i < size; i++) {
        Uart::sendHex(pMem[i]);
    }
}
