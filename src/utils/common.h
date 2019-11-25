#ifndef _COMMON_H_
#define _COMMON_H_

#include <cstdint>
#include <cstdbool>

#ifdef __HOST__
    #include <cstdlib>
    #include <stdio.h>
#endif

#include "config.h"
#include "rpi3_common.h"
#include "debug.h"

/*! \brief Error handler for when pure virtual methods are called. Just block
 *   infinitely.
 *  \todo Print an error message? */
extern "C" void __cxa_pure_virtual();

#endif /* _COMMON_H_ */
