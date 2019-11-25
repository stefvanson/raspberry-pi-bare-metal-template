/*! \brief Error handler for when pure virtual methods are called. Just block
 *   infinitely.
 *  \todo Print an error message? */
extern "C" void __cxa_pure_virtual() { while (1); }

