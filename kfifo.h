/**
 * @file        kfifo.h
 * @author      ZJY
 * @version     V1.0
 * @date        20xx-xx-xx
 * @brief       Kernel FIFO implementation header file
 * @details     This file contains the definition of a kernel-style FIFO buffer
 *              implementation with thread-safe operations.
 * @attention   None
 * @history
 * | Version | Description |
 * |---------|-------------|
 * | V1.0    | Initial implementation |
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
/**
 * @brief FIFO buffer structure
 * @details This structure represents a circular buffer (FIFO) with support for
 *          variable element sizes and thread-safe operations.
 */
typedef struct
{
	volatile uint32_t in;    /**< Input index */
	volatile uint32_t out;   /**< Output index */
	size_t            mask;  /**< Size mask (size - 1) */
	size_t            esize; /**< Element size in bytes */
	void              *data; /**< Pointer to buffer data */
}kfifo_t;

/* Exported macro ------------------------------------------------------------*/
/**
 * @brief Check if the FIFO is initialized
 * @param fifo Pointer to the FIFO structure to check
 * @return true if FIFO is initialized, false otherwise
 * @note Assumes the FIFO was zero-initialized before use
 */
#define kfifo_initialized(fifo) ((fifo)->mask)

/**
 * @brief Get the element size of the FIFO
 * @param fifo Pointer to the FIFO structure
 * @return Size of each element in bytes
 */
#define kfifo_esize(fifo)	((fifo)->esize)

/**
 * @brief Get the total size of the FIFO in elements
 * @param fifo Pointer to the FIFO structure
 * @return Total number of elements the FIFO can hold
 */
#define kfifo_size(fifo)	((fifo)->mask + 1)

/**
 * @brief Reset the FIFO buffer, removing all content
 * @param fifo Pointer to the FIFO structure to reset
 * @warning This operation is dangerous. It should only be called when the
 *          FIFO is exclusively locked or when it is secured that no other
 *          thread is accessing the FIFO.
 */
#define kfifo_reset(fifo) \
(void)({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	__tmp->in = __tmp->out = 0; \
})

/**
 * @brief Skip all FIFO content by moving output pointer to input pointer
 * @param fifo Pointer to the FIFO structure
 * @note The usage of kfifo_reset_out() is safe until it will be only called
 *       from the reader thread and there is only one concurrent reader.
 *       Otherwise it is dangerous and must be handled in the same way as
 *       kfifo_reset().
 */
#define kfifo_reset_out(fifo) \
(void)({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	__tmp->out = __tmp->in; \
})

/**
 * @brief Get the number of used elements in the FIFO
 * @param fifo Pointer to the FIFO structure
 * @return Number of elements currently stored in the FIFO
 */
#define kfifo_len(fifo) \
({ \
	typeof((fifo) + 1) __tmpl = (fifo); \
	__tmpl->in - __tmpl->out; \
})

/**
 * @brief Check if the FIFO is empty
 * @param fifo Pointer to the FIFO structure
 * @return true if FIFO is empty, false otherwise
 */
#define	kfifo_is_empty(fifo) \
({ \
	typeof((fifo) + 1) __tmpq = (fifo); \
	__tmpq->in == __tmpq->out; \
})

/**
 * @brief Check if the FIFO is full
 * @param fifo Pointer to the FIFO structure
 * @return true if FIFO is full, false otherwise
 */
#define	kfifo_is_full(fifo) \
({ \
	typeof((fifo) + 1) __tmpq = (fifo); \
	kfifo_len(__tmpq) > __tmpq->mask; \
})

/**
 * @brief Get the number of available (unused) elements in the FIFO
 * @param fifo Pointer to the FIFO structure
 * @return Number of elements that can still be added to the FIFO
 */
#define	kfifo_avail(fifo) \
({ \
	typeof((fifo) + 1) __tmpq = (fifo); \
	unsigned int __avail = kfifo_size(__tmpq) - kfifo_len(__tmpq); \
	__avail; \
}) \

/**
 * @brief Skip a specified number of output elements
 * @param fifo Pointer to the FIFO structure
 * @param count Number of elements to skip
 */
#define	kfifo_skip_count(fifo, count) do { \
	typeof((fifo) + 1) __tmp = (fifo); \
    __tmp->out += (count); \
} while(0)

/**
 * @brief Skip one output element
 * @param fifo Pointer to the FIFO structure
 */
#define	kfifo_skip(fifo)	kfifo_skip_count(fifo, 1)

/* Exported variable prototypes ----------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/
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


