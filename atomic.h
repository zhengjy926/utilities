/**
  ******************************************************************************
  * @file        : xxx.h
  * @author      : ZJY
  * @version     : V1.0
  * @date        : 20xx-xx-xx
  * @brief       : 
  * @attention   : None
  ******************************************************************************
  * @history     :
  *         V1.0 : 1.xxx
  ******************************************************************************
  */
#ifndef __ATOMIC_H__
#define __ATOMIC_H__

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "types.h"
#include "sys_conf.h"

/* Exported define -----------------------------------------------------------*/

/* Exported typedef ----------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported variable prototypes ----------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/
atomic_t hw_atomic_load(volatile atomic_t *ptr);
void     hw_atomic_store(volatile atomic_t *ptr, atomic_t val);
atomic_t hw_atomic_add(volatile atomic_t *ptr, atomic_t val);
atomic_t hw_atomic_sub(volatile atomic_t *ptr, atomic_t val);
atomic_t hw_atomic_and(volatile atomic_t *ptr, atomic_t val);
atomic_t hw_atomic_or(volatile atomic_t *ptr, atomic_t val);
atomic_t hw_atomic_xor(volatile atomic_t *ptr, atomic_t val);
atomic_t hw_atomic_exchange(volatile atomic_t *ptr, atomic_t val);
void     hw_atomic_flag_clear(volatile atomic_t *ptr);
atomic_t hw_atomic_flag_test_and_set(volatile atomic_t *ptr);
atomic_t hw_atomic_compare_exchange_strong(volatile atomic_t *ptr, atomic_t *expected, atomic_t desired);

#if defined(USING_HW_ATOMIC)
#define atomic_load(ptr)                            hw_atomic_load(ptr)
#define atomic_store(ptr, v)                        hw_atomic_store(ptr, v)
#define atomic_add(ptr, v)                          hw_atomic_add(ptr, v)
#define atomic_sub(ptr, v)                          hw_atomic_sub(ptr, v)
#define atomic_and(ptr, v)                          hw_atomic_and(ptr, v)
#define atomic_or(ptr, v)                           hw_atomic_or(ptr, v)
#define atomic_xor(ptr, v)                          hw_atomic_xor(ptr, v)
#define atomic_exchange(ptr, v)                     hw_atomic_exchange(ptr, v)
#define atomic_flag_clear(ptr)                      hw_atomic_flag_clear(ptr)
#define atomic_flag_test_and_set(ptr)               hw_atomic_flag_test_and_set(ptr)
#define atomic_compare_exchange_strong(ptr, v,des)  hw_atomic_compare_exchange_strong(ptr, v ,des)

#elif defined(USING_STDC_ATOMIC)

#ifndef __STDC_NO_ATOMICS__
#define atomic_load(ptr) atomic_load(ptr)
#define atomic_store(ptr, v) atomic_store(ptr, v)
#define atomic_add(ptr, v) atomic_fetch_add(ptr, v)
#define atomic_sub(ptr, v) atomic_fetch_sub(ptr, v)
#define atomic_and(ptr, v) atomic_fetch_and(ptr, v)
#define atomic_or(ptr, v)  atomic_fetch_or(ptr, v)
#define atomic_xor(ptr, v) atomic_fetch_xor(ptr, v)
#define atomic_exchange(ptr, v) atomic_exchange(ptr, v)
#define atomic_flag_clear(ptr) atomic_flag_clear(ptr)
#define atomic_flag_test_and_set(ptr) atomic_flag_test_and_set(ptr)
#define atomic_compare_exchange_strong(ptr, v,des) atomic_compare_exchange_strong(ptr, v ,des)
#else
#error "The standard library C doesn't support the atomic operation"
#endif /* __STDC_NO_ATOMICS__ */
#endif /* RT_USING_STDC_ATOMIC */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ATOMIC_H__ */

