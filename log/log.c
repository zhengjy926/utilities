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
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Exported variables  -------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void log_init(void)
{
    
}

/**
 * output the log
 *
 * @param format output format
 */
void log_output(const char *format, ...)
{
    char acBuffer[SEGGER_RTT_PRINTF_BUFFER_SIZE];
    va_list args;
    size_t len;

#if USING_RTOS
    SEGGER_RTT_LOCK();
#endif
    va_start(args, format);
    len = vsnprintf(acBuffer, sizeof(acBuffer), format, args);
    if (len > 255)
    {
        len = 255;
    }
    va_end(args);
    SEGGER_RTT_Write(0, acBuffer, len);
#if USING_RTOS
    SEGGER_RTT_UNLOCK();
#endif
}
/* Private functions ---------------------------------------------------------*/

