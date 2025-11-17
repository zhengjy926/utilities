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
static volatile uint8_t s_runtime_global_lvl = LOG_GLOBAL_LVL;
static char s_buffer[LOG_BUF_SIZE];

/* Exported variables  -------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
static inline int find_handler(const char *name)
{
    if (!name) return -1;
    for (uint8_t i = 0; i < s_handler_count; ++i) {
        if (s_handlers[i].name && strcmp(s_handlers[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

static inline bool any_handler_enabled(void)
{
    for (uint8_t i = 0; i < s_handler_count; ++i) {
        if (s_handlers[i].enabled) return true;
    }
    return false;
}

void log_init(void)
{
    memset((void*)s_handlers, 0, sizeof(s_handlers));
    s_handler_count = 0;
    s_runtime_global_lvl = LOG_GLOBAL_LVL;
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

void log_voutput(const char *format, va_list ap)
{
    if (!format)
        return;
    
    if (!any_handler_enabled())
        return;

    LOG_LOCK(); /* 静态缓冲：加大锁粒度，保护“格式化+分发” */
    char *buf = s_buffer;
    size_t off = 0;

#if LOG_WITH_TIMESTAMP
    /* 追加时间戳前缀，例如 [123456ms]  */
    uint32_t ts = log_time_now();
    int n = snprintf(buf, LOG_BUF_SIZE, "[%lu]", (unsigned long)ts);
    if (n < 0) { LOG_UNLOCK(); return; }
    off = (size_t)n;
    if (off >= LOG_BUF_SIZE) { off = LOG_BUF_SIZE - 1; buf[off] = '\0'; }
#endif

    /* 主体格式化 */
    int rem = (int)((off < LOG_BUF_SIZE) ? (LOG_BUF_SIZE - off) : 0);
    if (rem <= 0) { 
        buf[LOG_BUF_SIZE - 1] = '\0'; 
    } else {
        int m = vsnprintf(&buf[off], (size_t)rem, format, ap);
        if (m < 0) {
            LOG_UNLOCK();
            return;
        }
        size_t len = (size_t)m;

        /* 截断检测与安全裁剪：末尾放置 ... 并确保 '\0' */
        if ((size_t)m >= (size_t)rem) {
            const char dots[] = "...";
            const size_t tlen = 3;
            size_t pos = LOG_BUF_SIZE - 1; /* 末尾 '\0' 位置 */
            size_t start = (pos >= tlen) ? (pos - tlen) : 0;
            memcpy(&buf[start], dots, tlen);
            buf[LOG_BUF_SIZE - 1] = '\0';
        } else {
            off += len;
            /* 自动补 '\n' */
#if LOG_AUTO_LF
            if (off > 0 && buf[off - 1] != '\n') {
                if (off + 1 < LOG_BUF_SIZE) {
                    buf[off++] = '\n';
                    buf[off]   = '\0';
                } else {
                    buf[LOG_BUF_SIZE - 1] = '\n';
                }
            }
#endif
        }
    }

    /* 分发到所有已启用 handler */
    size_t out_len = strnlen(buf, LOG_BUF_SIZE);
    for (uint8_t i = 0; i < s_handler_count; ++i) {
        if (s_handlers[i].enabled && s_handlers[i].output) {
            s_handlers[i].output(buf, out_len);
        }
    }

    LOG_UNLOCK();
}

void log_output(const char *format, ...)
{
    (void)s_runtime_global_lvl;
    
    va_list ap;
    va_start(ap, format);
    log_voutput(format, ap);
    va_end(ap);
}

void log_hexdump(const char *tag, const void *buf, size_t len)
{
    if (!buf || len == 0) return;

    /* 保守地分行打印：每行 16 字节 */
    const uint8_t *p = (const uint8_t*)buf;
    char line[128]; /* 足够容纳一行 16B 的十六进制与 ASCII */
    size_t offset = 0;

    while (offset < len) {
        size_t n = (len - offset > 16) ? 16 : (len - offset);
        size_t w = 0;

        /* 行头：可加 tag */
#if LOG_WITH_TIMESTAMP
        w += (size_t)snprintf(line + w, sizeof(line) - w, "[%lu]", (unsigned long)log_time_now());
#endif
        if (tag) {
            w += (size_t)snprintf(line + w, sizeof(line) - w, "[%s][HEX] ", tag);
        } else {
            w += (size_t)snprintf(line + w, sizeof(line) - w, "[HEX] ");
        }

        /* 十六进制区域 */
        for (size_t i = 0; i < 16; ++i) {
            if (i < n) w += (size_t)snprintf(line + w, sizeof(line) - w, "%02X ", p[offset + i]);
            else       w += (size_t)snprintf(line + w, sizeof(line) - w, "   ");
        }
        w += (size_t)snprintf(line + w, sizeof(line) - w, " |");

        /* ASCII 区域 */
        for (size_t i = 0; i < n; ++i) {
            uint8_t c = p[offset + i];
            w += (size_t)snprintf(line + w, sizeof(line) - w, "%c", (c >= 32 && c < 127) ? c : '.');
        }
        w += (size_t)snprintf(line + w, sizeof(line) - w, "|\n");

        /* 输出一行 */
        for (uint8_t i = 0; i < s_handler_count; ++i) {
            if (s_handlers[i].enabled && s_handlers[i].output) {
                s_handlers[i].output(line, strnlen(line, sizeof(line)));
            }
        }
        offset += n;
    }
}
/* Private functions ---------------------------------------------------------*/

