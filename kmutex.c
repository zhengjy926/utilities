/**
  ******************************************************************************
  * @file        : kmutex.c
  * @author      : ZJY
  * @version     : V1.0
  * @date        : 2025-01-27
  * @brief       : 通用互斥锁抽象层实现
  * @attention   : 支持裸机和多种RTOS
  ******************************************************************************
  * @history     :
  *         V1.0 : 1.初始版本,支持裸机、FreeRTOS、RT-Thread
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "kmutex.h"
#include "errno-base.h"

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
#ifdef OS_BARE_METAL
static volatile uint32_t interrupt_nest = 0;  /* 中断嵌套计数 */
#endif

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/* ========================================================================== */
/* 互斥锁实现 (Mutex)                                                         */
/* ========================================================================== */

/**
  * @brief  创建互斥锁
  */
int kmutex_create(kmutex_t *mutex)
{
    if (!mutex) {
        return -EINVAL;
    }

#ifdef OS_FREERTOS
    *mutex = xSemaphoreCreateMutex();
    if (*mutex == NULL) {
        return -ENOMEM;
    }
    return 0;

#elif defined(OS_RT_THREAD)
    *mutex = rt_mutex_create("mutex", RT_IPC_FLAG_PRIO);
    if (*mutex == RT_NULL) {
        return -ENOMEM;
    }
    return 0;

#elif defined(OS_CMSIS_RTOS)
    *mutex = osMutexNew(NULL);
    if (*mutex == NULL) {
        return -ENOMEM;
    }
    return 0;

#else
    /* 裸机环境无需创建 */
    *mutex = 0;
    return 0;
#endif
}

/**
  * @brief  删除互斥锁
  */
int kmutex_delete(kmutex_t *mutex)
{
    if (!mutex) {
        return -EINVAL;
    }

#ifdef OS_FREERTOS
    if (*mutex) {
        vSemaphoreDelete(*mutex);
        *mutex = NULL;
    }
    return 0;

#elif defined(OS_RT_THREAD)
    if (*mutex) {
        rt_mutex_delete(*mutex);
        *mutex = RT_NULL;
    }
    return 0;

#elif defined(OS_CMSIS_RTOS)
    if (*mutex) {
        osMutexDelete(*mutex);
        *mutex = NULL;
    }
    return 0;

#else
    /* 裸机环境无需删除 */
    *mutex = 0;
    return 0;
#endif
}

/**
  * @brief  获取互斥锁(阻塞)
  */
int kmutex_lock(kmutex_t *mutex)
{
    if (!mutex) {
        return -EINVAL;
    }

#ifdef OS_FREERTOS
    if (xSemaphoreTake(*mutex, portMAX_DELAY) != pdTRUE) {
        return -EAGAIN;
    }
    return 0;

#elif defined(OS_RT_THREAD)
    rt_err_t ret = rt_mutex_take(*mutex, RT_WAITING_FOREVER);
    return (ret == RT_EOK) ? 0 : -EAGAIN;

#elif defined(OS_CMSIS_RTOS)
    osStatus_t status = osMutexAcquire(*mutex, osWaitForever);
    return (status == osOK) ? 0 : -EAGAIN;

#else
    /* 裸机环境: 关中断 */
    __disable_irq();
    (*mutex)++;
    return 0;
#endif
}

/**
  * @brief  尝试获取互斥锁(非阻塞)
  */
int kmutex_trylock(kmutex_t *mutex)
{
    if (!mutex) {
        return -EINVAL;
    }

#ifdef OS_FREERTOS
    if (xSemaphoreTake(*mutex, 0) != pdTRUE) {
        return -EBUSY;
    }
    return 0;

#elif defined(OS_RT_THREAD)
    rt_err_t ret = rt_mutex_take(*mutex, 0);
    return (ret == RT_EOK) ? 0 : -EBUSY;

#elif defined(OS_CMSIS_RTOS)
    osStatus_t status = osMutexAcquire(*mutex, 0);
    return (status == osOK) ? 0 : -EBUSY;

#else
    /* 裸机环境: 检查中断状态 */
    uint32_t primask = __get_PRIMASK();
    if (primask == 0) {  /* 中断未关闭 */
        __disable_irq();
        (*mutex)++;
        return 0;
    }
    return -EBUSY;
#endif
}

/**
  * @brief  获取互斥锁(超时)
  */
int kmutex_lock_timeout(kmutex_t *mutex, int32_t timeout_ms)
{
    if (!mutex) {
        return -EINVAL;
    }

    if (timeout_ms < 0) {
        return kmutex_lock(mutex);
    }

#ifdef OS_FREERTOS
    TickType_t ticks = (timeout_ms == 0) ? 0 : pdMS_TO_TICKS(timeout_ms);
    if (xSemaphoreTake(*mutex, ticks) != pdTRUE) {
        return -ETIMEDOUT;
    }
    return 0;

#elif defined(OS_RT_THREAD)
    rt_int32_t ticks = (timeout_ms == 0) ? 0 : rt_tick_from_millisecond(timeout_ms);
    rt_err_t ret = rt_mutex_take(*mutex, ticks);
    if (ret == -RT_ETIMEOUT) {
        return -ETIMEDOUT;
    }
    return (ret == RT_EOK) ? 0 : -EAGAIN;

#elif defined(OS_CMSIS_RTOS)
    uint32_t ticks = (timeout_ms == 0) ? 0 : timeout_ms;
    osStatus_t status = osMutexAcquire(*mutex, ticks);
    if (status == osErrorTimeout) {
        return -ETIMEDOUT;
    }
    return (status == osOK) ? 0 : -EAGAIN;

#else
    /* 裸机环境: 无超时概念 */
    return kmutex_trylock(mutex);
#endif
}

/**
  * @brief  释放互斥锁
  */
int kmutex_unlock(kmutex_t *mutex)
{
    if (!mutex) {
        return -EINVAL;
    }

#ifdef OS_FREERTOS
    if (xSemaphoreGive(*mutex) != pdTRUE) {
        return -EINVAL;
    }
    return 0;

#elif defined(OS_RT_THREAD)
    rt_err_t ret = rt_mutex_release(*mutex);
    return (ret == RT_EOK) ? 0 : -EINVAL;

#elif defined(OS_CMSIS_RTOS)
    osStatus_t status = osMutexRelease(*mutex);
    return (status == osOK) ? 0 : -EINVAL;

#else
    /* 裸机环境: 开中断 */
    if (*mutex > 0) {
        (*mutex)--;
        if (*mutex == 0) {
            __enable_irq();
        }
    }
    return 0;
#endif
}

/* ========================================================================== */
/* 中断锁实现 (IRQ Lock)                                                      */
/* ========================================================================== */

/**
  * @brief  关中断并保存状态
  */
irq_lock_t irq_lock(void)
{
#ifdef OS_FREERTOS
    /* FreeRTOS: 使用临界区 */
    taskENTER_CRITICAL();
    return 0;  /* FreeRTOS不需要返回状态 */

#elif defined(OS_RT_THREAD)
    /* RT-Thread: 使用调度器锁 */
    return (irq_lock_t)rt_hw_interrupt_disable();

#elif defined(OS_CMSIS_RTOS)
    /* CMSIS-RTOS: 进入临界区 */
    return (irq_lock_t)osKernelLock();

#else
    /* 裸机: 保存并关中断 */
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
#endif
}

/**
  * @brief  恢复中断状态
  */
void irq_unlock(irq_lock_t level)
{
#ifdef OS_FREERTOS
    /* FreeRTOS: 退出临界区 */
    (void)level;
    taskEXIT_CRITICAL();

#elif defined(OS_RT_THREAD)
    /* RT-Thread: 恢复中断 */
    rt_hw_interrupt_enable(level);

#elif defined(OS_CMSIS_RTOS)
    /* CMSIS-RTOS: 退出临界区 */
    osKernelRestoreLock(level);

#else
    /* 裸机: 恢复中断状态 */
    __set_PRIMASK(level);
#endif
}

/**
  * @brief  进入临界区
  */
irq_lock_t critical_enter(void)
{
#ifdef OS_FREERTOS
    taskENTER_CRITICAL();
    return 0;

#elif defined(OS_RT_THREAD)
    rt_base_t level = rt_enter_critical();
    return (irq_lock_t)level;

#elif defined(OS_CMSIS_RTOS)
    return (irq_lock_t)osKernelLock();

#else
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
#endif
}

/**
  * @brief  退出临界区
  */
void critical_exit(irq_lock_t level)
{
#ifdef OS_FREERTOS
    (void)level;
    taskEXIT_CRITICAL();

#elif defined(OS_RT_THREAD)
    rt_exit_critical();

#elif defined(OS_CMSIS_RTOS)
    osKernelRestoreLock(level);

#else
    __set_PRIMASK(level);
#endif
}

/**
  * @brief  检查当前是否在中断上下文
  */
bool in_interrupt(void)
{
#ifdef OS_FREERTOS
    return (xPortIsInsideInterrupt() != 0);

#elif defined(OS_RT_THREAD)
    return (rt_interrupt_get_nest() != 0);

#elif defined(OS_CMSIS_RTOS)
    return (osKernelGetState() == osKernelInterrupt);

#else
    /* 裸机: 检查中断嵌套计数 */
    return (interrupt_nest > 0);
#endif
}

/* ========================================================================== */
/* 自旋锁实现 (Spinlock)                                                      */
/* ========================================================================== */

/**
  * @brief  初始化自旋锁
  */
void spinlock_init(spinlock_t *lock)
{
    if (lock) {
        lock->lock = 0;
        lock->irq_state = 0;
    }
}

/**
  * @brief  获取自旋锁(关中断)
  */
void spinlock_lock(spinlock_t *lock)
{
    if (!lock) {
        return;
    }

    /* 关中断 */
    lock->irq_state = irq_lock();

    /* 忙等待获取锁 */
    while (__LDREXW(&lock->lock) != 0 || __STREXW(1, &lock->lock) != 0) {
        /* 自旋等待 */
    }

    /* 内存屏障 */
    __DMB();
}

/**
  * @brief  尝试获取自旋锁
  */
bool spinlock_trylock(spinlock_t *lock)
{
    if (!lock) {
        return false;
    }

    /* 关中断 */
    lock->irq_state = irq_lock();

    /* 尝试获取锁 */
    if (__LDREXW(&lock->lock) == 0 && __STREXW(1, &lock->lock) == 0) {
        __DMB();
        return true;
    }

    /* 失败,恢复中断 */
    irq_unlock(lock->irq_state);
    return false;
}

/**
  * @brief  释放自旋锁(恢复中断)
  */
void spinlock_unlock(spinlock_t *lock)
{
    if (!lock) {
        return;
    }

    /* 内存屏障 */
    __DMB();

    /* 释放锁 */
    lock->lock = 0;

    /* 恢复中断 */
    irq_unlock(lock->irq_state);
}

/* Private functions ---------------------------------------------------------*/

#ifdef OS_BARE_METAL
/**
  * @brief  进入中断(裸机环境)
  * @note   在中断服务程序开始时调用
  */
void interrupt_enter(void)
{
    irq_lock_t level = irq_lock();
    interrupt_nest++;
    irq_unlock(level);
}

/**
  * @brief  退出中断(裸机环境)
  * @note   在中断服务程序结束时调用
  */
void interrupt_leave(void)
{
    irq_lock_t level = irq_lock();
    if (interrupt_nest > 0) {
        interrupt_nest--;
    }
    irq_unlock(level);
}
#endif

