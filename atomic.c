/**
  ******************************************************************************
  * @file        : xxxx.c
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
/* Includes ------------------------------------------------------------------*/
#include "atomic.h"

/* 核心头文件 */
#if defined(__ARM_ARCH)
    #include "cmsis_compiler.h"
#else
    /* 其他架构 */
#endif
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Exported variables  -------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
atomic_t hw_atomic_load(volatile atomic_t *ptr)
{
    atomic_t oldval;
    do
    {
        oldval = __LDREXW(ptr);
    } while ((__STREXW(oldval, ptr)) != 0U);
    return oldval;
}

void hw_atomic_store(volatile atomic_t *ptr, atomic_t val)
{
    do
    {
        __LDREXW(ptr);
    } while ((__STREXW(val, ptr)) != 0U);
}

atomic_t hw_atomic_add(volatile atomic_t *ptr, atomic_t val)
{
    atomic_t oldval;
    do
    {
        oldval = __LDREXW(ptr);
    } while ((__STREXW(oldval + val, ptr)) != 0U);
    return oldval;
}

atomic_t hw_atomic_sub(volatile atomic_t *ptr, atomic_t val)
{
    atomic_t oldval;
    do
    {
        oldval = __LDREXW(ptr);
    } while ((__STREXW(oldval - val, ptr)) != 0U);
    return oldval;
}

atomic_t hw_atomic_and(volatile atomic_t *ptr, atomic_t val)
{
    atomic_t oldval;
    do
    {
        oldval = __LDREXW(ptr);
    } while ((__STREXW(oldval & val, ptr)) != 0U);
    return oldval;
}

atomic_t hw_atomic_or(volatile atomic_t *ptr, atomic_t val)
{
    atomic_t oldval;
    do
    {
        oldval = __LDREXW(ptr);
    } while ((__STREXW(oldval | val, ptr)) != 0U);
    return oldval;
}

atomic_t hw_atomic_xor(volatile atomic_t *ptr, atomic_t val)
{
    atomic_t oldval;
    do
    {
        oldval = __LDREXW(ptr);
    } while ((__STREXW(oldval ^ val, ptr)) != 0U);
    return oldval;
}

atomic_t hw_atomic_exchange(volatile atomic_t *ptr, atomic_t val)
{
    atomic_t oldval;
    do
    {
        oldval = __LDREXW(ptr);
    } while ((__STREXW(val, ptr)) != 0U);
    return oldval;
}

void hw_atomic_flag_clear(volatile atomic_t *ptr)
{
    do
    {
        __LDREXW(ptr);
    } while ((__STREXW(0, ptr)) != 0U);
}

atomic_t hw_atomic_flag_test_and_set(volatile atomic_t *ptr)
{
    atomic_t oldval;
    do
    {
        oldval = __LDREXW(ptr);
    } while ((__STREXW(1, ptr)) != 0U);
    return oldval;
}

atomic_t hw_atomic_compare_exchange_strong(volatile atomic_t *ptr, atomic_t *old, atomic_t new)
{
    atomic_t result;
    atomic_t temp = *old;
    do
    {
        result = __LDREXW(ptr);
        if (result != temp)
        {
            *old = result;
            __STREXW(result, ptr);
            break;
        }
    } while ((__STREXW(new, ptr)) != 0U);
    return (result == temp);
}
/* Private functions ---------------------------------------------------------*/

