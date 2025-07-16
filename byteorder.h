/**
  ******************************************************************************
  * @file        : byteorder.h
  * @author      : ZJY
  * @version     : V1.0
  * @date        : 2025-06-11
  * @brief       : Byte order conversion utilities for handling endianness
  * @attention   : This module provides functions to convert between CPU byte order
  *                and big-endian (network) byte order
  * @important   : This implementation is specifically designed for little-endian
  *                CPU architectures only. It will not work correctly on
  *                big-endian systems.
  ******************************************************************************
  * @history     :
  *         V1.0 : 1.Initial version - Basic byte order conversion functions
  ******************************************************************************
  */
#ifndef __BYTERORDER_H__
#define __BYTERORDER_H__

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "cmsis_compiler.h"

/* Exported define -----------------------------------------------------------*/

/* Exported typedef ----------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported variable prototypes ----------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/
/**
 * @brief  Convert a 16-bit value from CPU byte order (little-endian) to big-endian
 * @note   Uses ARM REV16 instruction for optimal performance
 * @param  val: Value to convert in CPU byte order (little-endian)
 * @return The converted value in big-endian byte order
 */
static inline uint16_t cpu_to_be16(uint16_t val)
{
    return (uint16_t)__REV16((uint32_t)val);
}

/**
 * @brief  Convert a 16-bit value from big-endian to CPU byte order (little-endian)
 * @note   Uses ARM REV16 instruction for optimal performance
 * @param  val: Value to convert in big-endian byte order
 * @return The converted value in CPU byte order (little-endian)
 */
static inline uint16_t be16_to_cpu(uint16_t val)
{
    return (uint16_t)__REV16((uint32_t)val);
}

/**
 * @brief  Convert a 32-bit value from CPU byte order (little-endian) to big-endian
 * @note   Uses ARM REV instruction for optimal performance
 * @param  val: Value to convert in CPU byte order (little-endian)
 * @return The converted value in big-endian byte order
 */
static inline uint32_t cpu_to_be32(uint32_t val)
{
    return __REV(val);
}

/**
 * @brief  Convert a 32-bit value from big-endian to CPU byte order (little-endian)
 * @note   Uses ARM REV instruction for optimal performance
 * @param  val: Value to convert in big-endian byte order
 * @return The converted value in CPU byte order (little-endian)
 */
static inline uint32_t be32_to_cpu(uint32_t val)
{
    return __REV(val);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BYTERORDER_H__ */

