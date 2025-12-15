/**
  ******************************************************************************
  * @file        : stimer.c
  * @author      :
  * @version     : V1.0
  * @data        :
  * @brief       : Software timer implementation for bare metal environment
  * @attattention: None
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stimer.h"
#include "errno-base.h"
#include <string.h>
#include <stdlib.h>
#include "sort_algo.h"
#include "types.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
LIST_HEAD(stimer_list);  /* Timer list head */
static STIMER_GET_TICK_FUNC_T stimer_tick_func = NULL; /* Function pointer to get system tick */
static uint32_t last_service_time = 0;  /* Last service time for drift compensation */
static uint32_t accumulated_drift = 0;  /* Accumulated time drift in ticks */

/* Exported variables -------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static int stimer_compare_func(const void *node1, const void *node2);
static uint32_t stimer_get_tick_internal(void);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief  Initialize software timer module
 * @param  get_tick_func: Function pointer to get system tick
 * @retval 0 for success, -1 for failure
 */
int stimer_init(STIMER_GET_TICK_FUNC_T get_tick_func)
{
    int ret = 0;
    list_node_init(&stimer_list);

    /* Initialize the tick function */
    if (get_tick_func != NULL) {
        stimer_tick_func = get_tick_func;
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief  Create a software timer
 * @param  timer: Timer handle
 * @param  period: Timer period
 * @param  auto_reload: Whether to auto reload
 * @param  callback: Callback function
 * @param  callback_arg: Argument passed to callback function
 * @retval 0 for success, -EINVAL for failure
 */
int stimer_create(stimer_t* timer, uint32_t period, uint8_t auto_reload, void (*callback)(void*), void *callback_arg)
{
    /* Parameter check */
    if (timer == NULL || callback == NULL) {
        return -EINVAL;
    }

    /* Initialize timer parameters */
    timer->period = period;
    timer->expire_time = 0;             /* Set when started */
    timer->auto_reload = auto_reload;
    timer->active = 0;
    timer->callback = callback;
    timer->callback_arg = callback_arg;
    list_node_init(&timer->node);

    return 0;
}

/**
 * @brief  Delete a software timer
 * @param  timer: Timer handle
 * @retval None
 */
void stimer_delete(stimer_t* timer)
{
    /* Parameter check */
    if (timer == NULL)
        return;

    /* Remove from list */
    if (!list_empty(&stimer_list) && !list_empty(&timer->node)) {
        list_del(&timer->node);
    }
}

/**
 * @brief  Start a software timer
 * @param  timer: Timer handle
 * @retval 0 for success, -EINVAL for failure
 */
int stimer_start(stimer_t* timer)
{
    /* Parameter check */
    if (timer == NULL)
        return -EINVAL;

    /* Remove from list first (if already in the list) */
    if (!list_empty(&timer->node)) {
        list_del(&timer->node);
    }

    /* Activate timer and set expiration time */
    timer->active = 1;
    timer->expire_time = stimer_get_tick_internal() + timer->period;

    /* Insert into list sorted by expiration time */
    sorted_insert_list(&stimer_list, &timer->node, stimer_compare_func);

    return 0;
}

/**
 * @brief  Stop a software timer
 * @param  timer: Timer handle
 * @retval 0 for success, -EINVAL for failure
 */
int stimer_stop(stimer_t* timer)
{
    /* Parameter check */
    if (timer == NULL)
        return -EINVAL;

    /* Stop timer */
    timer->active = 0;

    /* Remove from list */
    if (!list_empty(&stimer_list) && !list_empty(&timer->node)) {
        list_del(&timer->node);
    }

    return 0;
}

/**
 * @brief  Change the period of a software timer
 * @param  timer: Timer handle
 * @param  period: New timer period
 * @retval 0 for success, -EINVAL for failure
 */
int stimer_change_period(stimer_t* timer, uint32_t period)
{
    /* Parameter check */
    if (timer == NULL)
        return -EINVAL;

    /* Change period */
    timer->period = period;

    /* If timer is active, recalculate expiration time and reinsert */
    if (timer->active) {
        uint32_t current_tick = stimer_get_tick_internal();

        /* Remove from list */
        if (!list_empty(&timer->node)) {
            list_del(&timer->node);
        }

        /* Update expiration time and reinsert */
        timer->expire_time = current_tick + timer->period;
        sorted_insert_list(&stimer_list, &timer->node, stimer_compare_func);
    }

    return 0;
}

/**
 * @brief  Reset a software timer
 * @param  timer: Timer handle
 * @retval 0 for success, -EINVAL for failure
 */
int stimer_reset(stimer_t* timer)
{
    /* Parameter check */
    if (timer == NULL)
        return -EINVAL;

    /* Reset timer */
    if (timer->active) {
        /* Remove from list */
        if (!list_empty(&timer->node)) {
            list_del(&timer->node);
        }

        /* Update expiration time and reinsert */
        timer->expire_time = stimer_get_tick_internal() + timer->period;
        sorted_insert_list(&stimer_list, &timer->node, stimer_compare_func);
    }

    return 0;
}

/**
 * @brief  Get the status of a software timer
 * @param  timer: Timer handle
 * @retval 0 for inactive, 1 for active
 */
int8_t stimer_get_status(stimer_t* timer)
{
    if (timer == NULL)
        return -EINVAL;

    return timer->active;
}

/**
 * @brief  Software timer service function, should be called periodically in the main loop
 * @param  None
 * @retval None
 */
void stimer_service(void)
{
    list_t *pos, *next;
    stimer_t *timer;
    uint32_t current_time;
    uint32_t drift;

    /* Check if the list is empty */
    if (list_empty(&stimer_list)) {
        return;
    }

    /* Get current time once for all timers */
    current_time = stimer_get_tick_internal();

    /* Calculate and compensate for time drift */
    if (last_service_time != 0) {
        drift = current_time - last_service_time;
        if (drift > 1) {  /* If more than 1 tick has passed */
            accumulated_drift += (drift - 1);
        }
    }
    last_service_time = current_time;

    /* Traverse the list and check if any timer has expired */
    list_for_each_safe(pos, next, &stimer_list) {
        /* Get timer from the list node */
        timer = list_entry(pos, stimer_t, node);

        /* Check if timer has expired, considering accumulated drift */
        if (timer->active && ((current_time + accumulated_drift) >= timer->expire_time)) {
            if (!timer->auto_reload) {
                /* One-shot mode */
                timer->active = 0;
                list_del(pos);

                /* Call callback function */
                if (timer->callback != NULL) {
                    timer->callback(timer->callback_arg);
                }
            } else {
                /* Auto-reload mode */
                /* Call callback function first */
                if (timer->callback != NULL) {
                    timer->callback(timer->callback_arg);
                }

                /* Remove from current position */
                list_del(pos);

                /* Update expiration time with drift compensation */
                timer->expire_time = current_time + timer->period;

                /* Reinsert into sorted list */
                sorted_insert_list(&stimer_list, &timer->node, stimer_compare_func);
            }
        } else {
            /* Since list is sorted, we can break if we find a timer that hasn't expired */
            break;
        }
    }

    /* Reset accumulated drift after processing */
    accumulated_drift = 0;
}

/**
 * @brief  Internal function to get system tick using the function pointer
 * @param  None
 * @retval Current system time
 */
static uint32_t stimer_get_tick_internal(void)
{
    /* Check if tick function is initialized */
    if (stimer_tick_func == NULL) {
        return 0; /* Return 0 if tick function is not initialized */
    }

    /* Call the tick function */
    return stimer_tick_func();
}

/**
 * @brief  Compare function for timer sorting
 * @param  node1: First timer node
 * @param  node2: Second timer node
 * @retval < 0 if node1 expires before node2, 0 if equal, > 0 if node1 expires after node2
 */
static int stimer_compare_func(const void *node1, const void *node2)
{
    stimer_t *timer1 = list_entry((list_t *)node1, stimer_t, node);
    stimer_t *timer2 = list_entry((list_t *)node2, stimer_t, node);

    /* 使用安全的比较方法，避免溢出问题 */
    if (timer1->expire_time < timer2->expire_time) {
        return -1;
    } else if (timer1->expire_time > timer2->expire_time) {
        return 1;
    }
    return 0;
}
