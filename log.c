/**
  ******************************************************************************
  * @file        : log.c
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
/* Includes ------------------------------------------------------------------*/
#include "log.h"
#include <stdio.h>
#include <string.h>
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* 日志输出器数组 */
static log_handler_t log_handlers[LOG_MAX_HANDLERS];
static uint8_t handler_count = 0;

/* Exported variables  -------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void log_init(void)
{
    
}

/**
 * 注册日志输出处理器
 */
bool log_register_handler(const char *name, log_output_fn output)
{
    if (handler_count >= LOG_MAX_HANDLERS || !name || !output) {
        return false;
    }

    // 检查是否已经注册
    for (uint8_t i = 0; i < handler_count; i++) {
        if (strcmp(log_handlers[i].name, name) == 0) {
            return false;
        }
    }

    // 注册新的处理器
    log_handlers[handler_count].name = name;
    log_handlers[handler_count].output = output;
    log_handlers[handler_count].enabled = true;
    handler_count++;

    return true;
}

/**
 * 注销日志输出处理器
 */
bool log_unregister_handler(const char *name)
{
    for (uint8_t i = 0; i < handler_count; i++) {
        if (strcmp(log_handlers[i].name, name) == 0) {
            // 移动后面的处理器前移
            for (uint8_t j = i; j < handler_count - 1; j++) {
                log_handlers[j] = log_handlers[j + 1];
            }
            handler_count--;
            return true;
        }
    }
    return false;
}

/**
 * 启用指定的日志处理器
 */
void log_enable_handler(const char *name)
{
    for (uint8_t i = 0; i < handler_count; i++) {
        if (strcmp(log_handlers[i].name, name) == 0) {
            log_handlers[i].enabled = true;
            break;
        }
    }
}

/**
 * 禁用指定的日志处理器
 */
void log_disable_handler(const char *name)
{
    for (uint8_t i = 0; i < handler_count; i++) {
        if (strcmp(log_handlers[i].name, name) == 0) {
            log_handlers[i].enabled = false;
            break;
        }
    }
}

void log_output(const char *format, ...)
{
    static char buffer[LOG_BUF_SIZE];
    va_list args;
    int len;

#if USING_RTOS
    LOCK();
#endif
    // 格式化日志消息
    va_start(args, format);
    len = vsnprintf(buffer, LOG_BUF_SIZE, format, args);
    va_end(args);
    
    if (len < 0) {
        len = 0;        /* Handle encoding error */
    }

    if (len >= sizeof(buffer)) {
        /* Message was truncated, indicate it. */ 
        const char trunc_str[] = "...";
        strcpy(buffer + LOG_BUF_SIZE - sizeof(trunc_str), trunc_str);
        len = sizeof(buffer) - 1;
    }
    
    // 调用所有已注册且启用的处理器
    for (uint8_t i = 0; i < handler_count; i++) {
        if (log_handlers[i].enabled) {
            log_handlers[i].output(buffer, len);
        }
    }
#if USING_RTOS
    UNLOCK();
#endif
}
/* Private functions ---------------------------------------------------------*/

