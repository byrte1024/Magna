#pragma once

// util.h

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


// Check if a number is a power of 2 (non-zero)
#define IS_POWER_OF_2(x) (((x) != 0) && (((x) & ((x) - 1)) == 0))

// Round up to next power of 2 (returns 0 for 0 input)
static inline uint64_t next_power_of_2(uint64_t x) {
    if (x == 0) return 0;
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    return x + 1;
}

// Round down to previous power of 2 (returns 0 for 0 input)
static inline uint64_t prev_power_of_2(uint64_t x) {
    if (x == 0) return 0;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    return x - (x >> 1);
}

// Round to the nearest power of 2
static inline uint64_t nearest_power_of_2(uint64_t x) {
    if (x == 0) return 0;
    uint64_t next = next_power_of_2(x);
    uint64_t prev = prev_power_of_2(x);
    return (next - x) < (x - prev) ? next : prev;
}

// Check if a number is a power of 2 (non-zero)
static inline bool is_power_of_2(uint64_t x) {
	return (x != 0) && ((x & (x - 1)) == 0);
}
