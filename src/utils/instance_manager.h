#ifndef _INSTANCE_MANAGER_H_
#define _INSTANCE_MANAGER_H_

#include "common.h"

#define CEIL_DIV(x,y)   (((x) + (y) - 1) / (y))

using std::size_t;

/*! \brief Class that handles allocation of memory in the absence of dynamic
 * memory allocation possibilities. It handles storing/freeing of instances in
 * preallocated arrays. */
template <class T, size_t NUM_ENTRIES>
class InstanceManager {
    typedef int_fast16_t im_size_t;
    typedef im_size_t index_t;

    /*! \brief Size of inst[] + valid must be a multiple of 8.
     *  \todo Check if 4 is not sufficient. */
    static const int SIZE_WITH_PADDING = CEIL_DIV(sizeof(T) + sizeof(bool), 8) * 8 - 1;

    /*! \brief Struct for entries in the instance manager.
     *  \note Starting with the instance because it must be aligned in memory! */
    typedef struct {
        char inst[SIZE_WITH_PADDING];
        bool valid;
    } InstanceEntry;

    public:
        /*! \brief InstanceManager constructor. */
        InstanceManager() {
            size              = sizeof(entries)/sizeof(InstanceEntry);
            lastFilledIdx     = -1;
            curInstanceIndex  = -1;
            #ifdef ENABLE_MEMORY_PROFILING
                numEntriesFilled = 0;
            #endif
            for (index_t i = 0; i < (index_t) NUM_ENTRIES; i++) {
                entries[i].valid = false;
            }
        }

        /*! \brief Stores the instance in memory and returns a pointer to it.
         *  \param[in] inst The instance that must be stored.
         *  \returns A pointer to where the instance is stored or a nullptr if
         *           there was no memory left. */
        T* alloc(T inst) {
            for (index_t idx = 0; idx < size; idx++) {
                if (!entries[idx].valid) {
                    memcpy(entries[idx].inst, &inst, sizeof(T));
                    entries[idx].valid = true;
                    if (idx > lastFilledIdx) {
                        lastFilledIdx = idx;
                    }
                    #ifdef ENABLE_MEMORY_PROFILING
                        numEntriesFilled++;
                    #endif
                    return (T*) (&entries[idx].inst[0]);
                }
            }
            return nullptr;
        }

        /*! \brief Frees up the memory that is currently occupied by the
         *         instance to which the pointer points.
         *  \param[in] pInst The memory entry that must be freed. */
        void free(T* pInst) {
            index_t lastValidIdx = -1;
            for (index_t idx = 0; idx <= lastFilledIdx; idx++) {
                if ((T*) entries[idx].inst == pInst) {
                    if (entries[idx].valid) {
                        entries[idx].valid = false;
                        if (idx == lastFilledIdx) {
                            lastFilledIdx = lastValidIdx;
                        }
                        #ifdef ENABLE_MEMORY_PROFILING
                            numEntriesFilled--;
                        #endif
                    }
                    return;
                } else if (entries[idx].valid) {
                    lastValidIdx = idx;
                }
            }
        }

        /*! \brief This method returns the next instance amongst the entries and
         *         increases the internal index, i.e. the next time this
         *         function is called the next valid instance amongst the
         *         entries is returned (if any). Reset to the start using
         *         resetIndex().
         * \returns A pointer to the next instance. A nullptr if the last valid
         *          was reached. */
        T* getNextInstance() {
            for (index_t idx = curInstanceIndex + 1; idx <= lastFilledIdx; idx++) {
                if (entries[idx].valid) {
                    curInstanceIndex = idx;
                    return (T*) entries[idx].inst;
                }
            }
            return nullptr;
        }

        /*! \brief Sets the index for fetching instances one by one back to the
         *         first entry. */
        void resetIndex() {
            curInstanceIndex = -1;
        }

    private:
        void* memcpy(void* dest, const void* src, size_t num) {
            for (size_t i = 0; i < num; i++) {
                ((char*) dest)[i] = ((char*) src)[i];
            }
            return dest;
        }

    private:
        im_size_t size;
        index_t  lastFilledIdx;
        InstanceEntry entries[NUM_ENTRIES];
        index_t curInstanceIndex;

    public:
        #ifdef ENABLE_MEMORY_PROFILING
            index_t numEntriesFilled;
        #endif
};

#endif /* _INSTANCE_MANAGER_H_ */
