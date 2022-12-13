#ifndef _MEM_USAGE_H
#define _MEM_USAGE_H
#include <stdint.h>

// Returns the total memory usage in bytes. Returns 0 in case of an error.
uint64_t getMemoryUsage();

#endif
