/**
  ******************************************************************************
  * @file        : stimer.h
  * @author      : 
  * @version     : V1.0
  * @data        : 
  * @brief       : Software timer header file for bare metal environment
  * @attattention: None
  ******************************************************************************
  */

#ifndef __STIMER_H__
#define __STIMER_H__

#include <stdint.h>
#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "sys_def.h"
#include "my_list.h"

/* Exported define -----------------------------------------------------------*/
#define STIMER_AUTO_RELOAD      1
#define STIMER_ONE_SHOT         0

/* Exported typedef ----------------------------------------------------------*/
/**
 * @brief Software timer structure
 */
typedef struct {
    uint32_t    period;            /* Timer period */
    uint32_t    expire_time;       /* Expiration time */
    uint8_t     auto_reload;        /* Auto reload flag */
    uint8_t     active;             /* Active status */
    void        (*callback)(void*);    /* Callback function */
    void        *callback_arg;         /* Callback function argument */
    list_t     node;               /* List node for timer */
} stimer_t;

/**
 * @brief Function pointer type for getting system time
 */
typedef uint32_t (*STIMER_GET_TICK_FUNC_T)(void);

/* Exported macro ------------------------------------------------------------*/
/* Exported variables -------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
int     stimer_init         (STIMER_GET_TICK_FUNC_T get_tick_func);

int     stimer_create       (stimer_t* timer,
                             uint32_t period,
                             uint8_t auto_reload,
                             void (*callback)(void*),
                             void *callback_arg);

void    stimer_delete       (stimer_t* timer);
int     stimer_start        (stimer_t* timer);
int     stimer_stop         (stimer_t* timer);
int     stimer_change_period(stimer_t* timer, uint32_t period);
int     stimer_reset        (stimer_t* timer);
int8_t  stimer_get_status   (stimer_t* timer);
void    stimer_service      (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STIMER_H__ */
