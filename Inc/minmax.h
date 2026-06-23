#ifndef MINMAX_H
#define MINMAX_H

#include <stdint.h>

/**
 * min - return minimum of two values of the same or compatible types
 * @x: first value
 * @y: second value
 */
static inline uint32_t MIN_U32(uint32_t x, uint32_t y)
{
    return (x < y) ? x : y;
}

static inline int32_t MIN_S32(int32_t x, int32_t y)
{
    return (x < y) ? x : y;
}

static inline uint16_t MIN_U16(uint16_t x, uint16_t y)
{
    return (x < y) ? x : y;
}

static inline int16_t MIN_S16(int16_t x, int16_t y)
{
    return (x < y) ? x : y;
}

static inline uint8_t MIN_U8(uint8_t x, uint8_t y)
{
    return (x < y) ? x : y;
}

static inline int8_t MIN_S8(int8_t x, int8_t y)
{
    return (x < y) ? x : y;
}

static inline float MIN_FLOAT(float x, float y)
{
    return (x < y) ? x : y;
}

static inline double MIN_DOUBLE(double x, double y)
{
    return (x < y) ? x : y;
}

/**
 * max - return maximum of two values of the same or compatible types
 * @x: first value
 * @y: second value
 */
static inline uint32_t MAX_U32(uint32_t x, uint32_t y)
{
    return (x > y) ? x : y;
}

static inline int32_t MAX_S32(int32_t x, int32_t y)
{
    return (x > y) ? x : y;
}

static inline uint16_t MAX_U16(uint16_t x, uint16_t y)
{
    return (x > y) ? x : y;
}

static inline int16_t MAX_S16(int16_t x, int16_t y)
{
    return (x > y) ? x : y;
}

static inline uint8_t MAX_U8(uint8_t x, uint8_t y)
{
    return (x > y) ? x : y;
}

static inline int8_t MAX_S8(int8_t x, int8_t y)
{
    return (x > y) ? x : y;
}

static inline float MAX_FLOAT(float x, float y)
{
    return (x > y) ? x : y;
}

static inline double MAX_DOUBLE(double x, double y)
{
    return (x > y) ? x : y;
}

#endif // MINMAX_H
