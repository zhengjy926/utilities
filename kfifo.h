/**
  ******************************************************************************
  * @file        : kfifo.h
  * @author      : ZJY
  * @version     : V1.0
  * @date        : 20xx-xx-xx
  * @brief       : 
  * @attention   : None
  ******************************************************************************
  * @history     :
  *         V1.0 : 1.xxx
  ******************************************************************************
  */
#ifndef __KFIFO_H__
#define __KFIFO_H__

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "sys_def.h"
/* Exported define -----------------------------------------------------------*/

/* Exported typedef ----------------------------------------------------------*/
typedef struct
{
	volatile uint32_t in;
	volatile uint32_t out;
	size_t            mask;
	size_t            esize;
	void              *data;
}kfifo_t;

/* Exported macro ------------------------------------------------------------*/

/* Exported variable prototypes ----------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/
/**
 * kfifo_initialized - Check if the fifo is initialized
 * @fifo: address of the fifo to check
 *
 * Return %true if fifo is initialized, otherwise %false.
 * Assumes the fifo was 0 before.
 */
#define kfifo_initialized(fifo) ((fifo)->mask)

/**
 * kfifo_esize - returns the size of the element managed by the fifo
 * @fifo: address of the fifo to be used
 */
#define kfifo_esize(fifo)	((fifo)->esize)

/**
 * kfifo_size - returns the size of the fifo in elements
 * @fifo: address of the fifo to be used
 */
#define kfifo_size(fifo)	((fifo)->mask + 1)

/**
 * kfifo_reset - removes the entire fifo content
 * @fifo: address of the fifo to be used
 *
 * Note: usage of kfifo_reset() is dangerous. It should be only called when the
 * fifo is exclusived locked or when it is secured that no other thread is
 * accessing the fifo.
 */
#define kfifo_reset(fifo)   ((fifo)->in = (fifo)->out = 0)

/**
 * kfifo_reset_out - skip fifo content
 * @fifo: address of the fifo to be used
 *
 * Note: The usage of kfifo_reset_out() is safe until it will be only called
 * from the reader thread and there is only one concurrent reader. Otherwise
 * it is dangerous and must be handled in the same way as kfifo_reset().
 */
#define kfifo_reset_out(fifo)	((fifo)->in = (fifo)->out)

/**
 * kfifo_len - returns the number of used elements in the fifo
 * @fifo: address of the fifo to be used
 */
#define kfifo_len(fifo)     ((fifo)->in - (fifo)->out)

/**
 * kfifo_is_empty - returns true if the fifo is empty
 * @fifo: address of the fifo to be used
 */
static inline bool kfifo_is_empty(const kfifo_t *fifo)
{
    return ((fifo)->in == (fifo)->out);
}

/**
 * kfifo_is_full - returns true if the fifo is full
 * @fifo: address of the fifo to be used
 */
static inline bool kfifo_is_full(const kfifo_t *fifo)
{
    return (kfifo_len(fifo) > fifo->mask);
}


/**
 * kfifo_avail - returns the number of unused elements in the fifo
 * @fifo: address of the fifo to be used
 */
static inline size_t kfifo_avail(const kfifo_t *fifo)
{
    return (kfifo_size(fifo) - kfifo_len(fifo));
}

/**
 * kfifo_skip_count - skip output data
 * @fifo: address of the fifo to be used
 * @count: count of data to skip
 */
static inline void kfifo_skip_count(kfifo_t *fifo, size_t count)
{
    fifo->out += count;
}

/**
 * kfifo_skip - skip output data
 * @fifo: address of the fifo to be used
 */
#define	kfifo_skip(fifo)	kfifo_skip_count(fifo, 1)


int    kfifo_init             (kfifo_t *fifo, void *buffer, size_t size, size_t esize);
size_t kfifo_in               (kfifo_t *fifo, const void *buf, size_t len);
size_t kfifo_in_locked        (kfifo_t *fifo, const void *buf, size_t len);
size_t kfifo_out              (kfifo_t *fifo, void *buf, size_t len);
size_t kfifo_out_locked       (kfifo_t *fifo, void *buf, size_t len);
size_t kfifo_out_peek         (kfifo_t *fifo, void *buf, size_t len);
size_t kfifo_out_linear       (kfifo_t *fifo, size_t *tail, size_t n);
size_t kfifo_out_linear_locked(kfifo_t *fifo, size_t *tail, size_t n);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __KFIFO_H__ */


