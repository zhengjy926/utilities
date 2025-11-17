/**
  ******************************************************************************
  * @file        : kmutex.h
  * @author      : ZJY
  * @version     : V1.0
  * @date        : 2025-01-27
  * @brief       : 通用互斥锁抽象层接口
  * @attention   : 支持裸机和多种RTOS
  ******************************************************************************
  * @history     :
  *         V1.0 : 1.初始版本,支持裸机、FreeRTOS、RT-Thread
  ******************************************************************************
  */
#ifndef __KMUTEX_H__
#define __KMUTEX_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* 检测RTOS类型 */
#if defined(USING_FREERTOS) || defined(configUSE_MUTEXES)
    #define OS_FREERTOS
    #include "FreeRTOS.h"
    #include "task.h"
    #include "semphr.h"
#elif defined(RT_USING_MUTEX)
    #define OS_RT_THREAD
    #include "rtthread.h"
#elif defined(CMSIS_OS_H_)
    #define OS_CMSIS_RTOS
    #include "cmsis_os.h"
#else
    #define OS_BARE_METAL
#endif

/* Exported define -----------------------------------------------------------*/

/**
 * @brief 互斥锁类型定义
 */
#ifdef OS_FREERTOS
    typedef SemaphoreHandle_t kmutex_t;
    #define KMUTEX_INITIALIZER NULL
#elif defined(OS_RT_THREAD)
    typedef struct rt_mutex *kmutex_t;
    #define KMUTEX_INITIALIZER NULL
#elif defined(OS_CMSIS_RTOS)
    typedef osMutexId_t kmutex_t;
    #define KMUTEX_INITIALIZER NULL
#else
    /* 裸机环境: 使用中断锁 */
    typedef uint8_t kmutex_t;
    #define KMUTEX_INITIALIZER 0
#endif

/**
 * @brief 中断锁类型定义 (用于保护临界区)
 */
typedef uint32_t irq_lock_t;

/**
 * @brief 自旋锁类型定义 (用于短时间保护)
 */
typedef struct {
    volatile uint32_t lock;
    irq_lock_t irq_state;
} spinlock_t;

#define SPINLOCK_INITIALIZER {0, 0}

/* Exported typedef ----------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/**
 * @brief 静态初始化互斥锁
 */
#define KMUTEX_INIT(name)  kmutex_t name = KMUTEX_INITIALIZER

/**
 * @brief 静态初始化自旋锁
 */
#define SPINLOCK_INIT(name)  spinlock_t name = SPINLOCK_INITIALIZER

/* Exported variable prototypes ----------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/

/**
 * @defgroup 互斥锁接口
 * @brief 适用于可睡眠上下文,保护较长时间的临界区
 * @{
 */

/**
 * @brief 创建互斥锁
 * @param mutex 互斥锁指针
 * @return 0 成功, 负数表示错误码
 * @note 裸机环境下无需创建,返回0
 */
int kmutex_create(kmutex_t *mutex);

/**
 * @brief 删除互斥锁
 * @param mutex 互斥锁指针
 * @return 0 成功, 负数表示错误码
 */
int kmutex_delete(kmutex_t *mutex);

/**
 * @brief 获取互斥锁(阻塞)
 * @param mutex 互斥锁指针
 * @return 0 成功, 负数表示错误码
 * @note 裸机环境下退化为关中断
 */
int kmutex_lock(kmutex_t *mutex);

/**
 * @brief 尝试获取互斥锁(非阻塞)
 * @param mutex 互斥锁指针
 * @return 0 成功, 负数表示错误码
 */
int kmutex_trylock(kmutex_t *mutex);

/**
 * @brief 获取互斥锁(超时)
 * @param mutex 互斥锁指针
 * @param timeout_ms 超时时间(毫秒), 0表示立即返回, -1表示永久等待
 * @return 0 成功, 负数表示错误码
 */
int kmutex_lock_timeout(kmutex_t *mutex, int32_t timeout_ms);

/**
 * @brief 释放互斥锁
 * @param mutex 互斥锁指针
 * @return 0 成功, 负数表示错误码
 */
int kmutex_unlock(kmutex_t *mutex);

/** @} */

/**
 * @defgroup 中断锁接口
 * @brief 适用于所有上下文,保护极短时间的临界区
 * @{
 */

/**
 * @brief 关中断并保存状态
 * @return 中断状态
 * @note 适用于裸机和RTOS环境
 */
irq_lock_t irq_lock(void);

/**
 * @brief 恢复中断状态
 * @param level 之前保存的中断状态
 */
void irq_unlock(irq_lock_t level);

/**
 * @brief 进入临界区
 * @return 中断状态
 * @note 在RTOS中可能使用调度器锁
 */
irq_lock_t critical_enter(void);

/**
 * @brief 退出临界区
 * @param level 之前保存的状态
 */
void critical_exit(irq_lock_t level);

/**
 * @brief 检查当前是否在中断上下文
 * @return true 在中断上下文, false 不在
 */
bool in_interrupt(void);

/** @} */

/**
 * @defgroup 自旋锁接口
 * @brief 适用于SMP系统或需要忙等待的场景
 * @{
 */

/**
 * @brief 初始化自旋锁
 * @param lock 自旋锁指针
 */
void spinlock_init(spinlock_t *lock);

/**
 * @brief 获取自旋锁(关中断)
 * @param lock 自旋锁指针
 * @note 会关中断并忙等待
 */
void spinlock_lock(spinlock_t *lock);

/**
 * @brief 尝试获取自旋锁
 * @param lock 自旋锁指针
 * @return true 成功, false 失败
 */
bool spinlock_trylock(spinlock_t *lock);

/**
 * @brief 释放自旋锁(恢复中断)
 * @param lock 自旋锁指针
 */
void spinlock_unlock(spinlock_t *lock);

/** @} */

/**
 * @defgroup 辅助宏
 * @{
 */

/**
 * @brief 互斥锁保护代码块
 * @param mutex 互斥锁
 * @example
 *   KMUTEX_GUARD(my_mutex) {
 *       // 受保护的代码
 *   }
 */
#define KMUTEX_GUARD(mutex) \
    for (int _i = (kmutex_lock(&(mutex)), 0); \
         _i == 0; \
         _i = 1, kmutex_unlock(&(mutex)))

/**
 * @brief 中断锁保护代码块
 * @example
 *   IRQ_GUARD() {
 *       // 受保护的代码
 *   }
 */
#define IRQ_GUARD() \
    for (irq_lock_t _level = irq_lock(), _i = 0; \
         _i == 0; \
         _i = 1, irq_unlock(_level))

/**
 * @brief 临界区保护代码块
 * @example
 *   CRITICAL_GUARD() {
 *       // 受保护的代码
 *   }
 */
#define CRITICAL_GUARD() \
    for (irq_lock_t _level = critical_enter(), _i = 0; \
         _i == 0; \
         _i = 1, critical_exit(_level))

/**
 * @brief 自旋锁保护代码块
 * @param lock 自旋锁
 * @example
 *   SPINLOCK_GUARD(my_lock) {
 *       // 受保护的代码
 *   }
 */
#define SPINLOCK_GUARD(lock) \
    for (int _i = (spinlock_lock(&(lock)), 0); \
         _i == 0; \
         _i = 1, spinlock_unlock(&(lock)))

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __KMUTEX_H__ */

