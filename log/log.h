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
#include "SEGGER_RTT.h"
#include "stm32_dwt.h"
/* Exported define -----------------------------------------------------------*/
#define LOG_OUTPUT_LVL                  7

#define LOG_LVL_ASSERT                  0
#define LOG_LVL_ERROR                   3
#define LOG_LVL_WARNING                 4
#define LOG_LVL_INFO                    6
#define LOG_LVL_DBG                     7

#ifndef LOG_TAG
    #define LOG_TAG "NO_TAG"
#endif /* LOG_TAG */

#ifndef LOG_LVL
    #define LOG_LVL                LOG_LVL_DBG
#endif /* LOG_LVL */

#if (LOG_LVL >= LOG_LVL_DBG) && (LOG_OUTPUT_LVL >= LOG_LVL_DBG)
    #define LOG_D(fmt, ...)           log_output("[%.6fs][%s][DBG]" fmt, dwt_get_seconds(), LOG_TAG, ##__VA_ARGS__)
#else
    #define LOG_D(TAG, ...)
#endif /* (LOG_LVL >= LOG_LVL_DBG) && (LOG_OUTPUT_LVL >= LOG_LVL_DBG) */

#if (LOG_LVL >= LOG_LVL_INFO) && (LOG_OUTPUT_LVL >= LOG_LVL_INFO)
    #define LOG_I(fmt, ...)           log_output("[%s][INF]" fmt, LOG_TAG, ##__VA_ARGS__)
#else
    #define LOG_I(TAG, ...)
#endif /* (LOG_LVL >= LOG_LVL_INFO) && (LOG_OUTPUT_LVL >= LOG_LVL_INFO) */

#if (LOG_LVL >= LOG_LVL_WARNING) && (LOG_OUTPUT_LVL >= LOG_LVL_WARNING)
    #define LOG_W(fmt, ...)           log_output("[%s][WARN]" fmt, LOG_TAG, ##__VA_ARGS__)
#else
    #define LOG_W(TAG, ...)
#endif /* (LOG_LVL >= LOG_LVL_WARNING) && (LOG_OUTPUT_LVL >= LOG_LVL_WARNING) */

#if (LOG_LVL >= LOG_LVL_ERROR) && (LOG_OUTPUT_LVL >= LOG_LVL_ERROR)
    #define LOG_E(fmt, ...)           log_output("[%s][ERR]" fmt, LOG_TAG, ##__VA_ARGS__)
#else
    #define LOG_E(TAG, ...)
#endif /* (LOG_LVL >= LOG_LVL_ERROR) && (LOG_OUTPUT_LVL >= LOG_LVL_ERROR) */

/* assert for developer. */
#ifdef LOG_ASSERT_ENABLE
    #define LOG_ASSERT(EXPR)                                                  \
    if (!(EXPR))                                                              \
    {                                                                         \
        log_output("(%s) has assert failed at %s:%ld.\r\n", #EXPR, __FUNCTION__, __LINE__); \
        while (1);                                                            \
    }
#else
    #define LOG_ASSERT(EXPR)
#endif

/* ASSERT API definition */
#if !defined(ASSERT)
    #define ASSERT           LOG_ASSERT
#endif
/* Exported typedef ----------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported variable prototypes ----------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/
void log_init(void);
void log_output(const char *format, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LOG_H__ */

