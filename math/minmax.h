#ifndef __MINMAX_H
#define __MINMAX_H

#define __cmp_op_min <
#define __cmp_op_max >

#define __cmp(op, x, y)	((x) __cmp_op_##op (y) ? (x) : (y))

/**
 * min - return minimum of two values of the same or compatible types
 * @x: first value
 * @y: second value
 */
#define min(x, y) __cmp(min, x, y)

/**
 * max - return maximum of two values of the same or compatible types
 * @x: first value
 * @y: second value
 */
#define max(x, y) __cmp(max, x, y)

/**
 * min3 - return minimum of three values of the same or compatible types
 * @x: first value
 * @y: second value
 * @z: third value
 */
#define min3(x, y, z) min(min(x, y), z)

/**
 * max3 - return maximum of three values of the same or compatible types
 * @x: first value
 * @y: second value
 * @z: third value
 */
 #define max3(x, y, z) max(max(x, y), z)

#endif // __MINMAX_H
