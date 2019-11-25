#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "uart.h"

/*! \brief Class with functions that can be helpful for debugging. */
class Debug {
    public:
        static void memDump(uint8_t* pMem, unsigned int size);
};

#endif /* _DEBUG_H_ */
