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
#include "sys_conf.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

/* Exported types ------------------------------------------------------------*/

/**
 * @brief 日志输出回调函数类型定义
 * @param msg 日志消息
 * @param len 消息长度
 */
typedef void (*log_output_fn)(const char *msg, size_t len);

/**
 * @brief 日志输出器结构体
 */
typedef struct {
    const char *name;          // 输出器名称
    log_output_fn output;      // 输出回调函数
    bool enabled;              // 是否启用
} log_handler_t;

/* Exported constants --------------------------------------------------------*/
/**
 * @defgroup 日志级别
 * @{
 */
#define LOG_LVL_ERROR                   0   /**< Error conditions */
#define LOG_LVL_WARNING                 1   /**< Warning conditions */
#define LOG_LVL_INFO                    2   /**< Informational messages */
#define LOG_LVL_DEBUG                   3   /**< Debug-level messages */
/**
  * @}
  */

/**
 * @defgroup 颜色定义
 * @{
 */
#define LOG_CLR_RESET   "\033[0m"
#define LOG_CLR_RED     "\033[31m"
#define LOG_CLR_GREEN   "\033[32m"
#define LOG_CLR_YELLOW  "\033[33m"
#define LOG_CLR_BLUE    "\033[34m"
#define LOG_CLR_MAGENTA "\033[35m"
#define LOG_CLR_CYAN    "\033[36m"
/**
  * @}
  */

/**
 * @defgroup 系统配置（可在 sys_conf.h 中覆盖）
 * @{
 */
#ifndef LOG_MAX_HANDLERS
    #define LOG_MAX_HANDLERS            4
#endif

#ifndef LOG_BUF_SIZE
    #define LOG_BUF_SIZE                (1024)
#endif

#ifndef LOG_AUTO_LF
    #define LOG_AUTO_LF                 1       /* 自动在结尾补 '\n'（若用户未写换行） */
#endif

#ifndef LOG_GLOBAL_LVL
    #define LOG_GLOBAL_LVL              LOG_LVL_DEBUG
#endif

#ifndef LOG_ASSERT_ENABLE
    #define LOG_ASSERT_ENABLE           0
#endif

#if (LOG_ASSERT_ENABLE == 1)
    #define LOG_ASSERT(expr) \
            do { \
                if (!(expr)) { \
                    log_output(LOG_LVL_ERROR, LOG_CLR_RED"Wrong parameters value: file %s on line %d\r\n", __FILE__, __LINE__); \
                    while(1); \
                } \
            } while (0)
#else
    #define LOG_ASSERT(expr) ((void)0)
#endif
/**
  * @}
  */

/**
 * @defgroup 调试信息配置（可在具体 .c 文件中覆盖）
 * @{
 */
#ifndef LOG_TAG
    #define LOG_TAG                     "NO_TAG"
#endif /* LOG_TAG */

#ifndef LOG_LVL
    #define LOG_LVL                     LOG_LVL_DEBUG
#endif /* LOG_LVL */
/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
#ifndef LOG_LOCK
    #define LOG_LOCK()                  ((void)0)
#endif
#ifndef LOG_UNLOCK
    #define LOG_UNLOCK()                ((void)0)
#endif

#if (LOG_LVL >= LOG_LVL_DEBUG) && (LOG_GLOBAL_LVL >= LOG_LVL_DEBUG)
    #define LOG_D(fmt, ...) log_output(LOG_LVL_DEBUG, LOG_CLR_RESET "[DBG][" LOG_TAG "] " fmt, ##__VA_ARGS__)
#else
    #define LOG_D(...)
#endif

#if (LOG_LVL >= LOG_LVL_INFO) && (LOG_GLOBAL_LVL >= LOG_LVL_INFO)
    #define LOG_I(fmt, ...) log_output(LOG_LVL_INFO, LOG_CLR_GREEN "[INF][" LOG_TAG "]" fmt LOG_CLR_RESET, ##__VA_ARGS__)
#else
    #define LOG_I(...)
#endif

#if (LOG_LVL >= LOG_LVL_WARNING) && (LOG_GLOBAL_LVL >= LOG_LVL_WARNING)
    #define LOG_W(fmt, ...) log_output(LOG_LVL_WARNING, LOG_CLR_YELLOW  "[WRN][" LOG_TAG "] " fmt LOG_CLR_RESET, ##__VA_ARGS__)
#else
    #define LOG_W(...)
#endif

#if (LOG_LVL >= LOG_LVL_ERROR) && (LOG_GLOBAL_LVL >= LOG_LVL_ERROR)
    #define LOG_E(fmt, ...) log_output(LOG_LVL_ERROR, LOG_CLR_RED "[ERR][" LOG_TAG "]" fmt LOG_CLR_RESET, ##__VA_ARGS__)
#else
    #define LOG_E(...)
#endif

/* Exported variables --------------------------------------------------------*/


/* Exported functions --------------------------------------------------------*/
/** @addtogroup 初始化和注册函数
  * @{
  */
void log_init(void);
bool log_register_handler(const char *name, log_output_fn output);
bool log_unregister_handler(const char *name);
/**
  * @}
  */


void log_enable_handler(const char *name);
void log_disable_handler(const char *name);

void     log_set_global_level(uint8_t lvl);
uint8_t  log_get_global_level(void);

void log_output(uint8_t level, const char *format, ...);
void log_voutput(const char *format, va_list ap);

void log_hexdump(const char *tag, const void *buf, size_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LOG_H__ */

