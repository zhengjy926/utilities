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
#ifndef KFIFO_H
#define KFIFO_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>

/* Exported typedef ----------------------------------------------------------*/
/**
 * @brief FIFO buffer structure
 * @details This structure represents a circular buffer (FIFO) with support for
 *          variable element sizes and thread-safe operations.
 */
typedef struct
{
    volatile unsigned int in;    /**< Input index */
    volatile unsigned int out;   /**< Output index */
    unsigned int          mask;  /**< Size mask (size - 1) */
    unsigned int          esize; /**< Element size in bytes */
    void                 *data;  /**< Pointer to buffer data */
} kfifo_t;

/* Inline helpers (replace non-MISRA macros) ---------------------------------*/

/**
 * @brief Check if the FIFO is initialized
 * @param fifo Pointer to the FIFO structure to check
 * @return 1U if FIFO is initialized, 0U otherwise
 * @note  Assumes the FIFO was zero-initialized before use
 */
static inline uint8_t kfifo_initialized(const kfifo_t *fifo)
{
    return (fifo->mask != 0U) ? 1U : 0U;
}

/**
 * @brief Get the element size of the FIFO
 * @param fifo Pointer to the FIFO structure
 * @return Size of each element in bytes
 */
static inline unsigned int kfifo_esize(const kfifo_t *fifo)
{
    return fifo->esize;
}

/**
 * @brief Get the total size of the FIFO in elements
 * @param fifo Pointer to the FIFO structure
 * @return Total number of elements the FIFO can hold
 */
static inline unsigned int kfifo_size(const kfifo_t *fifo)
{
    return fifo->mask + 1U;
}

/**
 * @brief Reset the FIFO buffer, removing all content
 * @param fifo Pointer to the FIFO structure to reset
 * @warning This operation is dangerous. It should only be called when the
 *          FIFO is exclusively locked or when it is secured that no other
 *          thread is accessing the FIFO.
 */
static inline void kfifo_reset(kfifo_t *fifo)
{
    fifo->in  = 0U;
    fifo->out = 0U;
}

/**
 * @brief Skip all FIFO content by moving output pointer to input pointer
 * @param fifo Pointer to the FIFO structure
 * @note  Safe only when called from the reader thread with a single reader.
 */
static inline void kfifo_reset_out(kfifo_t *fifo)
{
    fifo->out = fifo->in;
}

/**
 * @brief Get the number of used elements in the FIFO
 * @param fifo Pointer to the FIFO structure
 * @return Number of elements currently stored in the FIFO
 */
static inline unsigned int kfifo_len(const kfifo_t *fifo)
{
    return fifo->in - fifo->out;
}

/**
 * @brief Check if the FIFO is empty
 * @param fifo Pointer to the FIFO structure
 * @return 1U if FIFO is empty, 0U otherwise
 */
static inline uint8_t kfifo_is_empty(const kfifo_t *fifo)
{
    return (fifo->in == fifo->out) ? 1U : 0U;
}

/**
 * @brief Check if the FIFO is full
 * @param fifo Pointer to the FIFO structure
 * @return 1U if FIFO is full, 0U otherwise
 */
static inline uint8_t kfifo_is_full(const kfifo_t *fifo)
{
    return (kfifo_len(fifo) > fifo->mask) ? 1U : 0U;
}

/**
 * @brief Get the number of available (unused) elements in the FIFO
 * @param fifo Pointer to the FIFO structure
 * @return Number of elements that can still be added to the FIFO
 */
static inline unsigned int kfifo_avail(const kfifo_t *fifo)
{
    return kfifo_size(fifo) - kfifo_len(fifo);
}

/**
 * @brief Skip a specified number of output elements
 * @param fifo  Pointer to the FIFO structure
 * @param count Number of elements to skip
 */
static inline void kfifo_skip_count(kfifo_t *fifo, unsigned int count)
{
    fifo->out += count;
}

/**
 * @brief Skip one output element
 * @param fifo Pointer to the FIFO structure
 */
static inline void kfifo_skip(kfifo_t *fifo)
{
    kfifo_skip_count(fifo, 1U);
}

/* Exported function prototypes ----------------------------------------------*/
int          kfifo_init             (kfifo_t *fifo,
                                     void *buffer,
                                     unsigned int size,
                                     unsigned int esize);
                                     
unsigned int kfifo_in               (kfifo_t *fifo,
                                     const void *buf,
                                     unsigned int len);
                                     
unsigned int kfifo_in_locked        (kfifo_t *fifo,
                                     const void *buf,
                                     unsigned int len);
                                     
unsigned int kfifo_out              (kfifo_t *fifo,
                                     void *buf,
                                     unsigned int len);
                                     
unsigned int kfifo_out_locked       (kfifo_t *fifo,
                                     void *buf,
                                     unsigned int len);
                                     
unsigned int kfifo_out_peek         (kfifo_t *fifo,
                                     void *buf,
                                     unsigned int len);
                                     
unsigned int kfifo_out_linear       (kfifo_t *fifo,
                                     unsigned int *tail,
                                     unsigned int n);
                                     
unsigned int kfifo_out_linear_locked(kfifo_t *fifo,
                                     unsigned int *tail,
                                     unsigned int n);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* KFIFO_H */
