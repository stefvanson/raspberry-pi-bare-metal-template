#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include "common.h"

/*! \brief Ring buffer a.k.a. circular buffer template class.
 *  \param[in] Instance The type that can be stored in the ring buffer.
 *  \param[in] NUM_ENTRIES The capacity (number of instances) of the RingBuffer. */
template <class Instance, int_fast16_t NUM_ENTRIES>
class RingBuffer {
    public:
        //! \brief Type for return values of RingBuffer operations.
        typedef enum {
            OK = 0,         //!< Operation successful.
            OK_LAST = 1,    /*!< Operation successful, but this was the last entry that could be
                                 read/written (buffer is full or empty after current operation) */
            NOK_OVERFLOW,   //!< Indicates that pushing to a full buffer was tried (but failed).
            NOK_EMPTY,      //!< Indicates that fetching from an empty buffer was tried (but failed).
        } Error;

    private:
        typedef int_fast16_t Index;

    public:
        //! \brief Default constructor.
        RingBuffer() : head(0), numEntriesFilled(0) {}

        /*! \brief Pushes a new instance into the buffer if possible.
         *  \param[in] i Instance that must be pushed.
         *  \returns Error code indicating success or reason of failure. */
        Error push(Instance i) {
            if (numEntriesFilled >= NUM_ENTRIES) {
                return NOK_OVERFLOW;
            } else {
                entries[head] = i;
                head = getNextIndex(head);
                numEntriesFilled++;
                if (numEntriesFilled < NUM_ENTRIES) {
                    return OK;
                } else {
                    return OK_LAST;
                }
            }
        }

        /*! \brief Pops the next instance in the buffer if possible.
         *  \param[in] i Pointer to where the popped instance is stored.
         *  \returns Error code indicating success or reason of failure. */
        Error pop(Instance* i) {
            if (numEntriesFilled == 0) {
                return NOK_EMPTY;
            } else {
                Index index = head - numEntriesFilled;
                index = index >= 0 ? index : index + NUM_ENTRIES;
                *i = entries[index];
                numEntriesFilled--;
                if (numEntriesFilled > 0) {
                    return OK;
                } else {
                    return OK_LAST;
                }
            }
        }

        /*! \brief Peeks at the instance in the next to-be-read entry.
         *  \param[in] i Pointer to where the peeked instance is stored. */
        Error peek(Instance* i) {
            if (numEntriesFilled == 0) {
                return NOK_EMPTY;
            } else {
                Index index = head - numEntriesFilled;
                index = index >= 0 ? index : index + NUM_ENTRIES;
                *i = entries[index];
                return OK;
            }
        }

        /*! \brief Gets the current amount of entries that are filled.
         *  \returns The amount of entries that are already filled. */
        unsigned getNumEntriesFilled() {
            return numEntriesFilled;
        }

        /*! \brief Checks if the buffer is currently full or not.
         *  \returns True if full, false otherwise. */
        bool isFull() {
            return (numEntriesFilled == NUM_ENTRIES);
        }

        /*! \brief Checks if the buffer is currently empty or not.
         *  \returns True if empty, false otherwise. */
        bool isEmpty() {
            return (numEntriesFilled == 0);
        }

    private:
        /*! \brief Gets the next index in the buffer (wraps around at the end).
         *  \param[in] index The current index.
         *  \returns The next index. */
        static inline Index getNextIndex(Index index) {
            if (index + 1 >= (Index) NUM_ENTRIES) {
                return 0;
            } else {
                return index + 1;
            }
        }

        Instance entries[NUM_ENTRIES];
        Index head;
        Index numEntriesFilled;
};

#endif /* _RING_BUFFER_H_ */
