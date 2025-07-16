#ifndef __ARM_BITOPS_H__
#define __ARM_BITOPS_H__

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>
#include "cmsis_compiler.h"

#define BIT(n)			(0x1UL << (n))

/***************************** 原子性的操作 **********************************/
/**
 * @brief 8位原子操作
 */
static inline void set_bit8_atomic(uint8_t mask, uint8_t *ptr)
{
    uint8_t oldval, newval;
    do {
        oldval = __LDREXB(ptr);
        newval = oldval | mask;
    } while (__STREXB(newval, ptr) != 0);
}

static inline void clear_bit8_atomic(uint8_t mask, uint8_t *ptr)
{
    uint8_t oldval, newval;
    do {
        oldval = __LDREXB(ptr);
        newval = oldval & (uint8_t)~mask;
    } while (__STREXB(newval, ptr) != 0);
}

static inline void change_bit8_atomic(uint8_t mask, uint8_t *ptr)
{
    uint8_t oldval, newval;
    do {
        oldval = __LDREXB(ptr);
        newval = oldval ^ mask;
    } while (__STREXB(newval, ptr) != 0);
}

static inline bool test_bit8_atomic(const uint8_t mask, uint8_t *ptr)
{
    volatile uint8_t *vptr = (volatile uint8_t *)ptr;
    uint8_t val = __LDREXB(vptr);
    __CLREX();
    return ((val & mask) != 0U);
}

static inline bool test_and_set_bit8_atomic(uint8_t mask, uint8_t *ptr)
{
    uint8_t oldval, newval;
    bool wasSet;
    do {
        oldval = __LDREXB(ptr);
        wasSet = ((oldval & mask) != 0U);
        newval = oldval | mask;
    } while (__STREXB(newval, ptr) != 0);

    return wasSet;
}

static inline bool test_and_clear_bit8_atomic(uint8_t mask, uint8_t *ptr)
{
    uint8_t oldval, newval;
    bool wasSet;
    do {
        oldval = __LDREXB(ptr);
        wasSet = ((oldval & mask) != 0U);
        newval = oldval & (uint8_t)~mask;
    } while (__STREXB(newval, ptr) != 0);

    return wasSet;
}

static inline bool test_and_change_bit8_atomic(uint8_t mask, uint8_t *ptr)
{
    uint8_t oldval, newval;
    bool wasSet;
    do {
        oldval = __LDREXB(ptr);
        wasSet = ((oldval & mask) != 0U);
        newval = oldval ^ mask;
    } while (__STREXB(newval, ptr) != 0);

    return wasSet;
}

/**
 * @brief 16位原子操作
 */
static inline void set_bit16_atomic(uint16_t mask, uint16_t *ptr)
{
    uint16_t oldval, newval;
    do {
        oldval = __LDREXH(ptr);
        newval = oldval | mask;
    } while (__STREXH(newval, ptr) != 0);
}

static inline void clear_bit16_atomic(uint16_t mask, uint16_t *ptr)
{
    uint16_t oldval, newval;
    do {
        oldval = __LDREXH(ptr);
        newval = oldval & (uint16_t)~mask;
    } while (__STREXH(newval, ptr) != 0);
}

static inline void change_bit16_atomic(uint16_t mask, uint16_t *ptr)
{
    uint16_t oldval, newval;
    do {
        oldval = __LDREXH(ptr);
        newval = oldval ^ mask;
    } while (__STREXH(newval, ptr) != 0);
}

static inline bool test_bit16_atomic(const uint16_t mask, uint16_t *ptr)
{
    volatile uint16_t *vptr = (volatile uint16_t *)ptr;
    uint16_t val = __LDREXH(vptr);
    __CLREX();
    return ((val & mask) != 0U);
}

static inline bool test_and_set_bit16_atomic(uint16_t mask, uint16_t *ptr)
{
    uint16_t oldval, newval;
    bool wasSet;
    do {
        oldval = __LDREXH(ptr);
        wasSet = ((oldval & mask) != 0U);
        newval = oldval | mask;
    } while (__STREXH(newval, ptr) != 0);

    return wasSet;
}

static inline bool test_and_clear_bit16_atomic(uint16_t mask, uint16_t *ptr)
{
    uint16_t oldval, newval;
    bool wasSet;
    do {
        oldval = __LDREXH(ptr);
        wasSet = ((oldval & mask) != 0U);
        newval = oldval & (uint16_t)~mask;
    } while (__STREXH(newval, ptr) != 0);

    return wasSet;
}

static inline bool test_and_change_bit16_atomic(uint16_t mask, uint16_t *ptr)
{
    uint16_t oldval, newval;
    bool wasSet;
    do {
        oldval = __LDREXH(ptr);
        wasSet = ((oldval & mask) != 0U);
        newval = oldval ^ mask;
    } while (__STREXH(newval, ptr) != 0);

    return wasSet;
}

/**
 * @brief 32位原子操作
 */
static inline void set_bit32_atomic(uint32_t mask, uint32_t *ptr)
{
    uint32_t oldval, newval;
    do {
        oldval = __LDREXW(ptr);
        newval = oldval | mask;
    } while (__STREXW(newval, ptr) != 0);
}

static inline void clear_bit32_atomic(uint32_t mask, uint32_t *ptr)
{
    uint32_t oldval, newval;
    do {
        oldval = __LDREXW(ptr);
        newval = oldval & (uint32_t)~mask;
    } while (__STREXW(newval, ptr) != 0);
}

static inline void change_bit32_atomic(uint32_t mask, uint32_t *ptr)
{
    uint32_t oldval, newval;
    do {
        oldval = __LDREXW(ptr);
        newval = oldval ^ mask;
    } while (__STREXW(newval, ptr) != 0);
}

static inline bool test_bit32_atomic(const uint32_t mask, uint32_t *ptr)
{
    volatile uint32_t *vptr = (volatile uint32_t *)ptr;
    uint32_t val = __LDREXW(vptr);
    __CLREX();
    return ((val & mask) != 0U);
}

static inline bool test_and_set_bit32_atomic(uint32_t mask, uint32_t *ptr)
{
    uint32_t oldval, newval;
    bool wasSet;
    do {
        oldval = __LDREXW(ptr);
        wasSet = ((oldval & mask) != 0U);
        newval = oldval | mask;
    } while (__STREXW(newval, ptr) != 0);

    return wasSet;
}

static inline bool test_and_clear_bit32_atomic(uint32_t mask, uint32_t *ptr)
{
    uint32_t oldval, newval;
    bool wasSet;
    do {
        oldval = __LDREXW(ptr);
        wasSet = ((oldval & mask) != 0U);
        newval = oldval & (uint32_t)~mask;
    } while (__STREXW(newval, ptr) != 0);

    return wasSet;
}

static inline bool test_and_change_bit32_atomic(uint32_t mask, uint32_t *ptr)
{
    uint32_t oldval, newval;
    bool wasSet;
    do {
        oldval = __LDREXW(ptr);
        wasSet = ((oldval & mask) != 0U);
        newval = oldval ^ mask;
    } while (__STREXW(newval, ptr) != 0);

    return wasSet;
}


/***************************** 通用的 ************************************/
#define set_bit(mask, ptr)          \
    do {                            \
        *(ptr) |= (mask);           \
    } while(0)

#define clear_bit(mask, ptr)        \
    do {                            \
        *(ptr) &= ~(mask);          \
    } while(0)

#define change_bit(mask, ptr)       \
    do {                            \
        *(ptr) ^= (mask);           \
    } while(0)

#define test_bit(mask, ptr)         ((*(ptr) & (mask)) ? 1 : 0)

#define test_and_set_bit(mask, ptr) \
    ({                              \
        bool was_set = test_bit(mask, ptr); \
        set_bit(mask, ptr);         \
        was_set;                    \
    })

#define test_and_clear_bit(mask, ptr) \
    ({                                \
        bool was_set = test_bit(mask, ptr); \
        clear_bit(mask, ptr);         \
        was_set;                      \
    })

#define test_and_change_bit(mask, ptr) \
    ({                                 \
        bool was_set = test_bit(mask, ptr); \
        change_bit(mask, ptr);         \
        was_set;                       \
    })


#ifdef __cplusplus
 }
#endif /* __cplusplus */
#endif /* __ARM_BITOPS_H__ */
