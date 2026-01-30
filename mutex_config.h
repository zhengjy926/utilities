/**
  ******************************************************************************
  * @file        : mutex_config.h
  * @author      : ZJY
  * @version     : V1.0
  * @date        : 2025-01-27
  * @brief       : 互斥锁配置文件
  * @attention   : 在此配置RTOS类型和相关参数
  ******************************************************************************
  */
#ifndef __MUTEX_CONFIG_H__
#define __MUTEX_CONFIG_H__

/* ========================================================================== */
/* RTOS选择配置                                                               */
/* ========================================================================== */

/**
 * @brief 选择运行环境
 * @note 取消注释对应的宏来选择RTOS
 * @note 如果全部注释,则为裸机环境
 */

/* 选项1: FreeRTOS */
// #define USING_FREERTOS

/* 选项2: RT-Thread */
// #define RT_USING_MUTEX

/* 选项3: CMSIS-RTOS v2 */
// #define CMSIS_OS_H_

/* 选项4: 裸机环境 (默认) */
/* 无需定义任何宏 */

/* ========================================================================== */
/* 功能配置                                                                   */
/* ========================================================================== */

/**
 * @brief 是否启用自旋锁
 * @note 自旋锁适用于SMP系统或极短时间的保护
 * @note 单核系统可以禁用以节省空间
 */
#define MUTEX_ENABLE_SPINLOCK       1

/**
 * @brief 是否启用互斥锁统计
 * @note 启用后可以统计锁的持有时间、竞争次数等
 */
#define MUTEX_ENABLE_STATISTICS     0

/**
 * @brief 是否启用死锁检测
 * @note 启用后会检测循环等待和重复加锁
 * @note 会增加一定开销,建议仅在调试时启用
 */
#define MUTEX_ENABLE_DEADLOCK_DETECT 0

/**
 * @brief 互斥锁超时默认值(毫秒)
 * @note 用于 mutex_lock_timeout 的默认参数
 */
#define MUTEX_DEFAULT_TIMEOUT_MS    1000

/* ========================================================================== */
/* 调试配置                                                                   */
/* ========================================================================== */

/**
 * @brief 是否启用互斥锁调试信息
 */
#define MUTEX_DEBUG_ENABLE          0

#if MUTEX_DEBUG_ENABLE
    #include <stdio.h>
    #define MUTEX_DEBUG(fmt, ...)   printf("[MUTEX] " fmt "\r\n", ##__VA_ARGS__)
#else
    #define MUTEX_DEBUG(fmt, ...)
#endif

/* ========================================================================== */
/* 断言配置                                                                   */
/* ========================================================================== */

/**
 * @brief 互斥锁断言宏
 */
#ifdef DEBUG
    #include <assert.h>
    #define MUTEX_ASSERT(expr)      assert(expr)
#else
    #define MUTEX_ASSERT(expr)
#endif

#endif /* __MUTEX_CONFIG_H__ */

