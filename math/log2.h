#include <stdbool.h>
#include <cmsis_compiler.h>

#ifndef __LOG2_H__
#define __LOG2_H__

/**
 * is_power_of_2() - check if a value is a power of two
 * @n: the value to check
 *
 * Determine whether some value is a power of two, where zero is
 * *not* considered a power of two.
 * Return: true if @n is a power of 2, otherwise false.
 */
static inline __attribute__((const))
bool is_power_of_2(unsigned int n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}

/**
 * __roundup_pow_of_two() - round up to nearest power of two
 * @n: value to round up
 */
static inline __attribute__((const))
unsigned int roundup_pow_of_two(unsigned int n)
{
    if (n <= 1)
        return 1;

    return 1U << (32 - __CLZ(n - 1)); /* __clz 是前导零计数指令 */
}

/**
 * __rounddown_pow_of_two() - round down to nearest power of two
 * @n: value to round down
 */
static inline __attribute__((const))
unsigned int rounddown_pow_of_two(unsigned int n)
{
	return 1U << ((32 - __CLZ(n)) - 1);
}

/**
 * ilog2 - log base 2 of 32-bit or a 64-bit unsigned value
 * @n: parameter
 *
 * constant-capable log of base 2 calculation
 * - this can be used to initialise global variables from constant data, hence
 * the massive ternary operator construction
 *
 * selects the appropriately-sized optimised version depending on sizeof(n)
 */
static inline uint32_t ilog2(uint32_t n) {
    return 31 - __CLZ(n);
}
#endif //__LOG2_H__

