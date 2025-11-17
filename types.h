/**
  ******************************************************************************
  * @file        : types.h
  * @author      : ZJY
  * @version     : V1.0
  * @date        : 2024-09-26
  * @brief       : 
  * @attention   : None
  ******************************************************************************
  * @history     :
  *         V1.0 : 1.xxx
  ******************************************************************************
  */
#ifndef __TYPES_H__
#define __TYPES_H__

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported define -----------------------------------------------------------*/

/* Exported typedef ----------------------------------------------------------*/
typedef  int32_t                base_t;  /**< Nbit CPU related date type */
typedef uint32_t                ubase_t; /**< Nbit unsigned CPU related data type */
typedef  int32_t                ssize_t; /**< The size t type that may return a negative value */

typedef ubase_t                 atomic_t;
typedef int32_t                 off_t;

/* Exported macro ------------------------------------------------------------*/
#define ARRAY_SIZE(arr)                                                         \
    (                                                                           \
        assert(!__builtin_types_compatible_p(typeof(arr), typeof(&(arr)[0]))),  \
        (sizeof(arr) / sizeof((arr)[0]))                                        \
    )
        
/**
 * @brief return the member address of ptr, if the type of ptr is the struct type.
 */
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
/* Exported variable prototypes ----------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TYPES_H__ */

