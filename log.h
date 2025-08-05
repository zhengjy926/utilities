/**
  ******************************************************************************
  * @file        : log.h
  * @author      : ZJY
  * @version     : V1.0
  * @date        : 2025-06-01
  * @brief       : 
  * @attention   : None
  ******************************************************************************
  * @history     :
  *         V1.0 : 1.xxx
  ******************************************************************************
  */
#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "sys_def.h"
#include <assert.h>

/* Exported define -----------------------------------------------------------*/
#define LOG_LVL_ASSERT                  0   /**<  */
#define LOG_LVL_ERROR                   1   /**< Error conditions */
#define LOG_LVL_WARNING                 2   /**< Warning conditions */
#define LOG_LVL_INFO                    3   /**< Informational messages */
#define LOG_LVL_DEBUG                   4   /**< Debug-level messages */

/* Default log tag if not defined in the source file */ 
#ifndef LOG_TAG
    #define LOG_TAG "NO_TAG"
#endif /* LOG_TAG */

#ifndef LOG_LVL
    #define LOG_LVL                     LOG_LVL_DEBUG
#endif /* LOG_LVL */

/* Maximum number of log output handlers */
#ifndef LOG_MAX_HANDLERS
    #define LOG_MAX_HANDLERS            4
#endif

/*  */
#ifndef LOG_BUF_SIZE
    #define LOG_BUF_SIZE                (1024)
#endif

#if (LOG_LVL >= LOG_LVL_DEBUG) && (LOG_GLOBAL_LVL >= LOG_LVL_DEBUG)
    #define LOG_D(fmt, ...)           log_output("[%s][DBG]" fmt, LOG_TAG, ##__VA_ARGS__)
#else
    #define LOG_D(fmt, ...)
#endif

#if (LOG_LVL >= LOG_LVL_INFO) && (LOG_GLOBAL_LVL >= LOG_LVL_INFO)
    #define LOG_I(fmt, ...)           log_output("[%s][INF]" fmt, LOG_TAG, ##__VA_ARGS__)
#else
    #define LOG_I(fmt, ...)
#endif

#if (LOG_LVL >= LOG_LVL_WARNING) && (LOG_GLOBAL_LVL >= LOG_LVL_WARNING)
    #define LOG_W(fmt, ...)           log_output("[%s][WARN]" fmt, LOG_TAG, ##__VA_ARGS__)
#else
    #define LOG_W(fmt, ...)
#endif

#if (LOG_LVL >= LOG_LVL_ERROR) && (LOG_GLOBAL_LVL >= LOG_LVL_ERROR)
    #define LOG_E(fmt, ...)           log_output("[%s][ERR]" fmt, LOG_TAG, ##__VA_ARGS__)
#else
    #define LOG_E(fmt, ...)
#endif

/* assert for developer. */
#if (LOG_ASSERT_ENABLE == 0)
    #define NODEBUG
#endif

/* Exported typedef ----------------------------------------------------------*/
/**
 * 日志输出回调函数类型定义
 * @param msg   日志消息
 * @param len   消息长度
 */
typedef void (*log_output_fn)(const char *msg, size_t len);

/**
 * 日志输出器结构体
 */
typedef struct {
    const char *name;          // 输出器名称
    log_output_fn output;      // 输出回调函数
    bool enabled;              // 是否启用
} log_handler_t;
/* Exported macro ------------------------------------------------------------*/

/* Exported variable prototypes ----------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/
void log_init(void);
bool log_register_handler(const char *name, log_output_fn output);
bool log_unregister_handler(const char *name);
void log_enable_handler(const char *name);
void log_disable_handler(const char *name);
void log_output(const char *format, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LOG_H__ */

