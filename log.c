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
static log_handler_t s_handlers[LOG_MAX_HANDLERS];
static uint8_t       s_handler_count;
static uint8_t       s_global_lvl = LOG_GLOBAL_LVL;

/* Exported variables  -------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static inline int find_handler(const char *name);

/* Exported functions --------------------------------------------------------*/

void log_init(void)
{
    memset((void*)s_handlers, 0, sizeof(s_handlers));
    s_handler_count = 0;
}

/**
 * 注册日志输出处理器
 */
bool log_register_handler(const char *name, log_output_fn output)
{
    if (!name || !output)
        return false;

    LOG_LOCK();
    if (s_handler_count >= LOG_MAX_HANDLERS) {
        LOG_UNLOCK();
        return false;
    }
    if (find_handler(name) >= 0) {
        LOG_UNLOCK();
        return false;
    }

    s_handlers[s_handler_count].name    = name;
    s_handlers[s_handler_count].output  = output;
    s_handlers[s_handler_count].enabled = true;
    s_handler_count++;
    LOG_UNLOCK();

    return true;
}

/**
 * 注销日志输出处理器
 */
bool log_unregister_handler(const char *name)
{
    LOG_LOCK();
    int idx = find_handler(name);
    if (idx < 0) {
        LOG_UNLOCK();
        return false;
    }
    for (uint8_t j = (uint8_t)idx; j + 1 < s_handler_count; ++j) {
        s_handlers[j] = s_handlers[j + 1];
    }
    s_handler_count--;
    LOG_UNLOCK();
    return true;
}

void log_set_handler_enabled(const char *name, bool en)
{
    LOG_LOCK();
    int idx = find_handler(name);
    if (idx >= 0)
        s_handlers[idx].enabled = en;
    LOG_UNLOCK();
}

void log_enable_handler(const char *name)
{
    log_set_handler_enabled(name, true);
}

void log_disable_handler(const char *name){
    log_set_handler_enabled(name, false);
}

void log_output(uint8_t level, const char *format, ...)
{
    if (level > s_global_lvl || s_handler_count == 0)
        return;

    char buf[LOG_BUF_SIZE];
    size_t off = 0;

#if LOG_WITH_TIMESTAMP
    uint32_t now = log_time_now();
    int t_len = snprintf(buf, LOG_BUF_SIZE, "[%lu] ", (unsigned long)now);
    if (t_len > 0) off += (size_t)t_len;
#endif

    va_list ap;
    va_start(ap, format);
    int body_len = vsnprintf(buf + off, LOG_BUF_SIZE - off, format, ap);
    va_end(ap);

    if (body_len < 0) return;
    off += (size_t)body_len;

    /* 截断与换行处理 */
    if (off >= LOG_BUF_SIZE) {
        off = LOG_BUF_SIZE - 1;
        buf[off-1] = '\n'; // 强制换行
    } else if (off > 0 && buf[off-1] != '\n') {
        buf[off++] = '\n';
    }
    
    for (uint8_t i = 0; i < s_handler_count; ++i) {
        if (s_handlers[i].enabled && s_handlers[i].output) {
            s_handlers[i].output(buf, off);
        }
    }
}

/* Private functions ---------------------------------------------------------*/

static inline int find_handler(const char *name)
{
    if (!name) return -1;
    for (uint8_t i = 0; i < s_handler_count; ++i) {
        if (s_handlers[i].name && strcmp(s_handlers[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}
